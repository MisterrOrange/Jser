#include "settings.h"
#include "ui_settingswindow.h"
#include <QSettings>


void Settings::saveSettings(Ui::SettingsWindow *ui) {
    QSettings settings;
    settings.setValue("stringColour", ui->stringColour->text());
    settings.setValue("numberColour", ui->numberColour->text());
    settings.setValue("floatColour", ui->floatColour->text());
    settings.setValue("booleanColour", ui->booleanColour->text());
    settings.setValue("nullColour", ui->nullColour->text());
    settings.setValue("arrayIndexColour", ui->arrayIndexColour->text());
    settings.setValue("arrayColour", ui->arrayColour->text());
    settings.setValue("dictionaryColour", ui->dictionaryColour->text());

    settings.setValue("showStorageName", ui->showStorageNameCheckbox->checkState() == Qt::Checked);

}


void Settings::validateSettings() {
    QSettings settings;
    // Check colour codes    
    if (!SettingsWindow::verifyHexColour(settings.value("stringColour").toString().toStdString()))
        settings.setValue("stringColour", "CE9178");

    if (!SettingsWindow::verifyHexColour(settings.value("numberColour").toString().toStdString()))
        settings.setValue("numberColour", "B5CEA8");

    if (!SettingsWindow::verifyHexColour(settings.value("floatColour").toString().toStdString()))
        settings.setValue("floatColour", "B5CEA8");

    if (!SettingsWindow::verifyHexColour(settings.value("booleanColour").toString().toStdString()))
        settings.setValue("booleanColour", "569CD6");

    if (!SettingsWindow::verifyHexColour(settings.value("nullColour").toString().toStdString()))
        settings.setValue("nullColour", "569CD6");

    if (!SettingsWindow::verifyHexColour(settings.value("arrayIndexColour").toString().toStdString()))
        settings.setValue("arrayIndexColour", "B5CEA8");

    if (!SettingsWindow::verifyHexColour(settings.value("arrayColour").toString().toStdString()))
        settings.setValue("arrayColour", "D4D4D4");

    if (!SettingsWindow::verifyHexColour(settings.value("dictionaryColour").toString().toStdString()))
        settings.setValue("dictionaryColour", "D4D4D4");
}

std::string Settings::toUpper(std::string string) {
    for (auto & c: string) c = toupper(c);
    return string;
}
