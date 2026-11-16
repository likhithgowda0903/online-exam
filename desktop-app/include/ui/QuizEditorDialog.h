#ifndef QUIZ_EDITOR_DIALOG_H
#define QUIZ_EDITOR_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include "models/Quiz.h"

/**
 * @file QuizEditorDialog.h
 * @brief Modal dialog for creating and managing examinations and their question pools.
 */
class QuizEditorDialog : public QDialog {
    Q_OBJECT

private:
    Quiz m_quiz;
    bool m_isEditMode;

    QLineEdit* m_titleEdit;
    QComboBox* m_categoryCombo;
    QSpinBox* m_durationSpin;
    QDoubleSpinBox* m_passPercentageSpin;
    QTextEdit* m_descriptionEdit;
    QLabel* m_totalMarksLabel;

    QTableWidget* m_questionsTable;
    QPushButton* m_addQuestionBtn;
    QPushButton* m_editQuestionBtn;
    QPushButton* m_deleteQuestionBtn;

    void setupUI();
    void refreshQuestionsTable();

public:
    explicit QuizEditorDialog(QWidget* parent = nullptr);
    explicit QuizEditorDialog(const Quiz& existingQuiz, QWidget* parent = nullptr);
    ~QuizEditorDialog() override = default;

    Quiz getQuiz() const { return m_quiz; }

private slots:
    void handleAddQuestion();
    void handleEditQuestion();
    void handleDeleteQuestion();
    void handleSaveQuiz();
};

#endif // QUIZ_EDITOR_DIALOG_H
