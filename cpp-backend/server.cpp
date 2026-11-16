/**
 * ============================================================================
 * ONLINE EXAMINATION SYSTEM - C++17 REST API & STATIC WEB SERVER
 * ============================================================================
 * High-performance, zero-dependency C++ backend server for the Online Examination
 * System. Communicates directly with SQLite 3 (online_exam.db) using Windows native
 * SQLite runtime and Windows Sockets (Winsock2).
 *
 * Architecture:
 * - C++17 Standard
 * - Windows Sockets 2 (ws2_32)
 * - Windows Native SQLite 3 (winsqlite3)
 * - Windows Native Cryptography (wincrypt) for SHA-256 password security
 * - Multi-threaded HTTP request dispatcher
 * ============================================================================
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wincrypt.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <filesystem>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")

namespace fs = std::filesystem;

// ============================================================================
// 1. NATIVE SQLITE 3 C-API BINDINGS (via Windows winsqlite3.dll)
// ============================================================================

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

typedef int (*fn_sqlite3_open)(const char *filename, sqlite3 **ppDb);
typedef int (*fn_sqlite3_close)(sqlite3 *);
typedef int (*fn_sqlite3_exec)(sqlite3*, const char *sql, int (*callback)(void*,int,char**,char**), void *, char **errmsg);
typedef int (*fn_sqlite3_prepare_v2)(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
typedef int (*fn_sqlite3_step)(sqlite3_stmt*);
typedef int (*fn_sqlite3_column_int)(sqlite3_stmt*, int iCol);
typedef double (*fn_sqlite3_column_double)(sqlite3_stmt*, int iCol);
typedef const unsigned char *(*fn_sqlite3_column_text)(sqlite3_stmt*, int iCol);
typedef int (*fn_sqlite3_finalize)(sqlite3_stmt *pStmt);
typedef const char *(*fn_sqlite3_errmsg)(sqlite3*);
typedef int (*fn_sqlite3_bind_text)(sqlite3_stmt*, int, const char*, int, void(*)(void*));
typedef int (*fn_sqlite3_bind_int)(sqlite3_stmt*, int, int);
typedef int (*fn_sqlite3_bind_double)(sqlite3_stmt*, int, double);
typedef int (*fn_sqlite3_column_count)(sqlite3_stmt*);
typedef const char *(*fn_sqlite3_column_name)(sqlite3_stmt*, int N);

static fn_sqlite3_open g_sqlite3_open = nullptr;
static fn_sqlite3_close g_sqlite3_close = nullptr;
static fn_sqlite3_exec g_sqlite3_exec = nullptr;
static fn_sqlite3_prepare_v2 g_sqlite3_prepare_v2 = nullptr;
static fn_sqlite3_step g_sqlite3_step = nullptr;
static fn_sqlite3_column_int g_sqlite3_column_int = nullptr;
static fn_sqlite3_column_double g_sqlite3_column_double = nullptr;
static fn_sqlite3_column_text g_sqlite3_column_text = nullptr;
static fn_sqlite3_finalize g_sqlite3_finalize = nullptr;
static fn_sqlite3_errmsg g_sqlite3_errmsg = nullptr;
static fn_sqlite3_bind_text g_sqlite3_bind_text = nullptr;
static fn_sqlite3_bind_int g_sqlite3_bind_int = nullptr;
static fn_sqlite3_bind_double g_sqlite3_bind_double = nullptr;
static fn_sqlite3_column_count g_sqlite3_column_count = nullptr;
static fn_sqlite3_column_name g_sqlite3_column_name = nullptr;

static HMODULE g_hSqliteDll = nullptr;
static std::mutex g_dbMutex;

bool initSqliteLibrary() {
    g_hSqliteDll = LoadLibraryA("winsqlite3.dll");
    if (!g_hSqliteDll) {
        g_hSqliteDll = LoadLibraryA("sqlite3.dll");
    }
    if (!g_hSqliteDll) {
        std::cerr << "[C++ Backend ERROR] Could not load winsqlite3.dll or sqlite3.dll" << std::endl;
        return false;
    }

    g_sqlite3_open = (fn_sqlite3_open)GetProcAddress(g_hSqliteDll, "sqlite3_open");
    g_sqlite3_close = (fn_sqlite3_close)GetProcAddress(g_hSqliteDll, "sqlite3_close");
    g_sqlite3_exec = (fn_sqlite3_exec)GetProcAddress(g_hSqliteDll, "sqlite3_exec");
    g_sqlite3_prepare_v2 = (fn_sqlite3_prepare_v2)GetProcAddress(g_hSqliteDll, "sqlite3_prepare_v2");
    g_sqlite3_step = (fn_sqlite3_step)GetProcAddress(g_hSqliteDll, "sqlite3_step");
    g_sqlite3_column_int = (fn_sqlite3_column_int)GetProcAddress(g_hSqliteDll, "sqlite3_column_int");
    g_sqlite3_column_double = (fn_sqlite3_column_double)GetProcAddress(g_hSqliteDll, "sqlite3_column_double");
    g_sqlite3_column_text = (fn_sqlite3_column_text)GetProcAddress(g_hSqliteDll, "sqlite3_column_text");
    g_sqlite3_finalize = (fn_sqlite3_finalize)GetProcAddress(g_hSqliteDll, "sqlite3_finalize");
    g_sqlite3_errmsg = (fn_sqlite3_errmsg)GetProcAddress(g_hSqliteDll, "sqlite3_errmsg");
    g_sqlite3_bind_text = (fn_sqlite3_bind_text)GetProcAddress(g_hSqliteDll, "sqlite3_bind_text");
    g_sqlite3_bind_int = (fn_sqlite3_bind_int)GetProcAddress(g_hSqliteDll, "sqlite3_bind_int");
    g_sqlite3_bind_double = (fn_sqlite3_bind_double)GetProcAddress(g_hSqliteDll, "sqlite3_bind_double");
    g_sqlite3_column_count = (fn_sqlite3_column_count)GetProcAddress(g_hSqliteDll, "sqlite3_column_count");
    g_sqlite3_column_name = (fn_sqlite3_column_name)GetProcAddress(g_hSqliteDll, "sqlite3_column_name");

    return (g_sqlite3_open && g_sqlite3_prepare_v2 && g_sqlite3_step && g_sqlite3_finalize);
}

// ============================================================================
// 2. CRYPTOGRAPHIC PASSWORD HASHING (Native Windows CryptoAPI SHA-256)
// ============================================================================

std::string computeSha256Hex(const std::string& input) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    std::string hexResult = "";

    if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
            if (CryptHashData(hHash, (const BYTE*)input.c_str(), (DWORD)input.length(), 0)) {
                BYTE hashBytes[32];
                DWORD hashLen = sizeof(hashBytes);
                if (CryptGetHashParam(hHash, HP_HASHVAL, hashBytes, &hashLen, 0)) {
                    char hexBuf[3];
                    for (DWORD i = 0; i < hashLen; ++i) {
                        snprintf(hexBuf, sizeof(hexBuf), "%02x", hashBytes[i]);
                        hexResult += hexBuf;
                    }
                }
            }
            CryptDestroyHash(hHash);
        }
        CryptReleaseContext(hProv, 0);
    }
    return hexResult;
}

std::string generateRandomSalt(size_t length = 16) {
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";
    std::string salt = "";
    HCRYPTPROV hProv = 0;
    if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        std::vector<BYTE> randomBytes(length);
        if (CryptGenRandom(hProv, (DWORD)length, randomBytes.data())) {
            for (size_t i = 0; i < length; ++i) {
                salt += charset[randomBytes[i] % (sizeof(charset) - 1)];
            }
        }
        CryptReleaseContext(hProv, 0);
    }
    if (salt.empty()) {
        for (size_t i = 0; i < length; ++i) {
            salt += charset[rand() % (sizeof(charset) - 1)];
        }
    }
    return salt;
}

std::string hashPasswordWithSalt(const std::string& plainText, const std::string& salt) {
    return computeSha256Hex(salt + plainText + salt);
}

// ============================================================================
// 3. UTILITIES & JSON HELPERS
// ============================================================================

std::string escapeJson(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b";  break;
            case '\f': oss << "\\f";  break;
            case '\n': oss << "\\n";  break;
            case '\r': oss << "\\r";  break;
            case '\t': oss << "\\t";  break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", (int)c);
                    oss << buf;
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

std::string extractJsonField(const std::string& json, const std::string& field) {
    std::string searchKey = "\"" + field + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "";

    pos += searchKey.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == ':' || json[pos] == '\t')) {
        pos++;
    }

    if (pos >= json.length()) return "";

    if (json[pos] == '"') {
        pos++;
        size_t endPos = json.find('"', pos);
        if (endPos != std::string::npos) {
            return json.substr(pos, endPos - pos);
        }
    } else {
        size_t endPos = json.find_first_of(",}\r\n ", pos);
        if (endPos != std::string::npos) {
            return json.substr(pos, endPos - pos);
        } else {
            return json.substr(pos);
        }
    }
    return "";
}

std::string toLowerString(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
    return str;
}

std::string getMimeType(const std::string& ext) {
    if (ext == ".html" || ext == ".htm") return "text/html; charset=utf-8";
    if (ext == ".css") return "text/css; charset=utf-8";
    if (ext == ".js") return "application/javascript; charset=utf-8";
    if (ext == ".json") return "application/json; charset=utf-8";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".gif") return "image/gif";
    if (ext == ".svg") return "image/svg+xml";
    if (ext == ".ico") return "image/x-icon";
    if (ext == ".mp3") return "audio/mpeg";
    if (ext == ".wav") return "audio/wav";
    return "application/octet-stream";
}

// ============================================================================
// 4. DATABASE MANAGER CLASS (SQLite 3 C++ Implementation)
// ============================================================================

class CppDatabaseManager {
public:
    static CppDatabaseManager& instance() {
        static CppDatabaseManager inst;
        return inst;
    }

    bool open(const std::string& dbPath = "online_exam.db") {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!initSqliteLibrary()) return false;
        m_dbPath = dbPath;
        int rc = g_sqlite3_open(dbPath.c_str(), &m_db);
        if (rc != 0) {
            std::cerr << "[C++ Database] Failed to open SQLite DB: " << g_sqlite3_errmsg(m_db) << std::endl;
            return false;
        }

        // Enable foreign keys
        char* errMsg = nullptr;
        g_sqlite3_exec(m_db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errMsg);

        createTablesIfNotExist();
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (m_db) {
            g_sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

    std::string getStatusJson() {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        std::ostringstream oss;
        oss << "{"
            << "\"status\":\"connected\","
            << "\"database_file\":\"" << escapeJson(m_dbPath) << "\","
            << "\"backend\":\"C++17 Winsock & Native SQLite Engine\","
            << "\"active_tables\":[\"users\",\"quizzes\",\"questions\",\"results\"]"
            << "}";
        return oss.str();
    }

    std::string getStudentsJson() {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!m_db) return "[]";

        const char* sql = "SELECT id, username, specific_id, full_name, email, role, created_at FROM users WHERE role = 'student'";
        sqlite3_stmt* stmt = nullptr;
        int rc = g_sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
        if (rc != 0) return "[]";

        std::ostringstream oss;
        oss << "[";
        bool first = true;
        while (g_sqlite3_step(stmt) == 100) { // 100 = SQLITE_ROW
            if (!first) oss << ",";
            first = false;

            const char* user = (const char*)g_sqlite3_column_text(stmt, 1);
            const char* spec = (const char*)g_sqlite3_column_text(stmt, 2);
            const char* name = (const char*)g_sqlite3_column_text(stmt, 3);
            const char* mail = (const char*)g_sqlite3_column_text(stmt, 4);
            const char* date = (const char*)g_sqlite3_column_text(stmt, 6);

            oss << "{"
                << "\"id\":\"" << (spec ? escapeJson(spec) : (user ? escapeJson(user) : "")) << "\","
                << "\"username\":\"" << (user ? escapeJson(user) : "") << "\","
                << "\"name\":\"" << (name ? escapeJson(name) : (user ? escapeJson(user) : "")) << "\","
                << "\"email\":\"" << (mail ? escapeJson(mail) : "") << "\","
                << "\"role\":\"student\","
                << "\"program\":\"Computer Science\","
                << "\"created_at\":\"" << (date ? escapeJson(date) : "") << "\""
                << "}";
        }
        g_sqlite3_finalize(stmt);
        oss << "]";
        return oss.str();
    }

    std::string getExamsJson(const std::string& adminId = "") {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!m_db) return "[]";

        std::string sql = "SELECT id, title, category, duration_minutes, pass_percentage, total_marks, created_by, created_at FROM quizzes ORDER BY created_at DESC";

        sqlite3_stmt* stmt = nullptr;
        int rc = g_sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != 0) return "[]";

        std::ostringstream oss;
        oss << "[";
        bool first = true;
        while (g_sqlite3_step(stmt) == 100) {
            if (!first) oss << ",";
            first = false;

            const char* id = (const char*)g_sqlite3_column_text(stmt, 0);
            const char* title = (const char*)g_sqlite3_column_text(stmt, 1);
            const char* cat = (const char*)g_sqlite3_column_text(stmt, 2);
            int duration = g_sqlite3_column_int(stmt, 3);
            double passPct = g_sqlite3_column_double(stmt, 4);
            int totalMarks = g_sqlite3_column_int(stmt, 5);
            const char* creator = (const char*)g_sqlite3_column_text(stmt, 6);
            const char* date = (const char*)g_sqlite3_column_text(stmt, 7);

            oss << "{"
                << "\"id\":\"" << (id ? escapeJson(id) : "") << "\","
                << "\"title\":\"" << (title ? escapeJson(title) : "") << "\","
                << "\"category\":\"" << (cat ? escapeJson(cat) : "") << "\","
                << "\"durationMinutes\":" << duration << ","
                << "\"passingPercentage\":" << passPct << ","
                << "\"totalMarks\":" << totalMarks << ","
                << "\"created_by\":\"" << (creator ? escapeJson(creator) : "") << "\","
                << "\"created_at\":\"" << (date ? escapeJson(date) : "") << "\""
                << "}";
        }
        g_sqlite3_finalize(stmt);
        oss << "]";
        return oss.str();
    }

    bool saveExam(const std::string& id, const std::string& title, const std::string& cat,
                  int duration, double passPct, int totalMarks, const std::string& createdBy,
                  std::string& errorMsg) {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!m_db) {
            errorMsg = "Database not open";
            return false;
        }

        // Enforce ownership: if exam exists, creator must match
        const char* checkSql = "SELECT created_by FROM quizzes WHERE id = ?";
        sqlite3_stmt* checkStmt = nullptr;
        if (g_sqlite3_prepare_v2(m_db, checkSql, -1, &checkStmt, nullptr) == 0) {
            g_sqlite3_bind_text(checkStmt, 1, id.c_str(), -1, nullptr);
            if (g_sqlite3_step(checkStmt) == 100) {
                const char* existingOwner = (const char*)g_sqlite3_column_text(checkStmt, 0);
                if (existingOwner && strlen(existingOwner) > 0 && !createdBy.empty()) {
                    if (toLowerString(existingOwner) != toLowerString(createdBy)) {
                        g_sqlite3_finalize(checkStmt);
                        errorMsg = "Access Denied: You do not have permission to modify an examination created by another administrator.";
                        return false;
                    }
                }
            }
            g_sqlite3_finalize(checkStmt);
        }

        const char* sql = "INSERT OR REPLACE INTO quizzes (id, title, category, duration_minutes, pass_percentage, total_marks, created_by, created_at) "
                          "VALUES (?, ?, ?, ?, ?, ?, ?, datetime('now'))";
        sqlite3_stmt* stmt = nullptr;
        if (g_sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != 0) {
            errorMsg = "Statement prepare failed";
            return false;
        }

        g_sqlite3_bind_text(stmt, 1, id.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 2, title.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 3, cat.c_str(), -1, nullptr);
        g_sqlite3_bind_int(stmt, 4, duration);
        g_sqlite3_bind_double(stmt, 5, passPct);
        g_sqlite3_bind_int(stmt, 6, totalMarks);
        g_sqlite3_bind_text(stmt, 7, createdBy.c_str(), -1, nullptr);

        int rc = g_sqlite3_step(stmt);
        g_sqlite3_finalize(stmt);
        return (rc == 101); // 101 = SQLITE_DONE
    }

    bool deleteExam(const std::string& id, const std::string& adminId, std::string& errorMsg) {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!m_db) {
            errorMsg = "Database not open";
            return false;
        }

        // Enforce ownership: only creator can delete
        const char* checkSql = "SELECT created_by FROM quizzes WHERE id = ?";
        sqlite3_stmt* checkStmt = nullptr;
        if (g_sqlite3_prepare_v2(m_db, checkSql, -1, &checkStmt, nullptr) == 0) {
            g_sqlite3_bind_text(checkStmt, 1, id.c_str(), -1, nullptr);
            if (g_sqlite3_step(checkStmt) == 100) {
                const char* existingOwner = (const char*)g_sqlite3_column_text(checkStmt, 0);
                if (existingOwner && strlen(existingOwner) > 0 && !adminId.empty()) {
                    if (toLowerString(existingOwner) != toLowerString(adminId)) {
                        g_sqlite3_finalize(checkStmt);
                        errorMsg = "Access Denied: You cannot delete an examination created by another administrator.";
                        return false;
                    }
                }
            }
            g_sqlite3_finalize(checkStmt);
        }

        const char* sql1 = "DELETE FROM questions WHERE quiz_id = ?";
        sqlite3_stmt* stmt1 = nullptr;
        if (g_sqlite3_prepare_v2(m_db, sql1, -1, &stmt1, nullptr) == 0) {
            g_sqlite3_bind_text(stmt1, 1, id.c_str(), -1, nullptr);
            g_sqlite3_step(stmt1);
            g_sqlite3_finalize(stmt1);
        }

        const char* sql2 = "DELETE FROM quizzes WHERE id = ?";
        sqlite3_stmt* stmt2 = nullptr;
        if (g_sqlite3_prepare_v2(m_db, sql2, -1, &stmt2, nullptr) == 0) {
            g_sqlite3_bind_text(stmt2, 1, id.c_str(), -1, nullptr);
            int rc = g_sqlite3_step(stmt2);
            g_sqlite3_finalize(stmt2);
            return (rc == 101);
        }
        return false;
    }
            return (rc == 101);
        }
        return false;
    }

    std::string getQuestionsJson() {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!m_db) return "{}";

        const char* sql = "SELECT id, quiz_id, question_text, option_a, option_b, option_c, option_d, correct_option, marks, explanation FROM questions";
        sqlite3_stmt* stmt = nullptr;
        if (g_sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != 0) return "{}";

        std::map<std::string, std::vector<std::string>> pool;
        while (g_sqlite3_step(stmt) == 100) {
            int qId = g_sqlite3_column_int(stmt, 0);
            const char* quizId = (const char*)g_sqlite3_column_text(stmt, 1);
            const char* text = (const char*)g_sqlite3_column_text(stmt, 2);
            const char* optA = (const char*)g_sqlite3_column_text(stmt, 3);
            const char* optB = (const char*)g_sqlite3_column_text(stmt, 4);
            const char* optC = (const char*)g_sqlite3_column_text(stmt, 5);
            const char* optD = (const char*)g_sqlite3_column_text(stmt, 6);
            int correct = g_sqlite3_column_int(stmt, 7);
            int marks = g_sqlite3_column_int(stmt, 8);
            const char* expl = (const char*)g_sqlite3_column_text(stmt, 9);

            std::string qKey = quizId ? quizId : "CS301";
            std::ostringstream qOss;
            qOss << "{"
                 << "\"id\":" << qId << ","
                 << "\"text\":\"" << (text ? escapeJson(text) : "") << "\","
                 << "\"options\":[\""
                 << (optA ? escapeJson(optA) : "") << "\",\""
                 << (optB ? escapeJson(optB) : "") << "\",\""
                 << (optC ? escapeJson(optC) : "") << "\",\""
                 << (optD ? escapeJson(optD) : "") << "\"],"
                 << "\"correct\":" << correct << ","
                 << "\"marks\":" << marks << ","
                 << "\"explanation\":\"" << (expl ? escapeJson(expl) : "") << "\""
                 << "}";
            pool[qKey].push_back(qOss.str());
        }
        g_sqlite3_finalize(stmt);

        std::ostringstream oss;
        oss << "{";
        bool firstKey = true;
        for (const auto& pair : pool) {
            if (!firstKey) oss << ",";
            firstKey = false;
            oss << "\"" << escapeJson(pair.first) << "\":[";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                if (i > 0) oss << ",";
                oss << pair.second[i];
            }
            oss << "]";
        }
        oss << "}";
        return oss.str();
    }

    std::string verifyAccount(const std::string& identifier, const std::string& email, const std::string& requiredRole) {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!m_db) return "{\"verified\":false,\"error\":\"Database not initialized\"}";

        const char* sql = "SELECT id, username, specific_id, full_name, email FROM users "
                          "WHERE role = ? AND (LOWER(username) = ? OR LOWER(specific_id) = ?) AND LOWER(email) = ?";
        sqlite3_stmt* stmt = nullptr;
        if (g_sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != 0) {
            return "{\"verified\":false,\"error\":\"Query preparation failed\"}";
        }

        std::string lowerIdent = toLowerString(identifier);
        std::string lowerEmail = toLowerString(email);

        g_sqlite3_bind_text(stmt, 1, requiredRole.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 2, lowerIdent.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 3, lowerIdent.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 4, lowerEmail.c_str(), -1, nullptr);

        int rc = g_sqlite3_step(stmt);
        if (rc == 100) {
            const char* user = (const char*)g_sqlite3_column_text(stmt, 1);
            const char* spec = (const char*)g_sqlite3_column_text(stmt, 2);
            const char* name = (const char*)g_sqlite3_column_text(stmt, 3);
            const char* mail = (const char*)g_sqlite3_column_text(stmt, 4);

            std::ostringstream oss;
            oss << "{"
                << "\"verified\":true,"
                << "\"username\":\"" << (user ? escapeJson(user) : "") << "\","
                << (requiredRole == "student" ? "\"roll_number\":\"" : "\"admin_id\":\"")
                << (spec ? escapeJson(spec) : (user ? escapeJson(user) : "")) << "\","
                << "\"full_name\":\"" << (name ? escapeJson(name) : "") << "\","
                << "\"email\":\"" << (mail ? escapeJson(mail) : "") << "\""
                << "}";
            g_sqlite3_finalize(stmt);
            return oss.str();
        }
        g_sqlite3_finalize(stmt);
        return "{\"verified\":false,\"error\":\"No matching " + requiredRole + " account found\"}";
    }

    bool resetPassword(const std::string& identifier, const std::string& newPassword) {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!m_db) return false;

        std::string salt = generateRandomSalt(16);
        std::string hashed = hashPasswordWithSalt(newPassword, salt);
        std::string lowerIdent = toLowerString(identifier);

        const char* sql = "UPDATE users SET password = ? WHERE LOWER(username) = ? OR LOWER(specific_id) = ?";
        sqlite3_stmt* stmt = nullptr;
        if (g_sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != 0) return false;

        g_sqlite3_bind_text(stmt, 1, hashed.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 2, lowerIdent.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 3, lowerIdent.c_str(), -1, nullptr);

        int rc = g_sqlite3_step(stmt);
        g_sqlite3_finalize(stmt);
        return (rc == 101);
    }

    bool registerUser(const std::string& username, const std::string& rawPassword, const std::string& fullName,
                      const std::string& email, const std::string& role, const std::string& specificId) {
        std::lock_guard<std::mutex> lock(g_dbMutex);
        if (!m_db) return false;

        std::string salt = generateRandomSalt(16);
        std::string hashed = hashPasswordWithSalt(rawPassword, salt);

        const char* sql = "INSERT INTO users (username, password, full_name, email, role, specific_id, created_at) "
                          "VALUES (?, ?, ?, ?, ?, ?, datetime('now'))";
        sqlite3_stmt* stmt = nullptr;
        if (g_sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != 0) return false;

        g_sqlite3_bind_text(stmt, 1, username.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 3, fullName.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 4, email.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 5, role.c_str(), -1, nullptr);
        g_sqlite3_bind_text(stmt, 6, specificId.c_str(), -1, nullptr);

        int rc = g_sqlite3_step(stmt);
        g_sqlite3_finalize(stmt);
        return (rc == 101);
    }

private:
    CppDatabaseManager() : m_db(nullptr), m_dbPath("online_exam.db") {}
    ~CppDatabaseManager() { close(); }

    void createTablesIfNotExist() {
        const char* schema = R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT UNIQUE NOT NULL,
                password TEXT NOT NULL,
                full_name TEXT NOT NULL,
                email TEXT UNIQUE NOT NULL,
                role TEXT NOT NULL,
                specific_id TEXT NOT NULL,
                created_at TEXT NOT NULL
            );

            CREATE TABLE IF NOT EXISTS quizzes (
                id TEXT PRIMARY KEY,
                title TEXT NOT NULL,
                category TEXT NOT NULL,
                duration_minutes INTEGER NOT NULL,
                pass_percentage REAL NOT NULL,
                total_marks INTEGER NOT NULL,
                created_at TEXT NOT NULL
            );

            CREATE TABLE IF NOT EXISTS questions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                quiz_id TEXT NOT NULL,
                question_text TEXT NOT NULL,
                option_a TEXT NOT NULL,
                option_b TEXT NOT NULL,
                option_c TEXT NOT NULL,
                option_d TEXT NOT NULL,
                correct_option INTEGER NOT NULL,
                marks INTEGER NOT NULL,
                explanation TEXT,
                FOREIGN KEY (quiz_id) REFERENCES quizzes(id) ON DELETE CASCADE
            );

            CREATE TABLE IF NOT EXISTS results (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                student_id TEXT NOT NULL,
                quiz_id TEXT NOT NULL,
                score INTEGER NOT NULL,
                total_marks INTEGER NOT NULL,
                percentage REAL NOT NULL,
                is_passed INTEGER NOT NULL,
                proctor_strikes INTEGER DEFAULT 0,
                attempt_date TEXT NOT NULL
            );
        )";
        char* err = nullptr;
        g_sqlite3_exec(m_db, schema, nullptr, nullptr, &err);
    }

    sqlite3* m_db;
    std::string m_dbPath;
};

// ============================================================================
// 5. HTTP SERVER & SOCKET CONNECTION HANDLER
// ============================================================================

void sendHttpResponse(SOCKET clientSock, int statusCode, const std::string& statusText,
                      const std::string& contentType, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Content-Length: " << body.length() << "\r\n"
             << "Access-Control-Allow-Origin: *\r\n"
             << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
             << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
             << "Connection: close\r\n\r\n"
             << body;
    std::string respStr = response.str();
    send(clientSock, respStr.c_str(), (int)respStr.length(), 0);
}

void handleClientConnection(SOCKET clientSock, std::string rootDir) {
    std::vector<char> buffer(65536);
    int bytesReceived = recv(clientSock, buffer.data(), (int)buffer.size() - 1, 0);
    if (bytesReceived <= 0) {
        closesocket(clientSock);
        return;
    }
    buffer[bytesReceived] = '\0';
    std::string rawRequest(buffer.data(), bytesReceived);

    // Parse Request Line
    std::istringstream stream(rawRequest);
    std::string method, path, httpVersion;
    stream >> method >> path >> httpVersion;

    if (method.empty() || path.empty()) {
        closesocket(clientSock);
        return;
    }

    // Handle CORS preflight
    if (method == "OPTIONS") {
        sendHttpResponse(clientSock, 204, "No Content", "text/plain", "");
        closesocket(clientSock);
        return;
    }

    // Extract POST payload body
    std::string postBody = "";
    size_t headerEnd = rawRequest.find("\r\n\r\n");
    if (headerEnd != std::string::npos) {
        postBody = rawRequest.substr(headerEnd + 4);
    }

    // Normalize path query
    size_t queryPos = path.find('?');
    if (queryPos != std::string::npos) {
        path = path.substr(0, queryPos);
    }

    // ========================================================================
    // C++ REST API ROUTER
    // ========================================================================

    if (path == "/api/status") {
        sendHttpResponse(clientSock, 200, "OK", "application/json", CppDatabaseManager::instance().getStatusJson());
    }
    else if (path == "/api/students") {
        sendHttpResponse(clientSock, 200, "OK", "application/json", CppDatabaseManager::instance().getStudentsJson());
    }
    else if (path == "/api/exams" && method == "GET") {
        std::string adminId = "";
        size_t adminParam = rawRequest.find("admin_id=");
        if (adminParam != std::string::npos) {
            size_t start = adminParam + 9;
            size_t end = rawRequest.find_first_of(" &\r\n", start);
            adminId = rawRequest.substr(start, end - start);
        }
        sendHttpResponse(clientSock, 200, "OK", "application/json", CppDatabaseManager::instance().getExamsJson(adminId));
    }
    else if (path == "/api/exams" && method == "POST") {
        std::string id = extractJsonField(postBody, "id");
        std::string title = extractJsonField(postBody, "title");
        std::string cat = extractJsonField(postBody, "category");
        int duration = std::max(1, atoi(extractJsonField(postBody, "durationMinutes").c_str()));
        double passPct = atof(extractJsonField(postBody, "passingPercentage").c_str());
        int totalMarks = std::max(10, atoi(extractJsonField(postBody, "totalMarks").c_str()));
        std::string createdBy = extractJsonField(postBody, "created_by");
        if (createdBy.empty()) createdBy = extractJsonField(postBody, "admin_id");

        std::string errMsg = "";
        bool ok = CppDatabaseManager::instance().saveExam(id, title, cat, duration, passPct, totalMarks, createdBy, errMsg);
        if (ok) {
            sendHttpResponse(clientSock, 200, "OK", "application/json", "{\"success\":true,\"id\":\"" + escapeJson(id) + "\"}");
        } else {
            int code = (errMsg.find("Access Denied") != std::string::npos) ? 403 : 400;
            sendHttpResponse(clientSock, code, code == 403 ? "Forbidden" : "Bad Request", "application/json", "{\"success\":false,\"error\":\"" + escapeJson(errMsg) + "\"}");
        }
    }
    else if (path == "/api/exams/delete" && method == "POST") {
        std::string id = extractJsonField(postBody, "id");
        std::string adminId = extractJsonField(postBody, "admin_id");
        if (adminId.empty()) adminId = extractJsonField(postBody, "created_by");

        std::string errMsg = "";
        bool ok = CppDatabaseManager::instance().deleteExam(id, adminId, errMsg);
        if (ok) {
            sendHttpResponse(clientSock, 200, "OK", "application/json", "{\"success\":true,\"deletedId\":\"" + escapeJson(id) + "\"}");
        } else {
            int code = (errMsg.find("Access Denied") != std::string::npos) ? 403 : 400;
            sendHttpResponse(clientSock, code, code == 403 ? "Forbidden" : "Bad Request", "application/json", "{\"success\":false,\"error\":\"" + escapeJson(errMsg) + "\"}");
        }
    }
    else if (path == "/api/questions" && method == "GET") {
        sendHttpResponse(clientSock, 200, "OK", "application/json", CppDatabaseManager::instance().getQuestionsJson());
    }
    else if (path == "/api/verify-student" && method == "POST") {
        std::string ident = extractJsonField(postBody, "identifier");
        std::string email = extractJsonField(postBody, "email");
        std::string result = CppDatabaseManager::instance().verifyAccount(ident, email, "student");
        sendHttpResponse(clientSock, 200, "OK", "application/json", result);
    }
    else if (path == "/api/verify-admin" && method == "POST") {
        std::string ident = extractJsonField(postBody, "identifier");
        std::string email = extractJsonField(postBody, "email");
        std::string result = CppDatabaseManager::instance().verifyAccount(ident, email, "admin");
        sendHttpResponse(clientSock, 200, "OK", "application/json", result);
    }
    else if (path == "/api/reset-password" && method == "POST") {
        std::string ident = extractJsonField(postBody, "identifier");
        std::string newPwd = extractJsonField(postBody, "newPassword");
        bool ok = CppDatabaseManager::instance().resetPassword(ident, newPwd);
        sendHttpResponse(clientSock, 200, "OK", "application/json", ok ? "{\"success\":true,\"message\":\"Password updated in SQLite 3.\"}" : "{\"success\":false,\"error\":\"Update failed.\"}");
    }
    else if (path == "/api/register" && method == "POST") {
        std::string user = extractJsonField(postBody, "username");
        std::string pwd = extractJsonField(postBody, "password");
        std::string name = extractJsonField(postBody, "fullName");
        std::string mail = extractJsonField(postBody, "email");
        std::string role = extractJsonField(postBody, "role");
        std::string spec = extractJsonField(postBody, "specificId");
        if (role.empty()) role = "student";

        bool ok = CppDatabaseManager::instance().registerUser(user, pwd, name, mail, role, spec);
        sendHttpResponse(clientSock, 200, "OK", "application/json", ok ? "{\"success\":true}" : "{\"success\":false,\"error\":\"Username or Email already registered.\"}");
    }
    else if (path == "/api/open-database" && method == "POST") {
        // Safe desktop launch of SQLite Database
        ShellExecuteA(NULL, "open", "online_exam.db", NULL, NULL, SW_SHOWNORMAL);
        sendHttpResponse(clientSock, 200, "OK", "application/json", "{\"success\":true,\"opened_with\":\"Desktop SQLite Handler\",\"database_file\":\"online_exam.db\"}");
    }
    else if (path == "/api/reveal-database" && method == "POST") {
        std::string fullPath = fs::absolute("online_exam.db").string();
        std::string cmd = "/select,\"" + fullPath + "\"";
        ShellExecuteA(NULL, "open", "explorer.exe", cmd.c_str(), NULL, SW_SHOWNORMAL);
        sendHttpResponse(clientSock, 200, "OK", "application/json", "{\"success\":true}");
    }
    else {
        // ====================================================================
        // STATIC FILE HANDLER (HTML, CSS, JS, IMAGES)
        // ====================================================================
        if (path == "/" || path == "/index.html") {
            path = "/index.html";
        }

        std::string relativePath = path;
        if (!relativePath.empty() && relativePath[0] == '/') {
            relativePath = relativePath.substr(1);
        }

        fs::path filePath = fs::path(rootDir) / relativePath;
        if (!fs::exists(filePath) || fs::is_directory(filePath)) {
            // Check fallback in root if rootDir was a subfolder
            filePath = fs::path("index.html");
        }

        if (fs::exists(filePath) && !fs::is_directory(filePath)) {
            std::ifstream file(filePath, std::ios::binary);
            if (file.is_open()) {
                std::ostringstream ss;
                ss << file.rdbuf();
                std::string fileContent = ss.str();
                std::string ext = filePath.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                sendHttpResponse(clientSock, 200, "OK", getMimeType(ext), fileContent);
            } else {
                sendHttpResponse(clientSock, 403, "Forbidden", "text/plain", "File access forbidden.");
            }
        } else {
            sendHttpResponse(clientSock, 404, "Not Found", "text/plain", "Resource not found.");
        }
    }

    closesocket(clientSock);
}

// ============================================================================
// 6. MAIN SERVER ENTRY POINT
// ============================================================================

int main(int argc, char* argv[]) {
    int port = 5500;
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0) port = 5500;
    }

    std::cout << "============================================================" << std::endl;
    std::cout << "  ONLINE EXAMINATION SYSTEM - C++17 REST BACKEND SERVER     " << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "[C++ Engine] Architecture: C++17 Multi-Threaded HTTP Core" << std::endl;
    std::cout << "[C++ Engine] Cryptography: Windows Native WinCrypt SHA-256" << std::endl;
    std::cout << "[C++ Engine] Database:     Windows Native SQLite 3 Engine" << std::endl;

    // 1. Initialize SQLite Database
    if (!CppDatabaseManager::instance().open("online_exam.db")) {
        std::cerr << "[WARNING] SQLite open failed. Verify online_exam.db is accessible." << std::endl;
    } else {
        std::cout << "[C++ Database] Connected successfully to online_exam.db" << std::endl;
    }

    // 2. Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[C++ Network] WSAStartup failed." << std::endl;
        return 1;
    }

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "[C++ Network] Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Enable SO_REUSEADDR
    BOOL reuse = TRUE;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons((u_short)port);

    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[C++ Network] Bind error on port " << port << ". Trying port 8080..." << std::endl;
        port = 8080;
        serverAddr.sin_port = htons((u_short)port);
        if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "[C++ Network] Bind failed on port " << port << std::endl;
            closesocket(listenSock);
            WSACleanup();
            return 1;
        }
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[C++ Network] Listen failed." << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    std::string rootDir = fs::current_path().string();
    std::cout << "[C++ Server]  Server listening at: http://localhost:" << port << "/" << std::endl;
    std::cout << "[C++ Server]  Static root:         " << rootDir << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "Ready to process requests. Press Ctrl+C to terminate server." << std::endl << std::endl;

    // Launch browser automatically
    std::string url = "http://localhost:" + std::to_string(port) + "/index.html";
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);

    // 3. Connection Accept Loop
    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientLen);
        if (clientSock == INVALID_SOCKET) {
            break;
        }

        // Handle each client in a separate thread
        std::thread([clientSock, rootDir]() {
            handleClientConnection(clientSock, rootDir);
        }).detach();
    }

    closesocket(listenSock);
    WSACleanup();
    return 0;
}
