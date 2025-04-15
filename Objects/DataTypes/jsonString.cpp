#include "jsonString.h"

JsonString::JsonString(std::string value) {
    m_value = new std::string(value);
}

JsonString::~JsonString() {
    delete m_value;
}
