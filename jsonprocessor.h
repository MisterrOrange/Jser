#ifndef JSONPROCESSOR_H
#define JSONPROCESSOR_H

#include "Objects/components.h"
#include <string>
#include "lib/mio.hpp"
#include "Objects/jsonmodel.h"


class JsonProcessor
{
public:
    JsonProcessor(std::string path);
    enum Status {
        kInDictionary,
        kInArray,
        kInString,
        kInNumber,
        kInFloat
    };
    JsonModel* getModel();

private:
    std::shared_ptr<Components> ParseJson(int startindex = 0);
    char getCharacter(int index);
    mio::mmap_source mmap;
    std::unique_ptr<JsonModel> m_model;
};

#endif // JSONPROCESSOR_H
