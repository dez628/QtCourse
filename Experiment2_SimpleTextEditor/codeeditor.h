#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;
class QWidget;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void setLineNumbersVisible(bool visible);
    void refreshLineNumberArea();

    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    void updateLineNumberAreaGeometry();

    QWidget *lineNumberArea;
    bool lineNumbersVisible;
};

#endif // CODEEDITOR_H
