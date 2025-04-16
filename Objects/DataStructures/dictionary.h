#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "..\components.h"
#include <map>
#include <string>

class Dictionary : public Components
{
public:
    Dictionary();
    Dictionary(Components *parent);
    ~Dictionary();
    void AddValue(std::string key, Components *value);
    Components *GetParent();

    std::map<std::string, Components*> m_pairs;


private:
    Components *m_parent;
};

#endif // DICTIONARY_H
