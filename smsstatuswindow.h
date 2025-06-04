#ifndef SMSSTATUSWINDOW_H
#define SMSSTATUSWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QMap>
#include <QStringList>
#include <QTimer>
#include <QDateTime>

class SMSStatusWindow : public QDialog {
    Q_OBJECT

public:
    explicit SMSStatusWindow(
                             const QString &senderNumber,
                             const QString &message,
                             QWidget *parent = nullptr);
    ~SMSStatusWindow();

private:
    QTableWidget *table;
    QMap<QString, QString> phoneInfoMap;  // number → name
    QStringList phoneNumbers;
    QString senderNumber;
    QString message;
    QString logFolderPath;

    int sentCount = 0;

    void initTable();
    void simulateSending();
    void updateStatus(const QString &number, const QString &status);
    void logMessage(const QString &recipientNumber, const QString &recipientName);
    void finishSending();
};

#endif // SMSSTATUSWINDOW_H
