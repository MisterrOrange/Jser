#ifndef JSONPROCESSOR_H
#define JSONPROCESSOR_H

#include <string>
#include "lib/mio.hpp"
#include "Objects/jsonmodel.h"
#include <QObject>


class JsonProcessor : public QObject
{
    Q_OBJECT

public:
    JsonProcessor(std::string path, boolean parse = true);
    enum Status {
        kInDictionary,
        kInArray,
        kInString,
        kInNumber,
        kInFloat
    };
    JsonModel* getModel();

    boolean wasSuccessfullyParsed();
    std::string getErrorMessage();

private:
    void ParseJson(int startindex = 0);
    char getCharacter(int index);

    boolean setMap();
    void deleteMap();

    std::string m_filePath;
    mio::mmap_source m_map;
    std::unique_ptr<JsonModel> m_model;
    // In milliseconds
    int m_parseTime = 0;

    boolean m_successfullyParsed;
    std::string m_errorMessage;
    // captureBefore specifies how many characters before error should be displayed. same goes for captureAfter
    void logError(std::string message, int position, int captureBefore, int captureAfter);

signals:
    void progressMade(int percentage);
    void parsingComplete();

public slots:
    void Parse();
};

#endif // JSONPROCESSOR_H
