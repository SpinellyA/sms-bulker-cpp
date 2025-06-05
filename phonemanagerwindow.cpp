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

int checkNumber(const QString &number) {
    if (number.isEmpty()) return 0;

    QString num = number.trimmed();

    if (number.length() == 11 && number.startsWith("09") && number.toLongLong()) return 2;
    if (number.length() == 13 && number.startsWith("+639") && number.mid(1).toLongLong()) return 1;
    if (number.length() == 10 && number.startsWith("9") && number.toLongLong()) return 3;
    if (number.length() == 12 && num.startsWith("639") && num.toLongLong()) return 4;

    return 0;
}


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

void transformNumber(QString& number) {
    int check = checkNumber(number);

    switch(check) {
    case 1:
        break;
    case 2:
        number = "+639" + number.mid(2);
        break;
    case 3:
        number = "+63" + number;
        break;
    case 4:
        number = "+" + number;
        break;
    default:
        break;
    }
}

void PhoneManagerWindow::addNumber() {
    AddEditDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QString number = dlg.getNumber().trimmed();
        QString name = dlg.getName().trimmed();

        if(!checkNumber(number)) {
            QMessageBox::warning(this, "Invalid", "Please enter a valid Philippine phone number.");
        return;
        }

        transformNumber(number);

        if (!phoneNumbers.contains(number)) {
            phoneNumbers.append(number);
            phoneInfo[number] = name.isEmpty() ? "NO NAME" : name;
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

    if(!checkNumber(number)) {
        QMessageBox::warning(this, "Invalid", "Please enter a valid Philippine phone number.");
        return;
    }

    transformNumber(number);

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
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Failed to open file.");
        return;
    }

    QTextStream in(&file);
    QStringList headers;
    if (!in.atEnd()) {
        QString line = in.readLine();
        headers = line.split(',', Qt::SkipEmptyParts);
    }

    // Find best match for name column
    int nameColIndex = -1;
    for (int i = 0; i < headers.size(); ++i) {
        QString col = headers[i].toLower();
        if (col.contains("name") && !col.contains("last") && !col.contains("surname")) {
            nameColIndex = i;
            break;
        }
    }

    QMap<QString, QString> foundNumbers;
    QSet<QString> seenNumbers;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(',', Qt::KeepEmptyParts);
        QString candidateNumber;

        // Search through each column to find a valid number
        for (QString val : fields) {
            val = val.trimmed();

            // Normalize common edge cases
            if(!checkNumber(val)) {
                continue;
            }

            transformNumber(val);

            candidateNumber = val;

            if (!candidateNumber.isEmpty()) break;
        }

        if (candidateNumber.isEmpty()) continue;
        if (seenNumbers.contains(candidateNumber)) continue;

        seenNumbers.insert(candidateNumber);
        QString name = "NO NAME";

        if (nameColIndex >= 0 && nameColIndex < fields.size()) {
            name = fields[nameColIndex].trimmed();
            if (name.isEmpty()) name = "NO NAME";
        }

        foundNumbers[candidateNumber] = name;
    }

    file.close();

    if (foundNumbers.isEmpty()) {
        QMessageBox::information(this, "No Valid Numbers", "No valid phone numbers found in the file.");
        return;
    }

    // Apply found data
    phoneNumbers.clear();
    phoneInfo.clear();

    phoneNumbers = foundNumbers.keys();
    std::sort(phoneNumbers.begin(), phoneNumbers.end());

    for (const QString &number : phoneNumbers) {
        phoneInfo[number] = foundNumbers[number];
    }

    updateList();
    savePhoneData();

    QMessageBox::information(this, "Import Successful",
                             QString("Imported %1 phone numbers.").arg(phoneNumbers.size()));
}

