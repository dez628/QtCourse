#include "calculatormainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    CalculatorMainWindow window;
    window.show();
    return application.exec();
}
