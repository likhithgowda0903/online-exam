#include "ui/QuizEditorDialog.h"
#include "ui/QuestionEditorDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>

QuizEditorDialog::QuizEditorDialog(QWidget* parent)
    : QDialog(parent), m_isEditMode(false) {
    setWindowTitle("Create Examination");
    setMinimumSize(780, 640);
    resize(820, 680);
    setupUI();
    refreshQuestionsTable();
}

QuizEditorDialog::QuizEditorDialog(const Quiz& existingQuiz, QWidget* parent)
    : QDialog(parent), m_quiz(existingQuiz), m_isEditMode(true) {
    setWindowTitle("Edit Examination: " + existingQuiz.getTitle());
    setMinimumSize(780, 640);
    resize(820, 680);
    setupUI();
    refreshQuestionsTable();
}

void QuizEditorDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(16);

    auto headerLabel = new QLabel(m_isEditMode ? "✏ Edit Examination Package" : "➕ Create New Examination", this);
    headerLabel->setObjectName("titleLabel");
    mainLayout->addWidget(headerLabel);

    // Form inputs
    auto formLayout = new QFormLayout();
    formLayout->setSpacing(12);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("e.g. Advanced C++ & Software Design");
    m_titleEdit->setText(m_quiz.getTitle());
    formLayout->addRow("Exam Title *:", m_titleEdit);

    auto hRow = new QHBoxLayout();
    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->addItems({"Computer Science", "Information Technology", "Software Engineering", "Mathematics", "General Aptitude"});
    if (!m_quiz.getCategory().isEmpty()) {
        m_categoryCombo->setCurrentText(m_quiz.getCategory());
    }
    hRow->addWidget(m_categoryCombo, 2);

    m_durationSpin = new QSpinBox(this);
    m_durationSpin->setRange(1, 300);
    m_durationSpin->setSuffix(" mins");
    m_durationSpin->setValue(m_quiz.getDurationMinutes() > 0 ? m_quiz.getDurationMinutes() : 15);
    hRow->addWidget(new QLabel("Duration:"));
    hRow->addWidget(m_durationSpin, 1);

    m_passPercentageSpin = new QDoubleSpinBox(this);
    m_passPercentageSpin->setRange(0.0, 100.0);
    m_passPercentageSpin->setSuffix(" %");
    m_passPercentageSpin->setValue(m_quiz.getPassPercentage() > 0 ? m_quiz.getPassPercentage() : 50.0);
    hRow->addWidget(new QLabel("Pass Mark:"));
    hRow->addWidget(m_passPercentageSpin, 1);

    formLayout->addRow("Category & Config *:", hRow);

    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setPlaceholderText("Brief description and instructions for candidates...");
    m_descriptionEdit->setMaximumHeight(65);
    m_descriptionEdit->setPlainText(m_quiz.getDescription());
    formLayout->addRow("Description:", m_descriptionEdit);

    mainLayout->addLayout(formLayout);

    // Question Management Section
    auto qHeaderRow = new QHBoxLayout();
    auto qSecLabel = new QLabel("Questions in this Examination", this);
    qSecLabel->setStyleSheet("font-size: 15px; font-weight: bold; color: #f8fafc;");
    qHeaderRow->addWidget(qSecLabel);

    qHeaderRow->addStretch();

    m_totalMarksLabel = new QLabel("Total Marks: 0", this);
    m_totalMarksLabel->setStyleSheet("font-weight: bold; color: #38bdf8; font-size: 14px;");
    qHeaderRow->addWidget(m_totalMarksLabel);

    mainLayout->addLayout(qHeaderRow);

    // Questions Table
    m_questionsTable = new QTableWidget(this);
    m_questionsTable->setColumnCount(4);
    m_questionsTable->setHorizontalHeaderLabels({"Q#", "Question Text", "Correct Option", "Marks"});
    m_questionsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_questionsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_questionsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_questionsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_questionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_questionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_questionsTable->setAlternatingRowColors(true);
    mainLayout->addWidget(m_questionsTable);

    // Question Action Buttons
    auto qBtnRow = new QHBoxLayout();
    m_addQuestionBtn = new QPushButton("➕ Add Question", this);
    m_addQuestionBtn->setObjectName("secondaryButton");
    connect(m_addQuestionBtn, &QPushButton::clicked, this, &QuizEditorDialog::handleAddQuestion);

    m_editQuestionBtn = new QPushButton("✏ Edit Selected", this);
    m_editQuestionBtn->setObjectName("secondaryButton");
    connect(m_editQuestionBtn, &QPushButton::clicked, this, &QuizEditorDialog::handleEditQuestion);

    m_deleteQuestionBtn = new QPushButton("🗑 Remove Selected", this);
    m_deleteQuestionBtn->setObjectName("dangerButton");
    connect(m_deleteQuestionBtn, &QPushButton::clicked, this, &QuizEditorDialog::handleDeleteQuestion);

    qBtnRow->addWidget(m_addQuestionBtn);
    qBtnRow->addWidget(m_editQuestionBtn);
    qBtnRow->addWidget(m_deleteQuestionBtn);
    qBtnRow->addStretch();
    mainLayout->addLayout(qBtnRow);

    // Bottom Dialog Buttons
    auto bottomRow = new QHBoxLayout();
    bottomRow->addStretch();

    auto cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setObjectName("secondaryButton");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    auto saveQuizBtn = new QPushButton(m_isEditMode ? "💾 Save Examination Changes" : "💾 Create Examination", this);
    saveQuizBtn->setObjectName("successButton");
    saveQuizBtn->setMinimumHeight(40);
    connect(saveQuizBtn, &QPushButton::clicked, this, &QuizEditorDialog::handleSaveQuiz);

    bottomRow->addWidget(cancelBtn);
    bottomRow->addWidget(saveQuizBtn);
    mainLayout->addLayout(bottomRow);
}

void QuizEditorDialog::refreshQuestionsTable() {
    const auto& questions = m_quiz.getQuestions();
    m_questionsTable->setRowCount(static_cast<int>(questions.size()));

    int totalMarks = 0;
    for (int i = 0; i < static_cast<int>(questions.size()); ++i) {
        const auto& q = questions[i];
        totalMarks += q.getMarks();

        m_questionsTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_questionsTable->setItem(i, 1, new QTableWidgetItem(q.getQuestionText()));
        m_questionsTable->setItem(i, 2, new QTableWidgetItem(QString("Option %1").arg(QChar('A' + q.getCorrectOptionIndex()))));
        m_questionsTable->setItem(i, 3, new QTableWidgetItem(QString::number(q.getMarks())));
    }

    m_totalMarksLabel->setText(QString("Total Questions: %1  |  Total Marks: %2").arg(questions.size()).arg(totalMarks));
}

void QuizEditorDialog::handleAddQuestion() {
    QuestionEditorDialog dlg(m_quiz.getId(), this);
    if (dlg.exec() == QDialog::Accepted) {
        m_quiz.addQuestion(dlg.getQuestion());
        refreshQuestionsTable();
    }
}

void QuizEditorDialog::handleEditQuestion() {
    int row = m_questionsTable->currentRow();
    if (row < 0 || row >= m_quiz.getQuestionCount()) {
        QMessageBox::warning(this, "Select Question", "Please select a question from the table to edit.");
        return;
    }

    const Question* targetQ = m_quiz.getQuestionAt(row);
    if (!targetQ) return;

    QuestionEditorDialog dlg(*targetQ, this);
    if (dlg.exec() == QDialog::Accepted) {
        auto questions = m_quiz.getQuestions();
        questions[row] = dlg.getQuestion();
        m_quiz.setQuestions(questions);
        refreshQuestionsTable();
    }
}

void QuizEditorDialog::handleDeleteQuestion() {
    int row = m_questionsTable->currentRow();
    if (row < 0 || row >= m_quiz.getQuestionCount()) {
        QMessageBox::warning(this, "Select Question", "Please select a question to remove.");
        return;
    }

    auto reply = QMessageBox::question(this, "Confirm Removal", "Are you sure you want to remove this question from the examination?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        auto questions = m_quiz.getQuestions();
        questions.erase(questions.begin() + row);
        m_quiz.setQuestions(questions);
        refreshQuestionsTable();
    }
}

void QuizEditorDialog::handleSaveQuiz() {
    QString title = m_titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Missing Title", "Please enter an examination title.");
        return;
    }

    if (m_quiz.getQuestionCount() == 0) {
        auto reply = QMessageBox::question(this, "No Questions Added", "You have not added any questions to this examination. Save anyway?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    m_quiz.setTitle(title);
    m_quiz.setCategory(m_categoryCombo->currentText());
    m_quiz.setDurationMinutes(m_durationSpin->value());
    m_quiz.setPassPercentage(m_passPercentageSpin->value());
    m_quiz.setDescription(m_descriptionEdit->toPlainText().trimmed());
    m_quiz.recalculateTotalMarks();

    accept();
}
