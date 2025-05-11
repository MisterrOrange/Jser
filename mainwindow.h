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

private:
    Ui::MainWindow *ui;
    std::unique_ptr<JsonProcessor> processor;
    void resizeEvent(QResizeEvent *event) override;
    void initializeTreeView(std::string jsonFilePath);

public slots:
    void addItem();
    void openFile();
    void showTreeView();
};
#endif // MAINWINDOW_H
