#ifndef ADDEDITDIALOG_H
#define ADDEDITDIALOG_H

#include <QDialog>

class QLineEdit;

class AddEditDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddEditDialog(QWidget *parent = nullptr,
                           const QString &number = "",
                           const QString &name = "",
                           bool editing = false);

    QString getNumber() const;
    QString getName() const;

private:
    QLineEdit *numberEdit;
    QLineEdit *nameEdit;
};

#endif // ADDEDITDIALOG_H
