#include "phonemanagerwindow.h"
#include "addeditdialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

PhoneManagerWindow::PhoneManagerWindow(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Manage Phone Numbers");
    resize(400, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);

    listWidget = new QListWidget();
    layout->addWidget(listWidget);

    QPushButton *addBtn = new QPushButton("Add Number");
    QPushButton *editBtn = new QPushButton("Edit Number");
    QPushButton *removeBtn = new QPushButton("Remove Number");
    QPushButton *importBtn = new QPushButton("Import From File");
    QPushButton *clearBtn = new QPushButton("Clear List");

    layout->addWidget(addBtn);
    layout->addWidget(editBtn);
    layout->addWidget(removeBtn);
    layout->addWidget(importBtn);
    layout->addWidget(clearBtn);

    connect(addBtn, &QPushButton::clicked, this, &PhoneManagerWindow::addNumber);
    connect(editBtn, &QPushButton::clicked, this, &PhoneManagerWindow::editNumber);
    connect(removeBtn, &QPushButton::clicked, this, &PhoneManagerWindow::removeNumber);
    connect(importBtn, &QPushButton::clicked, this, &PhoneManagerWindow::importFromFile);
    connect(clearBtn, &QPushButton::clicked, this, &PhoneManagerWindow::clearList);

    loadPhoneData();
    updateList();
}

PhoneManagerWindow::~PhoneManagerWindow() {}

void PhoneManagerWindow::loadPhoneData() {
    QFile file("phone_numbers.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray arr = doc.array();
        phoneNumbers.clear();
        for (const QJsonValue &val : arr) {
            if (val.isString()) {
                phoneNumbers.append(val.toString());
            }
        }
        file.close();
    }

    QFile infoFile("phone_info.json");
    if (infoFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(infoFile.readAll());
        QJsonObject obj = doc.object();
        for (const QString &number : obj.keys()) {
            phoneInfo[number] = obj[number].toObject()["name"].toString();
        }
        infoFile.close();
    }
}

void PhoneManagerWindow::savePhoneData() {
    QFile file("phone_numbers.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonArray arr;
        for (const QString &num : phoneNumbers)
            arr.append(num);
        file.write(QJsonDocument(arr).toJson());
        file.close();
    }

    QFile infoFile("phone_info.json");
    if (infoFile.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        for (const QString &num : phoneNumbers) {
            QJsonObject info;
            info["name"] = phoneInfo[num];
            obj[num] = info;
        }
        infoFile.write(QJsonDocument(obj).toJson());
        infoFile.close();
    }
}

void PhoneManagerWindow::updateList() {
    listWidget->clear();
    for (const QString &num : phoneNumbers) {
        QString name = phoneInfo.value(num, "NO NAME");
        listWidget->addItem(name + " - (" + num + ")");
    }
}

void PhoneManagerWindow::addNumber() {
    AddEditDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QString number = dlg.getNumber();
        QString name = dlg.getName();

        if (!phoneNumbers.contains(number)) {
            phoneNumbers.append(number);
            phoneInfo[number] = name;
            updateList();
            savePhoneData();
        } else {
            QMessageBox::warning(this, "Duplicate", "This number already exists.");
        }
    }
}

void PhoneManagerWindow::editNumber() {
    QListWidgetItem *item = listWidget->currentItem();
    if (!item) return;

    QString line = item->text();
    int start = line.indexOf('(');
    int end = line.indexOf(')');
    if (start == -1 || end == -1) return;

    QString number = line.mid(start + 1, end - start - 1);
    QString currentName = phoneInfo.value(number, "");

    AddEditDialog dlg(this, number, currentName, true);
    if (dlg.exec() == QDialog::Accepted) {
        QString newNumber = dlg.getNumber();
        QString newName = dlg.getName();

        if (newNumber != number) {
            phoneNumbers.removeAll(number);
            phoneNumbers.append(newNumber);
            phoneInfo.remove(number);
        }

        phoneInfo[newNumber] = newName;
        updateList();
        savePhoneData();
    }
}

void PhoneManagerWindow::removeNumber() {
    QListWidgetItem *item = listWidget->currentItem();
    if (!item) return;

    QString line = item->text();
    int start = line.indexOf('(');
    int end = line.indexOf(')');
    QString number = line.mid(start + 1, end - start - 1);

    phoneNumbers.removeAll(number);
    phoneInfo.remove(number);
    updateList();
    savePhoneData();
}

void PhoneManagerWindow::clearList() {
    phoneNumbers.clear();
    phoneInfo.clear();
    updateList();
    savePhoneData();
}

void PhoneManagerWindow::importFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import File", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith("09") && line.length() == 11) {
                QString normalized = "+63" + line.mid(1);
                if (!phoneNumbers.contains(normalized)) {
                    phoneNumbers.append(normalized);
                    phoneInfo[normalized] = "NO NAME";
                }
            }
        }
        file.close();
        updateList();
        savePhoneData();
        QMessageBox::information(this, "Import Complete", "Phone numbers imported.");
    }
}
