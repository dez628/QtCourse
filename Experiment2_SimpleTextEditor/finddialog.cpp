#include "finddialog.h"
#include "ui_finddialog.h"

#include <QLineEdit>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    connect(ui->findNextButton,
            &QPushButton::clicked,
            this,
            &FindDialog::emitFindRequest);
    connect(ui->cancelButton,
            &QPushButton::clicked,
            this,
            &FindDialog::reject);
    connect(ui->findLineEdit,
            &QLineEdit::returnPressed,
            this,
            &FindDialog::emitFindRequest);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::setSearchText(const QString &text)
{
    ui->findLineEdit->setText(text);
    ui->findLineEdit->selectAll();
}

void FindDialog::emitFindRequest()
{
    emit findRequested(ui->findLineEdit->text(),
                       ui->upRadioButton->isChecked(),
                       ui->caseSensitiveCheckBox->isChecked());
}
