#include "searchresultwindow.h"
#include "ui_searchresultwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QSettings>

SearchResultWindow::SearchResultWindow(MainWindow *main, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SearchResultWindow)
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    m_mainWindow = main;
    QObject::connect(ui->searchButton, &QPushButton::clicked, this, &SearchResultWindow::initiateSearch);

    QSettings settings;
    ui->matchKeysBox->setCheckState(settings.value("matchKeys", true).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->matchValuesBox->setCheckState(settings.value("matchValues", true).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->caseSensitiveBox->setCheckState(settings.value("caseSensitive", false).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->exactMatchBox->setCheckState(settings.value("exactMatch", false).toBool() ? Qt::Checked : Qt::Unchecked);

    int matchOption = settings.value("matchOption", 0).toInt();
    ui->matchContainsRadio->setChecked(matchOption == 0);
    ui->matchStartsRadio->setChecked(matchOption == 1);
    ui->matchEndsRadio->setChecked(matchOption == 2);

    ui->regexBox->setCheckState(settings.value("allowRegex", false).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->wildcardsBox->setCheckState(settings.value("allowWildcards", false).toBool() ? Qt::Checked : Qt::Unchecked);

    this->restoreGeometry(settings.value("searchResultWindowGeometry").toByteArray());
}

SearchResultWindow::~SearchResultWindow()
{
    delete ui;
}

void SearchResultWindow::initiateSearch() {
    ui->previousButton->setEnabled(false);
    ui->nextButton->setEnabled(false);

    if (!search())
        return;

    if (m_results.length() == 0) {
        ui->resultLabel->setText("0 results found");
        return;
    }

    std::function<void(bool)> highlightItem = [this](bool nextItem) {
        if (nextItem)
            this->m_index = ++this->m_index % this->m_results.length();
        else
            // Making sure index doesn't go negative
            this->m_index = (--this->m_index + this->m_results.length()) % this->m_results.length();
        QModelIndex index = this->m_results[this->m_index];
        this->m_mainWindow->highlightIndex(index);
        this->ui->resultLabel->setText("Result " + QString::number(this->m_index + 1) + " of " + QString::number(this->m_results.length()));
    };

    // Disconnect every signal so it doesn't send it multiple times
    QObject::disconnect(ui->previousButton, nullptr, nullptr, nullptr);
    QObject::disconnect(ui->nextButton, nullptr, nullptr, nullptr);

    QObject::connect(ui->previousButton, &QPushButton::clicked, this, [highlightItem]() {
        highlightItem(false);
    });
    QObject::connect(ui->nextButton, &QPushButton::clicked, this, [highlightItem]() {
        highlightItem(true);
    });
    ui->previousButton->setEnabled(true);
    ui->nextButton->setEnabled(true);

    // Brings index to beginning and triggers the text
    m_index = 1;
    highlightItem(false);
}

bool SearchResultWindow::search() {
    JsonModel *model = m_mainWindow->getJsonModel();
    if (model == nullptr) {
        QMessageBox box(this);
        box.setText("Open a file before using this!");
        box.exec();
        return false;
    }

    // Get search string
    QString searchTerm = ui->searchLineEdit->text();
    if (searchTerm == "") {
        QMessageBox box(this);
        box.setText("Search can't be empty");
        box.exec();
        return false;
    }
    // Get options
    bool matchKeys = ui->matchKeysBox->checkState();
    bool matchValues = ui->matchValuesBox->checkState();
    bool caseSensitive = ui->caseSensitiveBox->checkState();
    bool exactMatch = ui->exactMatchBox->checkState();
    int matchOption = ui->matchContainsRadio->isChecked() ? 0 : ui->matchStartsRadio ? 1 : 2;
    bool allowRegex = ui->regexBox->checkState();
    bool allowWildcards = ui->wildcardsBox->checkState();

    Qt::MatchFlags flags = Qt::MatchRecursive | Qt::MatchContains;
    if (caseSensitive) flags |= Qt::MatchCaseSensitive;
    if (exactMatch) flags |= Qt::MatchExactly;
    flags |= matchOption == 0 ? Qt::MatchContains : matchOption == 1 ? Qt::MatchStartsWith : Qt::MatchEndsWith;
    if (allowRegex) flags |= Qt::MatchRegularExpression;
    if (allowWildcards) flags |= Qt::MatchWildcard;

    QModelIndexList results;
    if (matchKeys) {
        QModelIndex column0Index = model->index(0, 0);
        results = model->match(column0Index, 0, searchTerm, -1, flags);
    }
    if (matchValues) {
        QModelIndex column1Index = model->index(0, 1);
        QModelIndexList results2 = model->match(column1Index, 0, searchTerm, -1, flags);
        // Strip "fake" data from column 1
        for (const QModelIndex &index : std::as_const(results2)) {
            if (model->isRealData(index))
                results.append(index);
        }
    }

    m_results = results;
    m_index = 0;
    return true;
}

void SearchResultWindow::closeEvent(QCloseEvent *e) {
    // Get options
    bool matchKeys = ui->matchKeysBox->checkState();
    bool matchValues = ui->matchValuesBox->checkState();
    bool caseSensitive = ui->caseSensitiveBox->checkState();
    bool exactMatch = ui->exactMatchBox->checkState();
    int matchOption = ui->matchContainsRadio->isChecked() ? 0 : ui->matchStartsRadio ? 1 : 2;
    bool allowRegex = ui->regexBox->checkState();
    bool allowWildcards = ui->wildcardsBox->checkState();

    // Save settings
    QSettings settings;
    settings.setValue("matchKeys", matchKeys);
    settings.setValue("matchValues", matchValues);
    settings.setValue("caseSensitive", caseSensitive);
    settings.setValue("exactMatch", exactMatch);
    settings.setValue("matchOption", matchOption);
    settings.setValue("allowRegex", allowRegex);
    settings.setValue("allowWildcards", allowWildcards);

    settings.setValue("searchResultWindowGeometry", saveGeometry());
    QWidget::closeEvent(e);
}
