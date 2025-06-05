#include "mainwindow.h"
#include "phonemanagerwindow.h"
#include "devicenumberdialog.h"
#include "smsstatuswindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QStyleFactory>

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
    messageEntry->setMinimumHeight(200);
    layout->addWidget(messageEntry);

    QPushButton *sendButton = new QPushButton("Send SMS");
    layout->addWidget(sendButton);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendSms);

    QPushButton *deviceButton = new QPushButton("Change My Number");
    layout->addWidget(deviceButton);
    connect(deviceButton, &QPushButton::clicked, this, &MainWindow::onManageDevices);

    QPushButton *helpButton = new QPushButton("Message Logs");
    layout->addWidget(helpButton);
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::onLogs);

    setCentralWidget(centralWidget);
    QPushButton *coolButton = new QPushButton("A cool buttom that does things");
    layout->addWidget(coolButton);
    connect(coolButton, &QPushButton::clicked, this, &MainWindow::onCoolButton);
}



MainWindow::~MainWindow() {}

void MainWindow::onSendSms() {
    QString message = messageEntry->toPlainText().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Empty", "Please enter a message first.");
        return;
    }
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

void MainWindow::onLogs() {
    QString path = QDir::currentPath() + "/MessageLogs";
    if (!QDir("MessageLogs").exists()) {
        QMessageBox::information(this, "Logs", "No MessageLogs folder found yet.");
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

int colorMode = 0;

void MainWindow::onCoolButton() {
    QStringList styles = QStyleFactory::keys();

    QString selectedStyle = styles.value(colorMode, "Fusion");
    QApplication::setStyle(QStyleFactory::create(selectedStyle));

    colorMode++;
    if(colorMode >= styles.size()) {
        colorMode = 0;
    }
}
