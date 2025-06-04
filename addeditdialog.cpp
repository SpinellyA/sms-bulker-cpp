#include "addeditdialog.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

AddEditDialog::AddEditDialog(QWidget *parent, const QString &number, const QString &name, bool editing)
    : QDialog(parent)
{
    setWindowTitle(editing ? "Edit Number" : "Add Number");
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Phone Number:"));
    numberEdit = new QLineEdit(number);
    layout->addWidget(numberEdit);

    layout->addWidget(new QLabel("Name:"));
    nameEdit = new QLineEdit(name);
    layout->addWidget(nameEdit);

    QPushButton *saveBtn = new QPushButton("Save");
    layout->addWidget(saveBtn);

    connect(saveBtn, &QPushButton::clicked, this, &AddEditDialog::accept);
}

QString AddEditDialog::getNumber() const {
    return numberEdit->text().trimmed();
}

QString AddEditDialog::getName() const {
    return nameEdit->text().trimmed();
}
