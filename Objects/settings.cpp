#include "settings.h"
#include "ui_settingswindow.h"
#include "../lib/ini.h"
#include <filesystem>
#include <fstream>


std::string Settings::stringColour = "";
std::string Settings::numberColour = "";
std::string Settings::floatColour = "";
std::string Settings::booleanColour = "";
std::string Settings::nullColour = "";
std::string Settings::arrayIndexColour = "";
std::string Settings::arrayColour = "";
std::string Settings::dictionaryColour = "";
bool Settings::showStorageName = true;


void Settings::saveSettings(Ui::SettingsWindow *ui) {
    stringColour = toUpper(ui->stringColour->text().toStdString());
    numberColour = toUpper(ui->numberColour->text().toStdString());
    floatColour = toUpper(ui->floatColour->text().toStdString());
    booleanColour = toUpper(ui->booleanColour->text().toStdString());
    nullColour = toUpper(ui->nullColour->text().toStdString());
    arrayIndexColour = toUpper(ui->arrayIndexColour->text().toStdString());
    arrayColour = toUpper(ui->arrayColour->text().toStdString());
    dictionaryColour = toUpper(ui->dictionaryColour->text().toStdString());

    showStorageName = ui->showStorageNameCheckbox->checkState() == Qt::Checked ? true : false;

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
    ini["appearance"]["arrayIndexColour"] = arrayIndexColour;
    ini["appearance"]["arrayColour"] = arrayColour;
    ini["appearance"]["dictionaryColour"] = dictionaryColour;

    ini["appearance"]["showStorageName"] = showStorageName ? "true" : "false";

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
    arrayIndexColour = ini["appearance"]["arrayIndexColour"];
    arrayColour = ini["appearance"]["arrayColour"];
    dictionaryColour = ini["appearance"]["dictionaryColour"];

    showStorageName = ini["appearance"]["showStorageName"] == "false" ? false : true;

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
    if (!SettingsWindow::verifyHexColour(arrayIndexColour))
        arrayIndexColour = "B5CEA8";
    if (!SettingsWindow::verifyHexColour(arrayColour))
        arrayColour = "D4D4D4";
    if (!SettingsWindow::verifyHexColour(dictionaryColour))
        dictionaryColour = "D4D4D4";
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

std::string Settings::getArrayIndexColour() {
    return arrayIndexColour;
}

std::string Settings::getDictionaryColour() {
    return dictionaryColour;
}

std::string Settings::getArrayColour() {
    return arrayColour;
}

bool Settings::getShowStorageName() {
    return showStorageName;
}
