#include "dictionary.h"
#include <stdexcept>

Dictionary::Dictionary() {
    m_parent = nullptr;
    Type = kDictionary;
}

Dictionary::Dictionary(Components *parent) {
    m_parent = parent;
    Type = kDictionary;
}

Dictionary::~Dictionary() {
    // Iterate through every key and delete asscociated component
    for (auto const &pair : m_pairs) {
        delete pair.second;
    }
}

void Dictionary::AddValue(std::string key, Components *value) {
    m_pairs[key] = value;
}

Components* Dictionary::GetParent() {
    if (m_parent != nullptr) { return m_parent; }
    throw std::invalid_argument("Parent is null");
}
