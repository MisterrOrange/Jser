#include "jsonprocessor.h"

#include <stack>


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
        char character = getCharacter(it);

        if (!state.empty()) {
            if (state.top() == kInString && character != '"') {
                // Handle escaped characters
                if (character == '\\') {
                    char nextCharacter = getCharacter(it+1);
                    switch (nextCharacter) {
                    case 'n': currentString += '\n'; break;
                    case 't': currentString += '\t'; break;
                    case '\'': currentString += '\''; break;
                    case '"': currentString += '"'; break;
                    case '\\': currentString += '\\'; break;
                    case 'r': currentString += '\r'; break;
                    case 'b': currentString += '\b'; break;
                    case 'f': currentString += '\f'; break;
                    default:
                        throw std::invalid_argument("Unknown escape character");
                    }
                    it++;
                    continue;
                }
                currentString += character;
                continue;
            }
            else if (state.top() == kInNumber || state.top() == kInFloat) {
                // Number has ended
                if (std::isdigit(character)) {
                    // Capture number
                    currentString += character;
                    continue;
                }
                // If it's a float
                if (character == '.') {
                    // Replace number with float
                    state.pop();
                    state.push(kInFloat);
                    currentString += '.';
                    continue;
                }
                else {
                    Components::ValueTypes valueType = state.top() == kInNumber ? Components::ValueTypes::kNumber : Components::ValueTypes::kFloat;
                    state.pop();
                    if (state.top() == kInDictionary) {
                        if (valueIncoming) {
                            std::shared_ptr<Components> child = std::make_shared<Components>(Components::kNone, currentComponent, key, valueType);
                            child->setValue(valueType, currentString);
                            currentComponent->addChild(child);
                        }
                        else {
                            key = currentString;
                        }

                        valueIncoming = !valueIncoming;
                    }
                    else if (state.top() == kInArray) {
                        std::shared_ptr<Components> child = std::make_shared<Components>(Components::kNone, currentComponent, arrayIndex);
                        arrayIndex++;
                        child->setValue(valueType, currentString);
                        currentComponent->addChild(child);
                    }
                    currentString = "";
                }
                continue;
            }
        }

        if (std::isdigit(character)) {
            state.push(kInNumber);
            currentString += character;
            continue;
        }

        switch (character) {
        // Handles boolean false
        case 'f':
            if (getCharacter(it+1) == 'a' && getCharacter(it+2) == 'l' && getCharacter(it+3) == 's' && getCharacter(it+4) == 'e') {
                it += 4;
                if (valueIncoming || state.top() == kInArray) {
                    std::shared_ptr<Components> child;
                    if (state.top() == kInDictionary) {
                         child = std::make_shared<Components>(Components::kNone, currentComponent, key);
                        valueIncoming = false;
                    }
                    else if (state.top() == kInArray) {
                        child = std::make_shared<Components>(Components::kNone, currentComponent, arrayIndex);
                        arrayIndex++;
                    }
                    else {
                        throw std::invalid_argument("Invalid state");
                    }
                    child->setValue(Components::kBoolean, "false");
                    currentComponent->addChild(child);
                }
                else {
                    key = "false";
                    valueIncoming = true;
                }
            }
            break;

        // Handles boolean true
        case 't':
            if (getCharacter(it+1) == 'r' && getCharacter(it+2) == 'u' && getCharacter(it+3) == 'e') {
                it += 3;
                if (valueIncoming || state.top() == kInArray) {
                    std::shared_ptr<Components> child;
                    if (state.top() == kInDictionary) {
                        child = std::make_shared<Components>(Components::kNone, currentComponent, key);
                        valueIncoming = false;
                    }
                    else if (state.top() == kInArray) {
                        child = std::make_shared<Components>(Components::kNone, currentComponent, arrayIndex);
                        arrayIndex++;
                    }
                    else {
                        throw std::invalid_argument("Invalid state");
                    }
                    child->setValue(Components::kBoolean, "true");
                    currentComponent->addChild(child);
                }
                else {
                    key = "true";
                    valueIncoming = true;
                }
            }
            break;

        // Handles null
        case 'n':
            if (getCharacter(it+1) == 'u' && getCharacter(it+2) == 'l' && getCharacter(it+3) == 'l') {
                it += 3;
                std::shared_ptr<Components> child;

                if (state.top() == kInDictionary) {
                    if (!valueIncoming) {
                        throw std::invalid_argument("Null type must not be a key");
                    }
                    child = std::make_shared<Components>(Components::kNone, currentComponent, key);
                    valueIncoming = false;
                }
                else if (state.top() == kInArray) {
                    child = std::make_shared<Components>(Components::kNone, currentComponent, arrayIndex);
                    arrayIndex++;
                }
                else {
                    throw std::invalid_argument("Invalid state");
                }
                child->setValue(Components::kNull, "null");
                currentComponent->addChild(child);
            }
            break;

        case '{':
            // Generate child
            if (state.empty()) {
                currentComponent = std::make_shared<Components>(Components::kDictionary);
                m_model = std::unique_ptr<JsonModel>(new JsonModel(currentComponent));
            }
            else {
                if (state.top() == kInDictionary)
                    currentComponent = std::make_shared<Components>(Components::kDictionary, currentComponent, key);
                else
                    currentComponent = std::make_shared<Components>(Components::kDictionary, currentComponent, arrayIndex);
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
            if (state.top() != kInArray) {
                throw std::invalid_argument("Expected array closure");
            }
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
                        std::shared_ptr<Components> child = std::make_shared<Components>(Components::kNone, currentComponent, key);
                        child->setValue(Components::kString, currentString);
                        currentComponent->addChild(child);
                    }
                }
                else if (state.top() == kInArray) {
                    std::shared_ptr<Components> child = std::make_shared<Components>(Components::kNone, currentComponent, arrayIndex);
                    arrayIndex++;
                    child->setValue(Components::kString, currentString);
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

char JsonProcessor::getCharacter(int index) {
    if (index > mmap.size())
        throw new std::invalid_argument("File index out of range");
    return mmap[index];
}
