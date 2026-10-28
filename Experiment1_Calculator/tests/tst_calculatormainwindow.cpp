#include "calculatormainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QtTest>

class CalculatorMainWindowTest : public QObject
{
    Q_OBJECT

private slots:
    void keyboardAndMouseShareBehavior();
    void backspaceAndEscape();

private:
    static QString displayText(CalculatorMainWindow &window)
    {
        return window.findChild<QLabel *>(QStringLiteral("displayLabel"))->text();
    }

    static QString expressionText(CalculatorMainWindow &window)
    {
        return window.findChild<QLabel *>(QStringLiteral("expressionLabel"))->text();
    }

    static void click(CalculatorMainWindow &window, const QString &objectName)
    {
        QPushButton *button = window.findChild<QPushButton *>(objectName);
        QVERIFY2(button != nullptr, qPrintable(objectName));
        QTest::mouseClick(button, Qt::LeftButton);
    }
};

void CalculatorMainWindowTest::keyboardAndMouseShareBehavior()
{
    CalculatorMainWindow window;
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));

    QTest::keyClicks(&window, QStringLiteral("12.5+7"));
    QCOMPARE(displayText(window), QStringLiteral("7"));
    QCOMPARE(expressionText(window), QStringLiteral("12.5 + 7"));

    QTest::keyClick(&window, Qt::Key_Return);
    QCOMPARE(displayText(window), QStringLiteral("19.5"));

    click(window, QStringLiteral("buttonClear"));
    click(window, QStringLiteral("buttonOne"));
    click(window, QStringLiteral("buttonDecimal"));
    click(window, QStringLiteral("buttonFive"));
    click(window, QStringLiteral("buttonAdd"));
    click(window, QStringLiteral("buttonSeven"));
    click(window, QStringLiteral("buttonEquals"));
    QCOMPARE(displayText(window), QStringLiteral("8.5"));
}

void CalculatorMainWindowTest::backspaceAndEscape()
{
    CalculatorMainWindow window;
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));

    QTest::keyClicks(&window, QStringLiteral("123"));
    QTest::keyClick(&window, Qt::Key_Backspace);
    QCOMPARE(displayText(window), QStringLiteral("12"));

    QTest::keyClick(&window, Qt::Key_Escape);
    QCOMPARE(displayText(window), QStringLiteral("0"));
    QCOMPARE(expressionText(window), QStringLiteral("准备计算"));
}

QTEST_MAIN(CalculatorMainWindowTest)

#include "tst_calculatormainwindow.moc"
