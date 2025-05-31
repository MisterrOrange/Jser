#include "searchresultwindow.h"
#include "ui_searchresultwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>


SearchResultWindow::SearchResultWindow(MainWindow *main, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SearchResultWindow)
{
    this->setWindowTitle("Search");
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    m_mainWindow = main;
    QObject::connect(ui->searchButton, &QPushButton::clicked, this, &SearchResultWindow::search);

    QSettings settings;
    ui->matchKeysBox->setCheckState(settings.value("matchKeys", true).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->matchValuesBox->setCheckState(settings.value("matchValues", true).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->caseSensitiveBox->setCheckState(settings.value("caseSensitive", false).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->exactMatchRadio->setChecked(settings.value("exactMatch", false).toBool());

    int matchOption = settings.value("matchOption", 0).toInt();
    ui->matchContainsRadio->setChecked(matchOption == 0);
    ui->matchStartsRadio->setChecked(matchOption == 1);
    ui->matchEndsRadio->setChecked(matchOption == 2);

    ui->regexRadio->setChecked(settings.value("allowRegex", false).toBool());
    ui->wildcardRadio->setChecked(settings.value("allowWildcards", false).toBool());

    this->restoreGeometry(settings.value("searchResultWindowGeometry").toByteArray());


    // Connect exactMatch, wildcard, regex radios
    QObject::connect(ui->exactMatchRadio, &QRadioButton::toggled, this, &SearchResultWindow::handleRadioButton);
    QObject::connect(ui->wildcardRadio, &QRadioButton::toggled, this, &SearchResultWindow::handleRadioButton);
    QObject::connect(ui->regexRadio, &QRadioButton::toggled, this, &SearchResultWindow::handleRadioButton);

    if (matchOption == 3)
        handleRadioButton(true);
    else
        handleRadioButton(false);
}

SearchResultWindow::~SearchResultWindow()
{
    delete ui;
}

void SearchResultWindow::initiateSearch() {
    ui->previousButton->setEnabled(false);
    ui->nextButton->setEnabled(false);


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
    bool exactMatch = ui->exactMatchRadio->isChecked();
    int matchOption = ui->matchContainsRadio->isChecked() ? 0 : ui->matchStartsRadio->isChecked() ? 1 : ui->matchEndsRadio->isChecked() ? 2 : 3;
    bool allowRegex = ui->regexRadio->isChecked();
    bool allowWildcards = ui->wildcardRadio->isChecked();

    Qt::MatchFlags flags = Qt::MatchRecursive;
    if (caseSensitive) flags |= Qt::MatchCaseSensitive;
    if (exactMatch) flags |= Qt::MatchExactly;
    if (matchOption != 3) flags |= matchOption == 0 ? Qt::MatchContains : matchOption == 1 ? Qt::MatchStartsWith : Qt::MatchEndsWith;
    if (allowRegex) flags |= Qt::MatchRegularExpression;
    if (allowWildcards) flags |= Qt::MatchWildcard;



    m_searchProgress = matchKeys + matchValues;
    m_results.clear();

    m_infoBox = new QMessageBox(this);
    m_infoBox->setText("Searching...");
    m_infoBox->setStandardButtons(QMessageBox::NoButton);
    m_infoBox->show();


    if (m_searchProgress == 0)
        return true;

    std::function<void(int)> searchColumn = [this, model, searchTerm, flags](int column) {
        saveSearch(model->match(model->index(0, column), 0, searchTerm, -1, flags), column);
    };

    if (matchKeys) {
        (void)QtConcurrent::run(searchColumn, 0);
    }
    if (matchValues) {
        (void)QtConcurrent::run(searchColumn, 1);
    }

    m_index = 0;
    return true;
}

void SearchResultWindow::saveSearch(QModelIndexList result, int column) {
    JsonModel *model = m_mainWindow->getJsonModel();
    for (const QModelIndex &index : std::as_const(result)) {
        if (column == 0) {
            m_results.append(index);
        }
        if (model->isRealData(index))
            m_results.append(index);
    }

    m_searchProgress--;
    if (m_searchProgress == 0) {
        emit searchFinished();
        m_infoBox->deleteLater();
        initiateSearch();
    }
    return;
}


void SearchResultWindow::closeEvent(QCloseEvent *e) {
    // Get options
    bool matchKeys = ui->matchKeysBox->checkState();
    bool matchValues = ui->matchValuesBox->checkState();
    bool caseSensitive = ui->caseSensitiveBox->checkState();
    bool exactMatch = ui->exactMatchRadio->isChecked();
    int matchOption = ui->matchContainsRadio->isChecked() ? 0 : ui->matchStartsRadio->isChecked() ? 1 : ui->matchEndsRadio->isChecked() ? 2 : 3;
    bool allowRegex = ui->regexRadio->isChecked();
    bool allowWildcards = ui->wildcardRadio->isChecked();

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

void SearchResultWindow::handleRadioButton(bool checked) {
    if (checked) {
        // Turn off box
        ui->caseSensitiveBox->setEnabled(false);
        ui->caseSensitiveBox->setCheckState(Qt::Unchecked);
    }
    else {
        // Turn on
        ui->caseSensitiveBox->setEnabled(true);
    }
}
