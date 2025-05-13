#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qpushbutton.h>
#include <QFileSystemModel>
#include <QAbstractItemModelTester>
#include <QResizeEvent>
#include <ctime>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <QString>
#include "processwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeView->setHeaderHidden(true);

    QObject::connect(ui->executeButton, SIGNAL(clicked()), this, SLOT(addItem()));
    QObject::connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(openFile()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addItem() {
    ui->executeButton->setEnabled(false);

    ui->executeButton->setEnabled(true);
}

void MainWindow::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Json File"), QDir::currentPath(), tr("Json Files (*.json *.txt)"));
    if (filePath == "")
        return;
    initializeTreeView(filePath.toStdString());
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    // Resize treeview
    int halfWidth = ui->treeView->viewport()->width() / 2;
    ui->treeView->header()->resizeSection(0, halfWidth);
    ui->treeView->header()->resizeSection(1, halfWidth);
}

void MainWindow::initializeTreeView(std::string jsonFilePath) {
    ProcessWindow *progressWindow = new ProcessWindow(this);
    processor.reset();
    processor = std::make_unique<JsonProcessor>(jsonFilePath, false);

    progressWindow->show();

    QThread *thread = new QThread();
    processor->moveToThread(thread);

    QObject::connect(thread, &QThread::started, processor.get(), &JsonProcessor::Parse);

    // Increase progress bar on progress
    QObject::connect(processor.get(), SIGNAL(progressMade(int)), progressWindow, SLOT(setProgressBar(int)));

    // Clean up on thread completion
    QObject::connect(processor.get(), &JsonProcessor::parsingComplete, progressWindow, &ProcessWindow::destroy);
    QObject::connect(processor.get(), &JsonProcessor::parsingComplete, this, &MainWindow::showTreeView);
    QObject::connect(processor.get(), &JsonProcessor::parsingComplete, thread, &QThread::quit);

    // Delete thread if progress window is destroyed
    QObject::connect(progressWindow, &QObject::destroyed, this, [thread]() { thread->requestInterruption(); });

    ui->treeView->setModel(nullptr);
    thread->start();
}

void MainWindow::showTreeView() {
    if (processor->wasSuccessfullyParsed()) {
        ui->treeView->setModel(processor->getModel());
        return;
    }
    QMessageBox errorBox(this);
    errorBox.setText(QString::fromStdString(processor->getErrorMessage()));
    errorBox.addButton(QMessageBox::StandardButton::Close);
    errorBox.exec();
}
