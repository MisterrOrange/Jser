#ifndef JSONPROCESSOR_H
#define JSONPROCESSOR_H

#include "Objects/components.h"
#include "Objects/DataStructures/dictionary.h"
#include <string>
#include "lib/mio.hpp"



class JsonProcessor
{
public:
    JsonProcessor(std::string path);
    enum Status {
        kInDictionary,
        kInArray,
        kInString
    };

private:
    std::shared_ptr<Components> ParseJson(int startindex);
    mio::mmap_source mmap;
    std::shared_ptr<Components> m_rootElement;
};

#endif // JSONPROCESSOR_H
