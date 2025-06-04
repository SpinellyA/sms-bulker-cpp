#include "mainwindow.h"
#include <QDir>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qDebug() << "Current working dir:" << QDir::currentPath();

    w.show();
    return a.exec();
}

