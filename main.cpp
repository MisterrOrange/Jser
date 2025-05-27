#include "mainwindow.h"

#include <QApplication>
#include <QPushButton>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("MisterOrange");
    QCoreApplication::setApplicationName("Jser");

    MainWindow w;
    w.show();

    return a.exec();
}
