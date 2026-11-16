#ifndef RESULT_DIALOG_H
#define RESULT_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include "models/Result.h"
#include "models/ExamSession.h"

/**
 * @file ResultDialog.h
 * @brief Detailed Examination Scorecard and Result Breakdown Dialog.
 */
class ResultDialog : public QDialog {
    Q_OBJECT

private:
    Result m_result;
    bool m_hasSessionReview;
    ExamSessionPtr m_session;

    void setupUI();

public:
    explicit ResultDialog(const Result& result, ExamSessionPtr session = nullptr, QWidget* parent = nullptr);
    ~ResultDialog() override = default;
};

#endif // RESULT_DIALOG_H
