#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "Objects/settings.h"
#include <QMessageBox>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");
    QObject::connect(ui->saveButton, &QPushButton::clicked, this, &SettingsWindow::saveSettings);
    QObject::connect(ui->cancelButton, &QPushButton::clicked, this, [this] () {
        delete this;
    });

    ui->stringColour->setText(QString::fromStdString(Settings::getStringColour()));
    ui->numberColour->setText(QString::fromStdString(Settings::getNumberColour()));
    ui->floatColour->setText(QString::fromStdString(Settings::getFloatColour()));
    ui->booleanColour->setText(QString::fromStdString(Settings::getBooleanColour()));
    ui->nullColour->setText(QString::fromStdString(Settings::getNullColour()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::saveSettings() {
    // Verifies hex colour codes
    if (!verifyHexColour(ui->stringColour->text().toStdString()) ||
        !verifyHexColour(ui->numberColour->text().toStdString()) ||
        !verifyHexColour(ui->floatColour->text().toStdString()) ||
        !verifyHexColour(ui->booleanColour->text().toStdString()) ||
        !verifyHexColour(ui->nullColour->text().toStdString())) {
        showErrorMessage("Invalid hex colour");
        return;
    }
    Settings::saveSettings(ui);
    delete this;
}

void SettingsWindow::showErrorMessage(std::string message) {
    QMessageBox errorBox(this);
    errorBox.setText(QString::fromStdString(message));
    errorBox.exec();
    return;
}

bool SettingsWindow::verifyHexColour(std::string colour) {
    if (colour.length() != 6)
        return false;
    return std::all_of(colour.begin(), colour.end(), [](char c) {
        return std::isxdigit(static_cast<unsigned char>(c));
    });
}
