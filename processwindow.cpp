#include "processwindow.h"
#include "ui_processwindow.h"

ProcessWindow::ProcessWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProcessWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    setProgressBar(0);
}

ProcessWindow::~ProcessWindow()
{
    delete ui;
}

void ProcessWindow::setProgressBar(int progress) {
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(progress);
}

void ProcessWindow::destroy() {
    delete this;
}
