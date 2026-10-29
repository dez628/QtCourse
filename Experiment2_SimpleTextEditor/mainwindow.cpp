#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "codeeditor.h"
#include "finddialog.h"
#include "replacedialog.h"

#include <QAction>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QGuiApplication>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QSaveFile>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , autoSaveTimer(new QTimer(this))
    , findDialog(nullptr)
    , replaceDialog(nullptr)
{
    ui->setupUi(this);

    connect(ui->plainTextEdit->document(),
            &QTextDocument::modificationChanged,
            this,
            &MainWindow::updateWindowTitle);

    connect(ui->plainTextEdit,
            &QPlainTextEdit::undoAvailable,
            ui->actionUndo,
            &QAction::setEnabled);
    connect(ui->plainTextEdit,
            &QPlainTextEdit::redoAvailable,
            ui->actionRedo,
            &QAction::setEnabled);
    connect(ui->plainTextEdit,
            &QPlainTextEdit::copyAvailable,
            this,
            [this](bool available) {
                ui->actionCut->setEnabled(available);
                ui->actionCopy->setEnabled(available);
            });
    connect(QGuiApplication::clipboard(),
            &QClipboard::dataChanged,
            this,
            &MainWindow::updatePasteAction);
    connect(autoSaveTimer,
            &QTimer::timeout,
            this,
            &MainWindow::autoSave);

    autoSaveTimer->setInterval(10000);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    updatePasteAction();
    setupActionIcons();
    setupStatusBar();
    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::on_actionNew_triggered()
{
    if (!maybeSave()) {
        return;
    }

    ui->plainTextEdit->clear();
    currentFilePath.clear();
    ui->plainTextEdit->document()->setModified(false);
    updateWindowTitle();
    statusBar()->showMessage(QStringLiteral("已新建文件"), 2000);
}

void MainWindow::on_actionOpen_triggered()
{
    if (!maybeSave()) {
        return;
    }

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("打开文本文件"),
        currentFilePath.isEmpty() ? QString() : QFileInfo(currentFilePath).absolutePath(),
        QStringLiteral("文本文件 (*.txt);;所有文件 (*)"));

    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionSaveAs_triggered()
{
    saveAs();
}

void MainWindow::on_actionAutoSave_toggled(bool checked)
{
    if (checked) {
        autoSaveTimer->start();
        if (currentFilePath.isEmpty()) {
            statusBar()->showMessage(
                QStringLiteral("自动保存将在首次手动保存文件后生效"), 4000);
        } else {
            statusBar()->showMessage(
                QStringLiteral("自动保存已开启，每 10 秒检查一次"), 3000);
        }
    } else {
        autoSaveTimer->stop();
        statusBar()->showMessage(QStringLiteral("自动保存已关闭"), 3000);
    }
}

bool MainWindow::save()
{
    return currentFilePath.isEmpty() ? saveAs() : saveFile(currentFilePath);
}

bool MainWindow::saveAs()
{
    const QString initialPath = currentFilePath.isEmpty()
        ? QDir::homePath() + QStringLiteral("/新建文本文件.txt")
        : currentFilePath;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("另存为"),
        initialPath,
        QStringLiteral("文本文件 (*.txt);;所有文件 (*)"));

    if (fileName.isEmpty()) {
        return false;
    }

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName += QStringLiteral(".txt");
    }

    return saveFile(fileName);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionUndo_triggered()
{
    ui->plainTextEdit->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    ui->plainTextEdit->redo();
}

void MainWindow::on_actionCut_triggered()
{
    ui->plainTextEdit->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->plainTextEdit->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->plainTextEdit->paste();
}

void MainWindow::on_actionSelectAll_triggered()
{
    ui->plainTextEdit->selectAll();
}

void MainWindow::on_actionAutoWrap_toggled(bool checked)
{
    ui->plainTextEdit->setLineWrapMode(
        checked ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void MainWindow::on_actionFont_triggered()
{
    bool accepted = false;
    const QFont font = QFontDialog::getFont(
        &accepted,
        ui->plainTextEdit->font(),
        this,
        QStringLiteral("选择字体"));

    if (accepted) {
        ui->plainTextEdit->setFont(font);
        ui->plainTextEdit->refreshLineNumberArea();
    }
}

void MainWindow::on_actionFontColor_triggered()
{
    const QColor color = QColorDialog::getColor(
        Qt::black,
        this,
        QStringLiteral("选择字体颜色"));
    if (!color.isValid()) {
        return;
    }

    QTextCharFormat format;
    format.setForeground(color);
    applyTextCharacterFormat(format);
}

void MainWindow::on_actionTextBackgroundColor_triggered()
{
    const QColor color = QColorDialog::getColor(
        Qt::yellow,
        this,
        QStringLiteral("选择字体背景色"));
    if (!color.isValid()) {
        return;
    }

    QTextCharFormat format;
    format.setBackground(color);
    applyTextCharacterFormat(format);
}

void MainWindow::on_actionEditorBackgroundColor_triggered()
{
    const QColor color = QColorDialog::getColor(
        ui->plainTextEdit->palette().color(QPalette::Base),
        this,
        QStringLiteral("选择编辑器背景色"));
    if (!color.isValid()) {
        return;
    }

    QPalette palette = ui->plainTextEdit->palette();
    palette.setColor(QPalette::Base, color);
    ui->plainTextEdit->setPalette(palette);
}

void MainWindow::on_actionShowLineNumbers_toggled(bool checked)
{
    ui->plainTextEdit->setLineNumbersVisible(checked);
}

void MainWindow::on_actionShowToolBar_toggled(bool checked)
{
    ui->mainToolBar->setVisible(checked);
    ui->editToolBar->setVisible(checked);
    ui->formatToolBar->setVisible(checked);
}

void MainWindow::on_actionShowStatusBar_toggled(bool checked)
{
    statusBar()->setVisible(checked);
}

void MainWindow::on_actionFind_triggered()
{
    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog,
                &FindDialog::findRequested,
                this,
                &MainWindow::findText);
    }

    const QString selectedText =
        ui->plainTextEdit->textCursor().selectedText().replace(
            QChar::ParagraphSeparator, QChar('\n'));
    if (!selectedText.isEmpty() && !selectedText.contains(QChar('\n'))) {
        findDialog->setSearchText(selectedText);
    }

    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void MainWindow::on_actionReplace_triggered()
{
    if (!replaceDialog) {
        replaceDialog = new ReplaceDialog(this);
        connect(replaceDialog,
                &ReplaceDialog::findRequested,
                this,
                &MainWindow::findText);
        connect(replaceDialog,
                &ReplaceDialog::replaceRequested,
                this,
                &MainWindow::replaceCurrent);
        connect(replaceDialog,
                &ReplaceDialog::replaceAllRequested,
                this,
                &MainWindow::replaceAll);
    }

    const QString selectedText =
        ui->plainTextEdit->textCursor().selectedText().replace(
            QChar::ParagraphSeparator, QChar('\n'));
    if (!selectedText.isEmpty() && !selectedText.contains(QChar('\n'))) {
        replaceDialog->setSearchText(selectedText);
    }

    replaceDialog->show();
    replaceDialog->raise();
    replaceDialog->activateWindow();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::updateWindowTitle()
{
    const QString modifiedMark = ui->plainTextEdit->document()->isModified()
        ? QStringLiteral("*")
        : QString();

    setWindowTitle(QStringLiteral("%1%2 - 简单文本编辑器")
                       .arg(modifiedMark, displayName()));
}

void MainWindow::updatePasteAction()
{
    const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData();
    ui->actionPaste->setEnabled(mimeData && mimeData->hasText());
}

void MainWindow::updateStatusBar()
{
    const QTextCursor cursor = ui->plainTextEdit->textCursor();
    lengthLabel->setText(
        QStringLiteral("字符数: %1").arg(ui->plainTextEdit->toPlainText().size()));
    linesLabel->setText(
        QStringLiteral("行数: %1").arg(ui->plainTextEdit->blockCount()));
    positionLabel->setText(
        QStringLiteral("第 %1 行, 第 %2 列")
            .arg(cursor.blockNumber() + 1)
            .arg(cursor.positionInBlock() + 1));
}

bool MainWindow::maybeSave()
{
    if (!ui->plainTextEdit->document()->isModified()) {
        return true;
    }

    const QMessageBox::StandardButton result = QMessageBox::warning(
        this,
        QStringLiteral("未保存的更改"),
        QStringLiteral("文档“%1”有未保存的更改。是否保存？").arg(displayName()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (result == QMessageBox::Save) {
        return save();
    }

    return result == QMessageBox::Discard;
}

bool MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(
            this,
            QStringLiteral("打开失败"),
            QStringLiteral("无法读取文件：\n%1\n\n%2")
                .arg(QFileInfo(fileName).absoluteFilePath(), file.errorString()));
        return false;
    }

    const QByteArray data = file.readAll();
    if (data.contains('\0')) {
        QMessageBox::warning(
            this,
            QStringLiteral("无法打开"),
            QStringLiteral("所选文件包含二进制数据，不是有效的文本文件。"));
        return false;
    }

    const QString absolutePath = QFileInfo(fileName).absoluteFilePath();
    ui->plainTextEdit->setPlainText(QString::fromUtf8(data));
    currentFilePath = absolutePath;
    ui->plainTextEdit->document()->setModified(false);
    updateWindowTitle();
    statusBar()->showMessage(QStringLiteral("已打开 %1").arg(absolutePath), 3000);
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(
            this,
            QStringLiteral("保存失败"),
            QStringLiteral("无法写入文件：\n%1\n\n%2")
                .arg(QFileInfo(fileName).absoluteFilePath(), file.errorString()));
        return false;
    }

    const QByteArray data = ui->plainTextEdit->toPlainText().toUtf8();
    if (file.write(data) != data.size() || !file.commit()) {
        QMessageBox::warning(
            this,
            QStringLiteral("保存失败"),
            QStringLiteral("文件没有完整保存：\n%1\n\n%2")
                .arg(QFileInfo(fileName).absoluteFilePath(), file.errorString()));
        return false;
    }

    currentFilePath = QFileInfo(fileName).absoluteFilePath();
    ui->plainTextEdit->document()->setModified(false);
    updateWindowTitle();
    statusBar()->showMessage(QStringLiteral("已保存 %1").arg(currentFilePath), 3000);
    return true;
}

QString MainWindow::displayName() const
{
    return currentFilePath.isEmpty()
        ? QStringLiteral("新建文本文件")
        : QFileInfo(currentFilePath).fileName();
}

void MainWindow::applyTextCharacterFormat(const QTextCharFormat &format)
{
    QTextCursor originalCursor = ui->plainTextEdit->textCursor();
    if (originalCursor.hasSelection()) {
        ui->plainTextEdit->mergeCurrentCharFormat(format);
        return;
    }

    if (originalCursor.document()->isEmpty()) {
        ui->plainTextEdit->mergeCurrentCharFormat(format);
        return;
    }

    const int originalPosition = originalCursor.position();
    originalCursor.select(QTextCursor::Document);
    originalCursor.mergeCharFormat(format);
    originalCursor.clearSelection();
    originalCursor.setPosition(originalPosition);
    ui->plainTextEdit->setTextCursor(originalCursor);
}

void MainWindow::setupStatusBar()
{
    lengthLabel = new QLabel(this);
    linesLabel = new QLabel(this);
    positionLabel = new QLabel(this);

    lengthLabel->setMinimumWidth(100);
    linesLabel->setMinimumWidth(80);
    positionLabel->setMinimumWidth(150);

    lengthLabel->setAlignment(Qt::AlignCenter);
    linesLabel->setAlignment(Qt::AlignCenter);
    positionLabel->setAlignment(Qt::AlignCenter);

    statusBar()->addPermanentWidget(lengthLabel);
    statusBar()->addPermanentWidget(linesLabel);
    statusBar()->addPermanentWidget(positionLabel);

    connect(ui->plainTextEdit,
            &QPlainTextEdit::textChanged,
            this,
            &MainWindow::updateStatusBar);
    connect(ui->plainTextEdit,
            &QPlainTextEdit::cursorPositionChanged,
            this,
            &MainWindow::updateStatusBar);

    updateStatusBar();
}

void MainWindow::setupActionIcons()
{
    ui->actionNew->setIcon(QIcon(QStringLiteral(":/icons/new.svg")));
    ui->actionOpen->setIcon(QIcon(QStringLiteral(":/icons/open.svg")));
    ui->actionSave->setIcon(QIcon(QStringLiteral(":/icons/save.svg")));
    ui->actionSaveAs->setIcon(QIcon(QStringLiteral(":/icons/save-as.svg")));
    ui->actionUndo->setIcon(QIcon(QStringLiteral(":/icons/undo.svg")));
    ui->actionRedo->setIcon(QIcon(QStringLiteral(":/icons/redo.svg")));
    ui->actionCut->setIcon(QIcon(QStringLiteral(":/icons/cut.svg")));
    ui->actionCopy->setIcon(QIcon(QStringLiteral(":/icons/copy.svg")));
    ui->actionPaste->setIcon(QIcon(QStringLiteral(":/icons/paste.svg")));
    ui->actionFind->setIcon(QIcon(QStringLiteral(":/icons/find.svg")));
    ui->actionReplace->setIcon(QIcon(QStringLiteral(":/icons/replace.svg")));
    ui->actionAutoWrap->setIcon(QIcon(QStringLiteral(":/icons/wrap.svg")));
    ui->actionShowLineNumbers->setIcon(
        QIcon(QStringLiteral(":/icons/line-numbers.svg")));
    ui->actionFontColor->setIcon(
        QIcon(QStringLiteral(":/icons/font-color.svg")));
    ui->actionTextBackgroundColor->setIcon(
        QIcon(QStringLiteral(":/icons/text-background.svg")));
    ui->actionEditorBackgroundColor->setIcon(
        QIcon(QStringLiteral(":/icons/editor-background.svg")));

    const QSize iconSize(20, 20);
    const QList<QToolBar *> toolBars = {
        ui->mainToolBar, ui->editToolBar, ui->formatToolBar
    };
    for (QToolBar *toolBar : toolBars) {
        toolBar->setIconSize(iconSize);
        toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
}

void MainWindow::autoSave()
{
    if (currentFilePath.isEmpty()
        || !ui->plainTextEdit->document()->isModified()) {
        return;
    }

    if (saveFile(currentFilePath)) {
        statusBar()->showMessage(
            QStringLiteral("已自动保存：%1").arg(displayName()), 3000);
    }
}

bool MainWindow::findText(const QString &text,
                          bool backward,
                          bool caseSensitive)
{
    if (text.isEmpty()) {
        QMessageBox::information(
            this,
            QStringLiteral("查找"),
            QStringLiteral("请输入要查找的文字。"));
        return false;
    }

    QTextDocument::FindFlags flags;
    if (backward) {
        flags |= QTextDocument::FindBackward;
    }
    if (caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor startCursor = ui->plainTextEdit->textCursor();
    if (startCursor.hasSelection()) {
        startCursor.setPosition(
            backward ? startCursor.selectionStart() : startCursor.selectionEnd());
    }
    ui->plainTextEdit->setTextCursor(startCursor);

    bool found = ui->plainTextEdit->find(text, flags);
    if (!found) {
        QTextCursor wrapCursor(ui->plainTextEdit->document());
        wrapCursor.movePosition(
            backward ? QTextCursor::End : QTextCursor::Start);
        ui->plainTextEdit->setTextCursor(wrapCursor);
        found = ui->plainTextEdit->find(text, flags);
    }

    if (found) {
        ui->plainTextEdit->ensureCursorVisible();
        statusBar()->showMessage(QStringLiteral("已找到“%1”").arg(text), 2000);
        return true;
    }

    QMessageBox::information(
        this,
        QStringLiteral("查找"),
        QStringLiteral("未找到“%1”。").arg(text));
    return false;
}

void MainWindow::replaceCurrent(const QString &text,
                                const QString &replacement,
                                bool backward,
                                bool caseSensitive)
{
    if (text.isEmpty()) {
        QMessageBox::information(
            this,
            QStringLiteral("替换"),
            QStringLiteral("请输入要查找的文字。"));
        return;
    }

    QTextCursor cursor = ui->plainTextEdit->textCursor();
    QString selectedText = cursor.selectedText();
    selectedText.replace(QChar::ParagraphSeparator, QChar('\n'));

    const Qt::CaseSensitivity sensitivity =
        caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    if (!cursor.hasSelection()
        || selectedText.compare(text, sensitivity) != 0) {
        if (!findText(text, backward, caseSensitive)) {
            return;
        }
        return;
    }

    cursor.insertText(replacement);
    ui->plainTextEdit->setTextCursor(cursor);
    statusBar()->showMessage(QStringLiteral("已替换 1 处"), 2000);
    findText(text, backward, caseSensitive);
}

void MainWindow::replaceAll(const QString &text,
                            const QString &replacement,
                            bool caseSensitive)
{
    if (text.isEmpty()) {
        QMessageBox::information(
            this,
            QStringLiteral("替换"),
            QStringLiteral("请输入要查找的文字。"));
        return;
    }

    QTextDocument::FindFlags flags;
    if (caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor searchCursor(ui->plainTextEdit->document());
    searchCursor.movePosition(QTextCursor::Start);
    QTextCursor editCursor = ui->plainTextEdit->textCursor();
    editCursor.beginEditBlock();

    int count = 0;
    QTextCursor match = ui->plainTextEdit->document()->find(
        text, searchCursor, flags);
    while (!match.isNull()) {
        match.insertText(replacement);
        searchCursor = match;
        ++count;
        match = ui->plainTextEdit->document()->find(
            text, searchCursor, flags);
    }

    editCursor.endEditBlock();

    if (count == 0) {
        QMessageBox::information(
            this,
            QStringLiteral("替换"),
            QStringLiteral("未找到“%1”。").arg(text));
        return;
    }

    QMessageBox::information(
        this,
        QStringLiteral("替换"),
        QStringLiteral("替换完成，共替换 %1 处。").arg(count));
}
