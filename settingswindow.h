#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    // Returns true if valid
    static bool verifyHexColour(std::string colour);

private:
    Ui::SettingsWindow *ui;
    void saveSettings();
    void showErrorMessage(std::string message);
};

#endif // SETTINGSWINDOW_H
