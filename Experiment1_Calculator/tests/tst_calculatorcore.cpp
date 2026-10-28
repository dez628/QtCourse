#include "calculatorcore.h"

#include <QtTest>

class CalculatorCoreTest : public QObject
{
    Q_OBJECT

private:
    static void enter(CalculatorCore &calculator, const QString &sequence)
    {
        for (const QChar token : sequence) {
            calculator.inputToken(QString(token));
        }
    }

private slots:
    void initialState();
    void decimalInput_data();
    void decimalInput();
    void fourOperations_data();
    void fourOperations();
    void consecutiveOperators();
    void chainedOperations();
    void resultContinuesWithOperator();
    void digitStartsNewCalculationAfterResult();
    void repeatedEquals();
    void divideByZeroAndRecovery();
    void errorStateIgnoresOperators();
    void backspaceAndClear();
    void negativeLeadingZero();
    void signAfterOperator();
};

void CalculatorCoreTest::initialState()
{
    CalculatorCore calculator;
    QCOMPARE(calculator.displayText(), QStringLiteral("0"));
    QCOMPARE(calculator.expressionText(), QStringLiteral("准备计算"));
    QVERIFY(!calculator.hasError());
}

void CalculatorCoreTest::decimalInput_data()
{
    QTest::addColumn<QString>("sequence");
    QTest::addColumn<QString>("display");

    QTest::newRow("leading decimal") << QStringLiteral(".5") << QStringLiteral("0.5");
    QTest::newRow("duplicate decimal") << QStringLiteral("1.2.3")
                                      << QStringLiteral("1.23");
}

void CalculatorCoreTest::decimalInput()
{
    QFETCH(QString, sequence);
    QFETCH(QString, display);

    CalculatorCore calculator;
    enter(calculator, sequence);
    QCOMPARE(calculator.displayText(), display);
}

void CalculatorCoreTest::fourOperations_data()
{
    QTest::addColumn<QString>("sequence");
    QTest::addColumn<double>("expected");

    QTest::newRow("addition") << QStringLiteral("12+8=") << 20.0;
    QTest::newRow("subtraction") << QStringLiteral("12−8=") << 4.0;
    QTest::newRow("multiplication") << QStringLiteral("12×8=") << 96.0;
    QTest::newRow("division") << QStringLiteral("12÷8=") << 1.5;
    QTest::newRow("decimal addition") << QStringLiteral("0.1+0.2=") << 0.3;
}

void CalculatorCoreTest::fourOperations()
{
    QFETCH(QString, sequence);
    QFETCH(double, expected);

    CalculatorCore calculator;
    enter(calculator, sequence);
    const double actual = calculator.displayText().toDouble();
    QVERIFY2(qAbs(actual - expected) < 1e-12,
             qPrintable(calculator.displayText()));
}

void CalculatorCoreTest::consecutiveOperators()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("8÷×2="));
    QCOMPARE(calculator.displayText(), QStringLiteral("16"));
    QCOMPARE(calculator.expressionText(), QStringLiteral("8 × 2 ="));
}

void CalculatorCoreTest::chainedOperations()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("2+3×4="));
    QCOMPARE(calculator.displayText(), QStringLiteral("20"));
}

void CalculatorCoreTest::resultContinuesWithOperator()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("2+3=×2="));
    QCOMPARE(calculator.displayText(), QStringLiteral("10"));
}

void CalculatorCoreTest::digitStartsNewCalculationAfterResult()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("2+3=7"));
    QCOMPARE(calculator.displayText(), QStringLiteral("7"));
    QCOMPARE(calculator.expressionText(), QStringLiteral("7"));
}

void CalculatorCoreTest::repeatedEquals()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("2+3=="));
    QCOMPARE(calculator.displayText(), QStringLiteral("8"));
}

void CalculatorCoreTest::divideByZeroAndRecovery()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("8÷0="));
    QVERIFY(calculator.hasError());
    QCOMPARE(calculator.displayText(), QStringLiteral("无法除以 0"));
    QCOMPARE(calculator.expressionText(), QStringLiteral("8 ÷ 0 ="));

    enter(calculator, QStringLiteral("5+2="));
    QVERIFY(!calculator.hasError());
    QCOMPARE(calculator.displayText(), QStringLiteral("7"));
}

void CalculatorCoreTest::errorStateIgnoresOperators()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("8÷0=+"));
    QVERIFY(calculator.hasError());
    QCOMPARE(calculator.displayText(), QStringLiteral("无法除以 0"));
}

void CalculatorCoreTest::backspaceAndClear()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("12.3⌫⌫⌫"));
    QCOMPARE(calculator.displayText(), QStringLiteral("1"));
    enter(calculator, QStringLiteral("⌫"));
    QCOMPARE(calculator.displayText(), QStringLiteral("0"));

    enter(calculator, QStringLiteral("9+1=C"));
    QCOMPARE(calculator.displayText(), QStringLiteral("0"));
    QCOMPARE(calculator.expressionText(), QStringLiteral("准备计算"));
}

void CalculatorCoreTest::negativeLeadingZero()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("5+±3"));
    QCOMPARE(calculator.displayText(), QStringLiteral("-3"));
}

void CalculatorCoreTest::signAfterOperator()
{
    CalculatorCore calculator;
    enter(calculator, QStringLiteral("5+±3="));
    QCOMPARE(calculator.displayText(), QStringLiteral("2"));
}

QTEST_APPLESS_MAIN(CalculatorCoreTest)

#include "tst_calculatorcore.moc"
