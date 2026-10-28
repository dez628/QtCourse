#include "calculatormainwindow.h"
#include "ui_calculator.h"

#include <QPushButton>

CalculatorMainWindow::CalculatorMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalculatorMainWindow)
{
    ui->setupUi(this);
    setupButtons();
    refreshDisplay();
}

CalculatorMainWindow::~CalculatorMainWindow()
{
    delete ui;
}

void CalculatorMainWindow::setupButtons()
{
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
        button->setFocusPolicy(Qt::NoFocus);
        connect(button, &QPushButton::clicked, this, [this, button]() {
            dispatchToken(button->property("calculatorToken").toString());
        });
    }
}

void CalculatorMainWindow::dispatchToken(const QString &token)
{
    m_core.inputToken(token);
    refreshDisplay();
}

void CalculatorMainWindow::refreshDisplay()
{
    ui->displayLabel->setText(m_core.displayText());
    ui->expressionLabel->setText(m_core.expressionText());
}
