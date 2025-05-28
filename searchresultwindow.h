#ifndef SEARCHRESULTWINDOW_H
#define SEARCHRESULTWINDOW_H

#include "mainwindow.h"
#include <QDialog>
#include <QModelIndex>

namespace Ui {
class SearchResultWindow;
}

class SearchResultWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SearchResultWindow(MainWindow *main, QWidget *parent = nullptr);
    ~SearchResultWindow();

private:
    Ui::SearchResultWindow *ui;
    void closeEvent(QCloseEvent *e) override;
    MainWindow *m_mainWindow;
    // Returns true on success
    bool search();
    QModelIndexList m_results;
    int m_index;

public slots:
    void initiateSearch();
};

#endif // SEARCHRESULTWINDOW_H
