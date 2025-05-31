#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "Objects/settings.h"
#include <QMessageBox>
#include <QSettings>

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
    QObject::connect(ui->deleteDataButton, &QPushButton::clicked, this, [this] () {
        Settings::deleteData();
        delete this; // To make it feel like something happened
    });

    QSettings settings;
    ui->stringColour->setText(settings.value("stringColour", "CE9178").toString());
    ui->numberColour->setText(settings.value("numberColour", "B5CEA8").toString());
    ui->floatColour->setText(settings.value("floatColour", "B5CEA8").toString());
    ui->booleanColour->setText(settings.value("booleanColour", "569CD6").toString());
    ui->nullColour->setText(settings.value("nullColour", "569CD6").toString());
    ui->arrayIndexColour->setText(settings.value("arrayIndexColour", "B5CEA8").toString());
    ui->arrayColour->setText(settings.value("arrayColour", "D4D4D4").toString());
    ui->dictionaryColour->setText(settings.value("dictionaryColour", "D4D4D4").toString());

    ui->showStorageNameCheckbox->setCheckState(settings.value("showStorageName", true).toBool() ? Qt::Checked : Qt::Unchecked);

    QObject::connect(ui->showStorageNameCheckbox, &QCheckBox::checkStateChanged, this, &SettingsWindow::handleShowStorageNameCheckbox);
    handleShowStorageNameCheckbox(ui->showStorageNameCheckbox->checkState());
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
        !verifyHexColour(ui->nullColour->text().toStdString()) ||
        !verifyHexColour(ui->arrayIndexColour->text().toStdString()) ||
        !verifyHexColour(ui->dictionaryColour->text().toStdString()) ||
        !verifyHexColour(ui->arrayColour->text().toStdString())) {
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

void SettingsWindow::handleShowStorageNameCheckbox(Qt::CheckState state) {
    if (state == Qt::Checked) {
        ui->arrayColour->setEnabled(true);
        ui->dictionaryColour->setEnabled(true);
    }
    else {
        ui->arrayColour->setEnabled(false);
        ui->dictionaryColour->setEnabled(false);
    }
}
