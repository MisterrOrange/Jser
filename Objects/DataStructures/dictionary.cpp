#include "dictionary.h"

Dictionary::Dictionary() {}

Dictionary::~Dictionary() {
    // Iterate through every key and delete asscociated component
    for (auto const &pair : m_pairs) {
        delete pair.second;
    }
}

void Dictionary::AddValue(std::string key, Components *value) {
    m_pairs[key] = value;
}
