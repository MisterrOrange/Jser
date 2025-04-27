#include "jsonprocessor.h"

#include <stack>

// TODO:
// - Add way to print Dictionary
// - Remove recursion from this (DONE)
// - Use smart pointers



JsonProcessor::JsonProcessor(std::string path) {
    std::error_code error;
    mmap = mio::make_mmap_source(path, error);


    m_rootElement = ParseJson(0);
}

std::shared_ptr<Components> JsonProcessor::ParseJson(int startIndex) {
    std::shared_ptr<Components> currentComponent = nullptr;

    std::stack<Status> state;
    std::string key;
    std::string currentString;
    // Stores whether next item is value or not
    bool valueIncoming = false;
    for (int it = startIndex; it < mmap.size(); ++it) {
        char character = mmap[it];

        // Capture characters for string
        if (!state.empty() && state.top() == kInString && character != '"') {
            currentString += character;
            continue;
        }

        switch (character) {
        case '{':
            // Generate child
            if (state.empty()) { currentComponent = std::shared_ptr<Components>(new Components(Components::kDictionary)); }
            else               { currentComponent = std::shared_ptr<Components>(new Components(Components::kDictionary, currentComponent.get())); }

            // Point parent to child
            if (!state.empty()) {
                if (state.top() == kInDictionary) {
                    Components *parent = currentComponent->parent();
                    parent->addChild(currentComponent);
                }
            }
            state.push(kInDictionary);
            valueIncoming = false;
            break;

        case '}':
            state.pop();
            if (!state.empty()) {
                if (currentComponent == nullptr) { throw std::invalid_argument("Component pointer is null"); }
                // Set current to parent
                currentComponent = std::static_pointer_cast<Components>(std::shared_ptr<Components>(currentComponent->parent()));
                if (state.top() == kInDictionary) { valueIncoming = false; }
            }
            break;

        case '"':
            // If is closing quote
            if (!state.empty() && state.top() == kInString) {
                state.pop();
                valueIncoming = !valueIncoming;

                // If key and value have been captured
                if (!valueIncoming) {
                    // Create child to save ky-value-pair
                    std::shared_ptr<Components> child = std::shared_ptr<Components>(new Components(Components::kString, currentComponent.get(), key));
                    child->setValue(currentString);
                }
                key = currentString;
                currentString = "";
            }
            else {
                state.push(kInString);
            }
            break;
        }
    }
    return currentComponent;
}
