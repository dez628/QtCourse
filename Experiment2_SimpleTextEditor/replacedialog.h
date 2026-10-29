#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class ReplaceDialog;
}
QT_END_NAMESPACE

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog() override;

    void setSearchText(const QString &text);

signals:
    void findRequested(const QString &text,
                       bool backward,
                       bool caseSensitive);
    void replaceRequested(const QString &text,
                          const QString &replacement,
                          bool backward,
                          bool caseSensitive);
    void replaceAllRequested(const QString &text,
                             const QString &replacement,
                             bool caseSensitive);

private slots:
    void emitFindRequest();
    void emitReplaceRequest();
    void emitReplaceAllRequest();

private:
    Ui::ReplaceDialog *ui;
};

#endif // REPLACEDIALOG_H
