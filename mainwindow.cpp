#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qpushbutton.h>
#include <QFileSystemModel>
#include <QAbstractItemModelTester>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->executeButton, SIGNAL(clicked()), this, SLOT(Additem()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Additem() {
    ui->executeButton->setEnabled(false);

    clock_t start = clock();
    processor = new JsonProcessor("C:/Users/admin/source/qt-repos/Jser/Inputs/d2manifest.json");
    // in Milliseconds
    float duration = ((clock() - start) * 1000 / CLOCKS_PER_SEC);
    ui->parseDurationLabel->setText("Parsed in " + QString::number(duration, 'f', 0) + " ms");
    ui->treeView->setModel(processor->getModel());

    ui->executeButton->setEnabled(true);
}
