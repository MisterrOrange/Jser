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

    clock_t start = clock();
    processor = new JsonProcessor("C:/Users/admin/source/qt-repos/Jser/Inputs/2.json");
    // in Milliseconds
    float duration = ((clock() - start) * 1000 / CLOCKS_PER_SEC);
    ui->parseDurationLabel->setText(QString::fromStdString("Parsed in " + std::to_string(duration) + " ms"));
    ui->treeView->setModel(processor->getModel());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Additem() {
    ui->executeButton->setEnabled(false);

    JsonModel *model = processor->getModel();

    ui->treeView->setModel(model);

    ui->executeButton->setEnabled(true);
}
