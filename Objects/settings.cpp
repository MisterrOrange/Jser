#include "settings.h"
#include "ui_settingswindow.h"
#include "../lib/ini.h"
#include <filesystem>
#include <iostream>
#include <fstream>


std::string Settings::stringColour = "";
std::string Settings::numberColour = "";
std::string Settings::floatColour = "";
std::string Settings::booleanColour = "";
std::string Settings::nullColour = "";


void Settings::saveSettings(Ui::SettingsWindow *ui) {
    stringColour = toUpper(ui->stringColour->text().toStdString());
    numberColour = toUpper(ui->numberColour->text().toStdString());
    floatColour = toUpper(ui->floatColour->text().toStdString());
    booleanColour = toUpper(ui->booleanColour->text().toStdString());
    nullColour = toUpper(ui->nullColour->text().toStdString());

    saveSettingsToFile();
}

void Settings::saveSettingsToFile() {
    validateSettings();
    checkIfFileExists();

    mINI::INIFile file("settings.ini");
    mINI::INIStructure ini;
    file.read(ini);

    ini["appearance"]["stringColour"] = stringColour;
    ini["appearance"]["numberColour"] = numberColour;
    ini["appearance"]["floatColour"] = floatColour;
    ini["appearance"]["booleanColour"] = booleanColour;
    ini["appearance"]["nullColour"] = nullColour;

    file.write(ini);
    return;
}

void Settings::loadSettings() {
    checkIfFileExists();
    mINI::INIFile file("settings.ini");
    mINI::INIStructure ini;
    file.read(ini);

    stringColour = ini["appearance"]["stringColour"];
    numberColour = ini["appearance"]["numberColour"];
    floatColour = ini["appearance"]["floatColour"];
    booleanColour = ini["appearance"]["booleanColour"];
    nullColour = ini["appearance"]["nullColour"];

    validateSettings();
}

void Settings::validateSettings() {
    // Check colour codes
    if (!SettingsWindow::verifyHexColour(stringColour))
        stringColour = "CE9178";
    if (!SettingsWindow::verifyHexColour(numberColour))
        numberColour = "B5CEA8";
    if (!SettingsWindow::verifyHexColour(floatColour))
        floatColour = "B5CEA8";
    if (!SettingsWindow::verifyHexColour(booleanColour))
        booleanColour = "569CD6";
    if (!SettingsWindow::verifyHexColour(nullColour))
        nullColour = "569CD6";
}

void Settings::checkIfFileExists() {
    std::filesystem::path file { "settings.ini" };
    if (std::filesystem::exists(file))
        return;
    std::ofstream settings("settings.ini");
    settings.close();
}

std::string Settings::toUpper(std::string string) {
    for (auto & c: string) c = toupper(c);
    return string;
}


std::string Settings::getStringColour() {
    return stringColour;
}

std::string Settings::getNumberColour() {
    return numberColour;
}

std::string Settings::getFloatColour() {
    return floatColour;
}

std::string Settings::getBooleanColour() {
    return booleanColour;
}

std::string Settings::getNullColour() {
    return nullColour;
}
