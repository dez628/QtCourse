#ifndef CALCULATORMAINWINDOW_H
#define CALCULATORMAINWINDOW_H

#include "calculatorcore.h"

#include <QMainWindow>

namespace Ui {
class CalculatorMainWindow;
}

class CalculatorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalculatorMainWindow(QWidget *parent = nullptr);
    ~CalculatorMainWindow() override;

private:
    void setupButtons();
    void dispatchToken(const QString &token);
    void refreshDisplay();

    Ui::CalculatorMainWindow *ui;
    CalculatorCore m_core;
};

#endif // CALCULATORMAINWINDOW_H
