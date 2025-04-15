#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "..\components.h"
#include <map>
#include <string>

class Dictionary : public Components
{
public:
    Dictionary();
    ~Dictionary();
    void AddValue(std::string key, Components *value);

private:
    std::map<std::string, Components*> m_pairs;
};

#endif // DICTIONARY_H
