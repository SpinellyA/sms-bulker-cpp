#include "mainwindow.h"
#include "phonemanagerwindow.h"
#include "devicenumberdialog.h"
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
    QString message = messageEntry->toPlainText();
    QMessageBox::information(this, "SMS Sent", "Sending done.\nMessage:\n" + message);
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
