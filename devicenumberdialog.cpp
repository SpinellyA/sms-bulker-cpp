#include "devicenumberdialog.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

DeviceNumberDialog::DeviceNumberDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Set Device Number");
    resize(300, 100);

    QVBoxLayout *layout = new QVBoxLayout(this);

    numberEdit = new QLineEdit(this);
    layout->addWidget(numberEdit);

    QPushButton *okBtn = new QPushButton("OK", this);
    layout->addWidget(okBtn);

    loadNumber();

    connect(okBtn, &QPushButton::clicked, this, [this]() {
        saveNumber(numberEdit->text().trimmed());
        accept();
    });
}

void DeviceNumberDialog::loadNumber() {
    QFile file("device_number.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();
    QString number = obj["number"].toString();
    numberEdit->setText(number);
}

void DeviceNumberDialog::saveNumber(const QString &number) {
    QFile file("device_number.json");
    if (!file.open(QIODevice::WriteOnly)) return;

    QJsonObject obj;
    obj["number"] = number;
    QJsonDocument doc(obj);
    file.write(doc.toJson());
}

QString DeviceNumberDialog::getNumber() const {
    return numberEdit->text().trimmed();
}
