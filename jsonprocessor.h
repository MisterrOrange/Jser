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

private:
    Dictionary* HandleDictionary(int startindex);
    mio::mmap_source mmap;
    Components *m_rootElement;
};

#endif // JSONPROCESSOR_H
