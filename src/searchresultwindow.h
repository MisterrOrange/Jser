#ifndef SEARCHRESULTWINDOW_H
#define SEARCHRESULTWINDOW_H

#include "mainwindow.h"
#include <QDialog>
#include <QModelIndex>
#include <QMessageBox>

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
    void saveSearch(QModelIndexList result, int column);
    int m_searchProgress;
    QModelIndexList m_results;
    QMessageBox *m_infoBox;
    int m_index;

public slots:
    void initiateSearch();
    void handleRadioButton(bool checked);

signals:
    void searchFinished();
};

#endif // SEARCHRESULTWINDOW_H
