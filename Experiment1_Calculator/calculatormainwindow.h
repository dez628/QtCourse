#ifndef CALCULATORMAINWINDOW_H
#define CALCULATORMAINWINDOW_H

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
    Ui::CalculatorMainWindow *ui;
};

#endif // CALCULATORMAINWINDOW_H
