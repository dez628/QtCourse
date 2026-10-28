#ifndef CALCULATORCORE_H
#define CALCULATORCORE_H

#include <QString>

class CalculatorCore
{
public:
    CalculatorCore();

    void inputToken(const QString &token);
    void clear();

    QString displayText() const;
    QString expressionText() const;
    bool hasError() const;

private:
    void inputDigit(const QString &digit);
    void inputDecimalPoint();
    void backspace();
    void toggleSign();
    void inputOperator(const QString &op);
    void calculate();

    void resetCalculation();
    bool executePendingOperation();
    void updateExpression();
    double currentValue() const;
    static QString formatNumber(double value);

    QString m_currentText;
    QString m_expressionText;
    QString m_leftDisplayText;
    QString m_pendingOperator;
    double m_accumulator;
    double m_lastOperand;
    QString m_lastOperator;
    bool m_waitingForOperand;
    bool m_freshInput;
    bool m_resultDisplayed;
    bool m_hasError;
};

#endif // CALCULATORCORE_H
