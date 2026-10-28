#include "calculatorcore.h"

#include <QtGlobal>

#include <cmath>

namespace {
constexpr int MaximumInputDigits = 16;

bool isOperator(const QString &token)
{
    return token == QStringLiteral("+")
        || token == QStringLiteral("−")
        || token == QStringLiteral("×")
        || token == QStringLiteral("÷");
}
}

CalculatorCore::CalculatorCore()
{
    clear();
}

void CalculatorCore::inputToken(const QString &token)
{
    if (token.size() == 1 && token.front().isDigit()) {
        inputDigit(token);
        return;
    }

    if (token == QStringLiteral(".")) {
        inputDecimalPoint();
    } else if (token == QStringLiteral("C")) {
        clear();
    } else if (token == QStringLiteral("⌫")) {
        backspace();
    } else if (token == QStringLiteral("±")) {
        toggleSign();
    } else if (isOperator(token)) {
        inputOperator(token);
    } else if (token == QStringLiteral("=")) {
        calculate();
    }
}

void CalculatorCore::clear()
{
    m_currentText = QStringLiteral("0");
    m_expressionText = QStringLiteral("准备计算");
    m_leftDisplayText.clear();
    m_pendingOperator.clear();
    m_lastOperator.clear();
    m_accumulator = 0.0;
    m_lastOperand = 0.0;
    m_waitingForOperand = false;
    m_freshInput = true;
    m_resultDisplayed = false;
    m_hasError = false;
}

QString CalculatorCore::displayText() const
{
    return m_currentText;
}

QString CalculatorCore::expressionText() const
{
    return m_expressionText;
}

bool CalculatorCore::hasError() const
{
    return m_hasError;
}

void CalculatorCore::inputDigit(const QString &digit)
{
    if (m_hasError || (m_resultDisplayed && !m_waitingForOperand)) {
        resetCalculation();
    }

    if (m_waitingForOperand) {
        m_currentText = digit;
        m_waitingForOperand = false;
    } else if (m_freshInput || m_currentText == QStringLiteral("0")) {
        m_currentText = digit;
    } else if (m_currentText == QStringLiteral("-0")) {
        m_currentText = QStringLiteral("-") + digit;
    } else {
        int digitCount = 0;
        for (const QChar character : m_currentText) {
            if (character.isDigit()) {
                ++digitCount;
            }
        }
        if (digitCount >= MaximumInputDigits) {
            return;
        }
        m_currentText += digit;
    }

    m_freshInput = false;
    m_resultDisplayed = false;
    updateExpression();
}

void CalculatorCore::inputDecimalPoint()
{
    if (m_hasError || (m_resultDisplayed && !m_waitingForOperand)) {
        resetCalculation();
    }

    if (m_waitingForOperand) {
        m_currentText = QStringLiteral("0.");
        m_waitingForOperand = false;
    } else if (m_freshInput) {
        m_currentText = QStringLiteral("0.");
    } else if (!m_currentText.contains(QLatin1Char('.'))) {
        m_currentText += QLatin1Char('.');
    }

    m_freshInput = false;
    m_resultDisplayed = false;
    updateExpression();
}

void CalculatorCore::backspace()
{
    if (m_hasError) {
        clear();
        return;
    }

    if (m_resultDisplayed || m_waitingForOperand) {
        return;
    }

    m_currentText.chop(1);
    if (m_currentText.isEmpty() || m_currentText == QStringLiteral("-")) {
        m_currentText = QStringLiteral("0");
        m_freshInput = true;
    }
    updateExpression();
}

void CalculatorCore::toggleSign()
{
    if (m_hasError) {
        return;
    }

    if (m_waitingForOperand) {
        m_currentText = QStringLiteral("-0");
        m_waitingForOperand = false;
        m_freshInput = false;
        m_resultDisplayed = false;
        updateExpression();
        return;
    }

    if (m_freshInput) {
        m_currentText = QStringLiteral("-0");
    } else if (m_currentText.startsWith(QLatin1Char('-'))) {
        m_currentText.remove(0, 1);
    } else {
        m_currentText.prepend(QLatin1Char('-'));
    }
    m_resultDisplayed = false;
    updateExpression();
}

void CalculatorCore::inputOperator(const QString &op)
{
    if (m_hasError) {
        return;
    }

    if (m_pendingOperator.isEmpty()) {
        m_accumulator = currentValue();
    } else if (m_waitingForOperand) {
        m_pendingOperator = op;
        m_leftDisplayText = m_currentText;
        updateExpression();
        return;
    } else if (!executePendingOperation()) {
        return;
    }

    m_pendingOperator = op;
    m_leftDisplayText = m_currentText;
    m_waitingForOperand = true;
    m_freshInput = true;
    m_resultDisplayed = false;
    updateExpression();
}

void CalculatorCore::calculate()
{
    if (m_hasError) {
        return;
    }

    if (m_pendingOperator.isEmpty()) {
        if (m_resultDisplayed && !m_lastOperator.isEmpty()) {
            const QString leftText = m_currentText;
            m_accumulator = currentValue();
            m_pendingOperator = m_lastOperator;
            m_currentText = formatNumber(m_lastOperand);
            m_waitingForOperand = false;
            if (!executePendingOperation()) {
                return;
            }
            m_expressionText = leftText + QStringLiteral(" ")
                + m_lastOperator + QStringLiteral(" ") + m_currentText
                + QStringLiteral(" =");
        } else if (!m_waitingForOperand) {
            m_expressionText = m_currentText + QStringLiteral(" =");
            m_resultDisplayed = true;
            m_freshInput = true;
        }
        return;
    }

    if (m_waitingForOperand) {
        m_currentText = m_leftDisplayText;
        m_waitingForOperand = false;
    }

    const QString leftText = m_leftDisplayText;
    const QString operatorText = m_pendingOperator;
    const QString rightText = m_currentText;
    m_lastOperator = m_pendingOperator;
    m_lastOperand = currentValue();

    if (!executePendingOperation()) {
        m_expressionText = leftText + QStringLiteral(" ") + operatorText
            + QStringLiteral(" ") + rightText + QStringLiteral(" =");
        return;
    }

    m_expressionText = leftText + QStringLiteral(" ") + operatorText
        + QStringLiteral(" ") + rightText + QStringLiteral(" =");
    m_pendingOperator.clear();
    m_resultDisplayed = true;
    m_freshInput = true;
    m_waitingForOperand = false;
}

void CalculatorCore::resetCalculation()
{
    m_expressionText = QStringLiteral("准备计算");
    m_leftDisplayText.clear();
    m_pendingOperator.clear();
    m_lastOperator.clear();
    m_accumulator = 0.0;
    m_lastOperand = 0.0;
    m_waitingForOperand = false;
    m_freshInput = true;
    m_resultDisplayed = false;
    m_hasError = false;
}

bool CalculatorCore::executePendingOperation()
{
    const double rightOperand = currentValue();
    double result = m_accumulator;

    if (m_pendingOperator == QStringLiteral("+")) {
        result += rightOperand;
    } else if (m_pendingOperator == QStringLiteral("−")) {
        result -= rightOperand;
    } else if (m_pendingOperator == QStringLiteral("×")) {
        result *= rightOperand;
    } else if (m_pendingOperator == QStringLiteral("÷")) {
        if (qFuzzyIsNull(rightOperand)) {
            m_currentText = QStringLiteral("无法除以 0");
            m_hasError = true;
            m_pendingOperator.clear();
            m_leftDisplayText.clear();
            m_freshInput = true;
            m_resultDisplayed = false;
            m_waitingForOperand = false;
            return false;
        }
        result /= rightOperand;
    }

    if (!std::isfinite(result)) {
        m_currentText = QStringLiteral("计算结果超出范围");
        m_hasError = true;
        m_pendingOperator.clear();
        m_leftDisplayText.clear();
        m_freshInput = true;
        m_resultDisplayed = false;
        m_waitingForOperand = false;
        return false;
    }

    m_accumulator = result;
    m_currentText = formatNumber(result);
    return true;
}

void CalculatorCore::updateExpression()
{
    if (!m_pendingOperator.isEmpty()) {
        m_expressionText = m_leftDisplayText + QStringLiteral(" ")
            + m_pendingOperator;
        if (!m_waitingForOperand) {
            m_expressionText += QStringLiteral(" ") + m_currentText;
        }
    } else if (!m_resultDisplayed) {
        m_expressionText = m_currentText;
    }
}

double CalculatorCore::currentValue() const
{
    bool ok = false;
    const double value = m_currentText.toDouble(&ok);
    return ok ? value : 0.0;
}

QString CalculatorCore::formatNumber(double value)
{
    if (qFuzzyIsNull(value)) {
        return QStringLiteral("0");
    }

    QString text = QString::number(value, 'g', 15);
    if (text == QStringLiteral("-0")) {
        return QStringLiteral("0");
    }
    return text;
}
