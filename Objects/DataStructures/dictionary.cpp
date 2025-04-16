#include "dictionary.h"
#include <stdexcept>

Dictionary::Dictionary() {
    m_parent = nullptr;
    Type = kDictionary;
}

Dictionary::Dictionary(std::shared_ptr<Components> parent) {
    m_parent = parent;
    Type = kDictionary;
}

void Dictionary::AddValue(std::string key, std::shared_ptr<Components> value) {
    m_pairs[key] = value;
}

std::shared_ptr<Components> Dictionary::GetParent() {
    if (m_parent != nullptr) { return m_parent; }
    throw std::invalid_argument("Parent is null");
}
