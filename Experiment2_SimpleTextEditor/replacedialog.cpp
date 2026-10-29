#include "replacedialog.h"
#include "ui_replacedialog.h"

#include <QLineEdit>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);

    connect(ui->findNextButton,
            &QPushButton::clicked,
            this,
            &ReplaceDialog::emitFindRequest);
    connect(ui->replaceButton,
            &QPushButton::clicked,
            this,
            &ReplaceDialog::emitReplaceRequest);
    connect(ui->replaceAllButton,
            &QPushButton::clicked,
            this,
            &ReplaceDialog::emitReplaceAllRequest);
    connect(ui->cancelButton,
            &QPushButton::clicked,
            this,
            &ReplaceDialog::reject);
    connect(ui->findLineEdit,
            &QLineEdit::returnPressed,
            this,
            &ReplaceDialog::emitFindRequest);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::setSearchText(const QString &text)
{
    ui->findLineEdit->setText(text);
    ui->findLineEdit->selectAll();
}

void ReplaceDialog::emitFindRequest()
{
    emit findRequested(ui->findLineEdit->text(),
                       ui->upRadioButton->isChecked(),
                       ui->caseSensitiveCheckBox->isChecked());
}

void ReplaceDialog::emitReplaceRequest()
{
    emit replaceRequested(ui->findLineEdit->text(),
                          ui->replaceLineEdit->text(),
                          ui->upRadioButton->isChecked(),
                          ui->caseSensitiveCheckBox->isChecked());
}

void ReplaceDialog::emitReplaceAllRequest()
{
    emit replaceAllRequested(ui->findLineEdit->text(),
                             ui->replaceLineEdit->text(),
                             ui->caseSensitiveCheckBox->isChecked());
}
