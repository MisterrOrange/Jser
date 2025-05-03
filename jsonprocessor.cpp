#include "jsonprocessor.h"

#include <stack>

// TODO:
// - Add way to print Dictionary
// - Remove recursion from this (DONE)
// - Use smart pointers



JsonProcessor::JsonProcessor(std::string path) {
    std::error_code error;
    mmap = mio::make_mmap_source(path, error);

    ParseJson();
}

std::shared_ptr<Components> JsonProcessor::ParseJson(int startIndex) {
    std::shared_ptr<Components> currentComponent = nullptr;

    std::stack<Status> state;
    std::string key;
    std::string currentString;
    int arrayIndex = 0;
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
            if (state.empty()) {
                currentComponent = std::make_shared<Components>(Components::kDictionary);
                m_model = std::unique_ptr<JsonModel>(new JsonModel(currentComponent));
            }
            else {
                currentComponent = std::make_shared<Components>(Components::kDictionary, currentComponent, key);
                currentComponent->parent()->addChild(currentComponent);
            }
            state.push(kInDictionary);
            valueIncoming = false;
            break;

        case '}':
            state.pop();
            if (state.empty()) { continue; }

            if (currentComponent == nullptr) { throw std::invalid_argument("Component pointer is null"); }
            // Set currentComponent to it's parent
            currentComponent = currentComponent->parent();
            if (state.top() == kInDictionary)
                { valueIncoming = false; }
            else if (state.top() == kInArray)
                { arrayIndex = currentComponent->childCount(); }
            break;

        case '[':
            if (state.empty()) {
                currentComponent = std::make_shared<Components>(Components::kArray);
                m_model = std::make_unique<JsonModel>(currentComponent);
            }
            else {
                // If there's a dictionary before going into array now
                if (state.top() == Status::kInDictionary) {
                    currentComponent = std::make_shared<Components>(Components::kArray, currentComponent, key);
                }
                else if (state.top() == Status::kInArray){
                    currentComponent = std::make_shared<Components>(Components::kArray, currentComponent, arrayIndex);
                }
                else { throw new std::invalid_argument("State is invalid"); }
                currentComponent->parent()->addChild(currentComponent);
            }
            arrayIndex = 0;
            state.push(kInArray);
            break;

        case ']':
            if (state.top() != kInArray) { throw std::invalid_argument("Expected array closure"); }
            state.pop();
            if (state.empty()) { continue; }

            currentComponent = currentComponent->parent();
            if (state.top() == kInDictionary) {
                valueIncoming = false;
            }
            else if (state.top() == kInArray) {
                arrayIndex = currentComponent->childCount();
            }
            break;

        case '"':
            // If is closing quote
            if (!state.empty() && state.top() == kInString) {
                state.pop();
                valueIncoming = !valueIncoming;

                if (state.top() == kInDictionary) {
                    // If key and value have been captured
                    if (!valueIncoming) {
                        // Create child to save key-value-pair
                        std::shared_ptr<Components> child = std::make_shared<Components>(Components::kString, currentComponent, key);
                        child->setValue(currentString);
                        currentComponent->addChild(child);
                    }
                }
                else if (state.top() == kInArray) {
                    std::shared_ptr<Components> child = std::make_shared<Components>(Components::kString, currentComponent, arrayIndex);
                    arrayIndex++;
                    child->setValue(currentString);
                    currentComponent->addChild(child);
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

JsonModel* JsonProcessor::getModel() {
    return m_model.get();
}
