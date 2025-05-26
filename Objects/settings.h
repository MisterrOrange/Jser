#ifndef SETTINGS_H
#define SETTINGS_H
#include "../settingswindow.h"


class Settings
{
public:
    Settings() = delete;

    static void saveSettings(Ui::SettingsWindow *ui);
    static void loadSettings();

    static std::string getStringColour();
    static std::string getNumberColour();
    static std::string getFloatColour();
    static std::string getBooleanColour();
    static std::string getNullColour();

private:
    static void saveSettingsToFile();
    // Also creates it if it doesn't exist
    static void checkIfFileExists();
    static void validateSettings();
    static std::string toUpper(std::string string);

    static std::string stringColour;
    static std::string numberColour;
    static std::string floatColour;
    static std::string booleanColour;
    static std::string nullColour;
};

#endif // SETTINGS_H
