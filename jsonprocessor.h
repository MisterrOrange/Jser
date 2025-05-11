#ifndef JSONPROCESSOR_H
#define JSONPROCESSOR_H

#include "Objects/components.h"
#include <string>
#include "lib/mio.hpp"
#include "Objects/jsonmodel.h"
#include <QObject>


class JsonProcessor : public QObject
{
    Q_OBJECT

public:
    JsonProcessor(std::string path, boolean parse = true);
    void Parse();
    enum Status {
        kInDictionary,
        kInArray,
        kInString,
        kInNumber,
        kInFloat
    };
    JsonModel* getModel();

private:
    void ParseJson(int startindex = 0);
    char getCharacter(int index);
    mio::mmap_source mmap;
    std::unique_ptr<JsonModel> m_model;

    // In milliseconds
    int parseTime = 0;

signals:
    void progressMade(int percentage);
    void parsingComplete();
};

#endif // JSONPROCESSOR_H
