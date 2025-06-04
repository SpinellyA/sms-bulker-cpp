#include "mainwindow.h"
#include "phonemanagerwindow.h"
#include "devicenumberdialog.h"
#include "smsstatuswindow.h"
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("SMS Gateway Sender");
    this->resize(400, 400);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QPushButton *phoneButton = new QPushButton("Manage Phone Numbers");
    layout->addWidget(phoneButton);
    connect(phoneButton, &QPushButton::clicked, this, &MainWindow::onManagePhones);

    QLabel *messageLabel = new QLabel("Message:");
    messageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(messageLabel);

    messageEntry = new QTextEdit();
    messageEntry->setFixedHeight(100);
    layout->addWidget(messageEntry);

    QPushButton *sendButton = new QPushButton("Send SMS");
    layout->addWidget(sendButton);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendSms);

    QPushButton *deviceButton = new QPushButton("Change My Number");
    layout->addWidget(deviceButton);
    connect(deviceButton, &QPushButton::clicked, this, &MainWindow::onManageDevices);

    QPushButton *helpButton = new QPushButton("Help and FAQ");
    layout->addWidget(helpButton);
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::onHelp);

    QLabel *tosLabel = new QLabel("Please note that your messages are monitored to enforce TOS.");
    layout->addWidget(tosLabel);

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::onSendSms() {
    QString message = messageEntry->toPlainText().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Empty", "Please enter a message first.");
        return;
    }
    // Load numbers from file or memory
    QStringList numbers;
    QFile file("phone_numbers.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr) {
            if (val.isString()) {
                numbers.append(val.toString());
            }
        }
    }

    QString sender;
    QFile devFile("device_number.json");
    if (devFile.open(QIODevice::ReadOnly)) {
        QJsonObject obj = QJsonDocument::fromJson(devFile.readAll()).object();
        sender = obj["number"].toString();
    }

    if (sender.isEmpty()) {
        QMessageBox::warning(this, "No Sender", "Please set a device number in Manage Devices first.");
        return;
    }

    SMSStatusWindow *window = new SMSStatusWindow(sender, message, this);
    window->exec();
}

void MainWindow::onManagePhones() {
    PhoneManagerWindow *window = new PhoneManagerWindow(this);
    window->exec();
}

void MainWindow::onManageDevices() {
    DeviceNumberDialog *dialog = new DeviceNumberDialog(this);
    dialog->setWindowTitle("Change My Number");
    dialog->exec();
}

void MainWindow::onHelp() {
    QMessageBox::information(this, "Help & FAQ", "This would show the help/FAQ content.");
}
