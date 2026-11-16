#include "ui/QuestionEditorDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>

QuestionEditorDialog::QuestionEditorDialog(int quizId, QWidget* parent)
    : QDialog(parent), m_isEditMode(false) {
    m_question.setQuizId(quizId);
    m_question.setMarks(5);
    setWindowTitle("Add New Question");
    setMinimumSize(560, 580);
    setupUI();
}

QuestionEditorDialog::QuestionEditorDialog(const Question& existingQuestion, QWidget* parent)
    : QDialog(parent), m_question(existingQuestion), m_isEditMode(true) {
    setWindowTitle("Edit Question");
    setMinimumSize(560, 580);
    setupUI();
}

void QuestionEditorDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(14);

    auto headerLabel = new QLabel(m_isEditMode ? "✏ Edit Question" : "➕ Add Multiple Choice Question", this);
    headerLabel->setObjectName("titleLabel");
    mainLayout->addWidget(headerLabel);

    auto formLayout = new QFormLayout();
    formLayout->setSpacing(12);

    m_questionTextEdit = new QTextEdit(this);
    m_questionTextEdit->setPlaceholderText("Enter the question prompt here...");
    m_questionTextEdit->setMaximumHeight(85);
    m_questionTextEdit->setPlainText(m_question.getQuestionText());
    formLayout->addRow("Question Prompt *:", m_questionTextEdit);

    m_optAEdit = new QLineEdit(this);
    m_optAEdit->setPlaceholderText("Option A text");
    m_optAEdit->setText(m_question.getOption(0));
    formLayout->addRow("Option A *:", m_optAEdit);

    m_optBEdit = new QLineEdit(this);
    m_optBEdit->setPlaceholderText("Option B text");
    m_optBEdit->setText(m_question.getOption(1));
    formLayout->addRow("Option B *:", m_optBEdit);

    m_optCEdit = new QLineEdit(this);
    m_optCEdit->setPlaceholderText("Option C text");
    m_optCEdit->setText(m_question.getOption(2));
    formLayout->addRow("Option C *:", m_optCEdit);

    m_optDEdit = new QLineEdit(this);
    m_optDEdit->setPlaceholderText("Option D text");
    m_optDEdit->setText(m_question.getOption(3));
    formLayout->addRow("Option D *:", m_optDEdit);

    m_correctCombo = new QComboBox(this);
    m_correctCombo->addItems({"Option A", "Option B", "Option C", "Option D"});
    m_correctCombo->setCurrentIndex(m_question.getCorrectOptionIndex());
    formLayout->addRow("Correct Option *:", m_correctCombo);

    m_marksSpin = new QSpinBox(this);
    m_marksSpin->setRange(1, 100);
    m_marksSpin->setValue(m_question.getMarks() > 0 ? m_question.getMarks() : 5);
    formLayout->addRow("Marks Awarded *:", m_marksSpin);

    m_explanationEdit = new QTextEdit(this);
    m_explanationEdit->setPlaceholderText("Provide an explanation or rationale for the correct answer...");
    m_explanationEdit->setMaximumHeight(70);
    m_explanationEdit->setPlainText(m_question.getExplanation());
    formLayout->addRow("Explanation:", m_explanationEdit);

    mainLayout->addLayout(formLayout);

    // Buttons
    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    auto cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setObjectName("secondaryButton");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    auto saveBtn = new QPushButton(m_isEditMode ? "Save Changes" : "Add Question", this);
    saveBtn->setObjectName("successButton");
    connect(saveBtn, &QPushButton::clicked, this, &QuestionEditorDialog::handleSave);

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);
}

void QuestionEditorDialog::handleSave() {
    QString qText = m_questionTextEdit->toPlainText().trimmed();
    QString optA = m_optAEdit->text().trimmed();
    QString optB = m_optBEdit->text().trimmed();
    QString optC = m_optCEdit->text().trimmed();
    QString optD = m_optDEdit->text().trimmed();

    if (qText.isEmpty() || optA.isEmpty() || optB.isEmpty() || optC.isEmpty() || optD.isEmpty()) {
        QMessageBox::warning(this, "Missing Fields", "Please fill in the question text and all four options.");
        return;
    }

    m_question.setQuestionText(qText);
    m_question.setOptions({optA, optB, optC, optD});
    m_question.setCorrectOptionIndex(m_correctCombo->currentIndex());
    m_question.setMarks(m_marksSpin->value());
    m_question.setExplanation(m_explanationEdit->toPlainText().trimmed());

    accept();
}
