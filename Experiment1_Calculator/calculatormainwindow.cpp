#include "calculatormainwindow.h"
#include "ui_calculator.h"

CalculatorMainWindow::CalculatorMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalculatorMainWindow)
{
    ui->setupUi(this);
}

CalculatorMainWindow::~CalculatorMainWindow()
{
    delete ui;
}
