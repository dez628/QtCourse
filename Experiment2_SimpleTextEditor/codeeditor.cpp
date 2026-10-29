#include "codeeditor.h"

#include <QPainter>
#include <QResizeEvent>
#include <QTextBlock>
#include <QWidget>

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor)
        : QWidget(editor)
        , codeEditor(editor)
    {
    }

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , lineNumberArea(new LineNumberArea(this))
    , lineNumbersVisible(true)
{
    connect(this,
            &CodeEditor::blockCountChanged,
            this,
            &CodeEditor::updateLineNumberAreaWidth);
    connect(this,
            &CodeEditor::updateRequest,
            this,
            &CodeEditor::updateLineNumberArea);

    lineNumberArea->setCursor(Qt::ArrowCursor);
    updateLineNumberAreaWidth(0);
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
    if (lineNumbersVisible == visible) {
        return;
    }

    lineNumbersVisible = visible;
    lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth(blockCount());
    updateLineNumberAreaGeometry();
}

void CodeEditor::refreshLineNumberArea()
{
    updateLineNumberAreaWidth(blockCount());
    updateLineNumberAreaGeometry();
    lineNumberArea->update();
}

int CodeEditor::lineNumberAreaWidth() const
{
    if (!lineNumbersVisible) {
        return 0;
    }

    int digits = 2;
    int maxLines = qMax(1, blockCount());
    while (maxLines >= 10) {
        maxLines /= 10;
        ++digits;
    }

    return 12 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(242, 242, 242));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(
        blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(100, 100, 100));
            painter.drawText(
                0,
                top,
                lineNumberArea->width() - 6,
                fontMetrics().height(),
                Qt::AlignRight,
                number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    updateLineNumberAreaGeometry();
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    updateLineNumberAreaGeometry();
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy != 0) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(
            0,
            rect.y(),
            lineNumberArea->width(),
            rect.height());
    }
}

void CodeEditor::updateLineNumberAreaGeometry()
{
    const QRect area = contentsRect();
    lineNumberArea->setGeometry(
        QRect(area.left(),
              area.top(),
              lineNumberAreaWidth(),
              area.height()));
}
