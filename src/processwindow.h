#ifndef PROCESSWINDOW_H
#define PROCESSWINDOW_H

#include <QDialog>

namespace Ui {
class ProcessWindow;
}

class ProcessWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessWindow(QWidget *parent = nullptr);
    ~ProcessWindow();

private:
    Ui::ProcessWindow *ui;

public slots:
    void setProgressBar(int progress);
    void destroy();
};

#endif // PROCESSWINDOW_H
