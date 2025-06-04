#include "smsstatuswindow.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QRandomGenerator>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

SMSStatusWindow::SMSStatusWindow(const QString &senderNumber,
                                 const QString &message,
                                 QWidget *parent)
    : QDialog(parent),
    senderNumber(senderNumber),
    message(message)
{
    setWindowTitle("SMS Sending Status");
    resize(500, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);
    table = new QTableWidget(this);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Phone Number", "Status"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(table);

    // Load numbers
    QFile numFile("phone_numbers.json");
    if (numFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(numFile.readAll());
        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr) {
            if (val.isString()) {
                QString number = val.toString();
                if (number != senderNumber) {
                    phoneNumbers.append(number);
                }
            }
        }
        numFile.close();
    }

    // Load info
    QFile infoFile("phone_info.json");
    if (infoFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(infoFile.readAll());
        QJsonObject obj = doc.object();
        for (const QString &number : obj.keys()) {
            QJsonObject inner = obj[number].toObject();
            QString name = inner["name"].toString();
            phoneInfoMap[number] = name.isEmpty() ? "NO NAME" : name;
        }
        infoFile.close();
    }

    initTable();
    simulateSending();
}


SMSStatusWindow::~SMSStatusWindow() {}

void SMSStatusWindow::initTable() {
    table->setRowCount(phoneNumbers.size());

    for (int i = 0; i < phoneNumbers.size(); ++i) {
        QString number = phoneNumbers[i];
        if (number == senderNumber)
            continue;

        QTableWidgetItem *numItem = new QTableWidgetItem(number);
        QTableWidgetItem *statusItem = new QTableWidgetItem("Pending");

        table->setItem(i, 0, numItem);
        table->setItem(i, 1, statusItem);
    }

    // Prepare log folder
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString("yyyy-MM-dd_hh-mm-ss");
    logFolderPath = QString("MessageLogs/%1").arg(senderNumber);

    QDir dir;
    if (!dir.exists(logFolderPath)) {
        dir.mkpath(logFolderPath);
    }

    logFolderPath += QString("/%1.txt").arg(timestamp);
}

void SMSStatusWindow::simulateSending() {
    for (int i = 0; i < phoneNumbers.size(); ++i) {
        QString number = phoneNumbers[i];
        if (number == senderNumber)
            continue;

        int delay = QRandomGenerator::global()->bounded(5000); // 0 to 4999 ms

        QTimer::singleShot(delay, this, [=]() {
            updateStatus(number, "Sent");
            logMessage(number, phoneInfoMap.value(number, "NO NAME"));

            sentCount++;
            if (sentCount == phoneNumbers.size() - 1) {
                finishSending();
            }
        });

        updateStatus(number, "Sending");
    }
}

void SMSStatusWindow::updateStatus(const QString &number, const QString &status) {
    for (int row = 0; row < table->rowCount(); ++row) {
        if (table->item(row, 0) && table->item(row, 0)->text() == number) {
            table->item(row, 1)->setText(status);
            break;
        }
    }
}

void SMSStatusWindow::logMessage(const QString &recipientNumber, const QString &recipientName) {
    QFile logFile(logFolderPath);
    if (!logFile.open(QIODevice::Append | QIODevice::Text)) return;

    QTextStream stream(&logFile);
    stream << "\n\n";
    stream << "SENT TO: " << recipientNumber << ", " << recipientName << "\n";
    stream << "Message: " << message << "\n\n";
    logFile.close();
}

void SMSStatusWindow::finishSending() {
    QMessageBox::information(this, "Success",
                             QString("Successfully sent to %1/%2 numbers!")
                             .arg(sentCount).arg(phoneNumbers.size() - 1));
}
