#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "jsonprocessor.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    JsonModel *getJsonModel() const;

private:
    Ui::MainWindow *ui;
    std::unique_ptr<JsonProcessor> processor;
    void closeEvent(QCloseEvent *e) override;
    void resizeEvent(QResizeEvent *event) override;
    void initializeTreeView(std::string jsonFilePath);
    void openSettings();

public slots:
    void openFile();
    void showTreeView();
    void handleContextMenu(const QPoint &pos);
    void highlightIndex(QModelIndex index);
};
#endif // MAINWINDOW_H
