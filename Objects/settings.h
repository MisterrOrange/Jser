#ifndef SETTINGS_H
#define SETTINGS_H
#include "../settingswindow.h"


class Settings
{
public:
    Settings() = delete;

    static void saveSettings(Ui::SettingsWindow *ui);
    static void deleteData();

private:
    static void validateSettings();
    static std::string toUpper(std::string string);

};

#endif // SETTINGS_H
