#include "calculatormainwindow.h"
#include "ui_calculator.h"

#include <QKeyEvent>
#include <QPushButton>

CalculatorMainWindow::CalculatorMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalculatorMainWindow)
{
    ui->setupUi(this);
    setupButtons();
    refreshDisplay();
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
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

void CalculatorMainWindow::keyPressEvent(QKeyEvent *event)
{
    QString token;
    const int key = event->key();

    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        token = QString::number(key - Qt::Key_0);
    } else {
        switch (key) {
        case Qt::Key_Plus:
            token = QStringLiteral("+");
            break;
        case Qt::Key_Minus:
            token = QStringLiteral("−");
            break;
        case Qt::Key_Asterisk:
            token = QStringLiteral("×");
            break;
        case Qt::Key_Slash:
            token = QStringLiteral("÷");
            break;
        case Qt::Key_Period:
        case Qt::Key_Comma:
            token = QStringLiteral(".");
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Equal:
            token = QStringLiteral("=");
            break;
        case Qt::Key_Backspace:
            token = QStringLiteral("⌫");
            break;
        case Qt::Key_Delete:
        case Qt::Key_Escape:
        case Qt::Key_C:
            token = QStringLiteral("C");
            break;
        case Qt::Key_F9:
            token = QStringLiteral("±");
            break;
        default:
            break;
        }
    }

    if (token.isEmpty()) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    dispatchToken(token);
    event->accept();
}
