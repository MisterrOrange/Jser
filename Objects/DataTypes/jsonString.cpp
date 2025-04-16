#include "jsonString.h"

JsonString::JsonString(std::string value) {
    m_value = new std::string(value);
    Type = kString;
}

JsonString::~JsonString() {
    delete m_value;
}
