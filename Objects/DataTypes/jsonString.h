#ifndef STRING_H
#define STRING_H

#include <string>
#include "..\components.h"



class JsonString : public Components
{
public:
    JsonString(std::string value);
    ~JsonString();

private:
    std::string *m_value;
};

#endif // STRING_H
