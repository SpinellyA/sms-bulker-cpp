#ifndef DEVICENUMBERDIALOG_H
#define DEVICENUMBERDIALOG_H

#include <QDialog>

class QLineEdit;

class DeviceNumberDialog : public QDialog {
    Q_OBJECT

public:
    explicit DeviceNumberDialog(QWidget *parent = nullptr);
    QString getNumber() const;

private:
    QLineEdit *numberEdit;
    void loadNumber();
    void saveNumber(const QString &number);
};

#endif
