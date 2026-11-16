#ifndef QUESTION_EDITOR_DIALOG_H
#define QUESTION_EDITOR_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include "models/Question.h"

/**
 * @file QuestionEditorDialog.h
 * @brief Modal dialog for creating and editing individual multiple-choice questions.
 */
class QuestionEditorDialog : public QDialog {
    Q_OBJECT

private:
    Question m_question;
    bool m_isEditMode;

    QTextEdit* m_questionTextEdit;
    QLineEdit* m_optAEdit;
    QLineEdit* m_optBEdit;
    QLineEdit* m_optCEdit;
    QLineEdit* m_optDEdit;
    QComboBox* m_correctCombo;
    QSpinBox* m_marksSpin;
    QTextEdit* m_explanationEdit;

    void setupUI();

public:
    explicit QuestionEditorDialog(int quizId, QWidget* parent = nullptr);
    explicit QuestionEditorDialog(const Question& existingQuestion, QWidget* parent = nullptr);
    ~QuestionEditorDialog() override = default;

    Question getQuestion() const { return m_question; }

private slots:
    void handleSave();
};

#endif // QUESTION_EDITOR_DIALOG_H
