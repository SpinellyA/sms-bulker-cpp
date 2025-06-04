#ifndef PHONEMANAGERWINDOW_H
#define PHONEMANAGERWINDOW_H

#include <QDialog>
#include <QListWidget>
#include <QMap>
#include <QString>

class PhoneManagerWindow : public QDialog {
    Q_OBJECT

public:
    explicit PhoneManagerWindow(QWidget *parent = nullptr);
    ~PhoneManagerWindow();

private slots:
    void addNumber();
    void editNumber();
    void removeNumber();
    void clearList();
    void importFromFile();

private:
    QListWidget *listWidget;

    QStringList phoneNumbers;
    QMap<QString, QString> phoneInfo; // number → name mapping i hope it works

    void loadPhoneData();
    void savePhoneData();
    void updateList();
};

#endif
