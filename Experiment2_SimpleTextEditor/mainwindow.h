#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QCloseEvent;
class QLabel;
class QTextCharFormat;
class QTimer;
class FindDialog;
class ReplaceDialog;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionAutoSave_toggled(bool checked);
    void on_actionExit_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionSelectAll_triggered();
    void on_actionAutoWrap_toggled(bool checked);
    void on_actionFont_triggered();
    void on_actionFontColor_triggered();
    void on_actionTextBackgroundColor_triggered();
    void on_actionEditorBackgroundColor_triggered();
    void on_actionShowLineNumbers_toggled(bool checked);
    void on_actionShowToolBar_toggled(bool checked);
    void on_actionShowStatusBar_toggled(bool checked);
    void on_actionFind_triggered();
    void on_actionReplace_triggered();
    void on_actionAbout_triggered();
    void updateWindowTitle();
    void updatePasteAction();
    void updateStatusBar();

private:
    bool maybeSave();
    bool save();
    bool saveAs();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    QString displayName() const;
    void applyTextCharacterFormat(const QTextCharFormat &format);
    void setupActionIcons();
    void setupStatusBar();
    void autoSave();
    bool findText(const QString &text, bool backward, bool caseSensitive);
    void replaceCurrent(const QString &text,
                        const QString &replacement,
                        bool backward,
                        bool caseSensitive);
    void replaceAll(const QString &text,
                    const QString &replacement,
                    bool caseSensitive);

    Ui::MainWindow *ui;
    QString currentFilePath;
    QLabel *lengthLabel;
    QLabel *linesLabel;
    QLabel *positionLabel;
    QTimer *autoSaveTimer;
    FindDialog *findDialog;
    ReplaceDialog *replaceDialog;
};
#endif // MAINWINDOW_H
