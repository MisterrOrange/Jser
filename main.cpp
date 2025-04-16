#include "mainwindow.h"
#include "jsonprocessor.h"

#include <QApplication>
#include <QPushButton>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // Process json
    JsonProcessor processor("C:/Users/admin/source/qt-repos/Jser/Inputs/2.json");

    return a.exec();
}
