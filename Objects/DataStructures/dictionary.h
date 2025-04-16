#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "..\components.h"
#include <map>
#include <string>
#include <memory>

class Dictionary : public Components
{
public:
    Dictionary();
    Dictionary(std::shared_ptr<Components> parent);
    void AddValue(std::string key, std::shared_ptr<Components> value);
    std::shared_ptr<Components> GetParent();

    std::map<std::string, std::shared_ptr<Components>> m_pairs;


private:
    std::shared_ptr<Components> m_parent;
};

#endif // DICTIONARY_H
