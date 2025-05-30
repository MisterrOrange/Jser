#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qmimedata.h>
#include <qpushbutton.h>
#include <QFileSystemModel>
#include <QAbstractItemModelTester>
#include <QResizeEvent>
#include <ctime>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <QString>
#include <QMenu>
#include <QClipboard>
#include <QGuiApplication>
#include <QSettings>
#include "Objects/components.h"
#include "processwindow.h"
#include "settingswindow.h"
#include "searchresultwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeView->setHeaderHidden(true);

    QObject::connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(openFile()));
    QObject::connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::openSettings);
    QObject::connect(ui->searchButton, &QPushButton::clicked, this, [this]() {
        SearchResultWindow *resultWindow = new SearchResultWindow(this, this);
        resultWindow->show();
    });
    QSettings settings;
    this->restoreGeometry(settings.value("mainWindowWindowGeometry").toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Json File"), QDir::currentPath(), tr("Json Files (*.json *.txt)"));
    if (filePath == "")
        return;
    initializeTreeView(filePath.toStdString());
}

void MainWindow::openSettings() {
    SettingsWindow *settings = new SettingsWindow(this);
    settings->exec();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    // Resize treeview
    int halfWidth = ui->treeView->viewport()->width() / 2;
    ui->treeView->header()->resizeSection(0, halfWidth);
    ui->treeView->header()->resizeSection(1, halfWidth);
}

void MainWindow::closeEvent(QCloseEvent *e) {
    QSettings settings;
    settings.setValue("mainWindowWindowGeometry", saveGeometry());

    QWidget::closeEvent(e);
}

void MainWindow::initializeTreeView(std::string jsonFilePath) {
    ProcessWindow *progressWindow = new ProcessWindow(this);
    processor.reset();
    processor = std::make_unique<JsonProcessor>(jsonFilePath, false);

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
    progressWindow->exec();
    ui->searchButton->setEnabled(true);
    // Resize treeview
    int halfWidth = ui->treeView->viewport()->width() / 2;
    ui->treeView->header()->resizeSection(0, halfWidth);
    ui->treeView->header()->resizeSection(1, halfWidth);
}

void MainWindow::showTreeView() {
    if (processor->wasSuccessfullyParsed()) {
        ui->treeView->setModel(processor->getModel());
        ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(handleContextMenu(QPoint)));
        return;
    }
    QMessageBox errorBox(this);
    errorBox.setText(QString::fromStdString(processor->getErrorMessage()));
    errorBox.addButton(QMessageBox::StandardButton::Close);
    errorBox.exec();
}

void MainWindow::handleContextMenu(const QPoint &pos) {
    QModelIndex index = ui->treeView->indexAt(pos);
    if (!index.isValid())
        return;
    const Components *item = static_cast<const Components*>(index.internalPointer());


    QMenu menu(this);
    QAction* keyAction = menu.addAction("Copy Key");
    QAction* valueAction = menu.addAction("Copy Value");

    if (!item->isValuePresent()) {
        valueAction->setEnabled(false);
    }

    QObject::connect(keyAction, &QAction::triggered, this, [index] () mutable {
        if (index.column() != 0) {
            index = index.siblingAtColumn(0);
        }
        QString data = index.data(Qt::DisplayRole).toString();
        QGuiApplication::clipboard()->setText(data);
    });
    QObject::connect(valueAction, &QAction::triggered, this, [index] () mutable {
        if (index.column() != 1) {
            index = index.siblingAtColumn(1);
        }
        QString data = index.data(Qt::DisplayRole).toString();
        QGuiApplication::clipboard()->setText(data);
    });
    menu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}

JsonModel* MainWindow::getJsonModel() const {
    if (processor == nullptr)
        return nullptr;
    return processor->getModel();
}

void MainWindow::highlightIndex(QModelIndex index) {
    ui->treeView->scrollTo(index, QAbstractItemView::PositionAtCenter);
    ui->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    const QMimeData *data = event->mimeData();
    QList<QUrl> paths = data->urls();

    if (data->hasUrls() && paths.length() == 1) {
        std::string path = paths[0].toLocalFile().toStdString();
        size_t extensionPosition = path.find_last_of(".");
        std::string extension = path.substr(extensionPosition + 1);

        if (extension == "json" || extension == "txt")
            event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    QUrl path = event->mimeData()->urls()[0];
    initializeTreeView(path.toLocalFile().toStdString());
}
