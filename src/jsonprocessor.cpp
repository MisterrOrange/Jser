#include "jsonprocessor.h"
#include "Objects/components.h"

#include <stack>
#include <ctime>
#include <QThread>


JsonProcessor::JsonProcessor(std::string path, boolean parse) {
    m_filePath = path;

    if (parse)
        ParseJson();
}

void JsonProcessor::ParseJson(int startIndex) {
    clock_t startTime = clock();
    m_successfullyParsed = false;

    std::shared_ptr<Components> currentComponent = nullptr;

    if (!setMap()) {
        return;
    }

    std::stack<Status> state;
    std::wstring key;
    Components::ValueTypes keyType;
    std::wstring currentString;
    int arrayIndex = 0;
    // Stores whether next item is value or not
    bool valueIncoming = false;

    // Create stack of 100 int's that indicate when next percantage is reached
    std::stack<long> percentages;
    // +1 ensures it's not 0
    long stepSize = std::ceil(m_map.size() / 100) + 1;
    int currentPercentage = 0;
    for (long i = m_map.size(); i - stepSize > 0; i -= stepSize) {
        percentages.push(i);
    }

    for (long it = startIndex; it < m_map.size(); ++it) {
        if (it > percentages.top()) {
            percentages.pop();
            emit progressMade(++currentPercentage);
            // To not check too often
            if (QThread::currentThread()->isInterruptionRequested()) {
                m_model.reset();
                m_errorMessage = "Parsing aborted";
                return;
            }
        }
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
                    case '/': currentString += '/'; break; // idk who even escapes '/'
                    case 'u': {
                        std::string hexNumber = std::string(1, getCharacter(it+2)) + getCharacter(it+3)+ getCharacter(it+4)+ getCharacter(it+5);
                        uint decimalNumber = std::stoul(hexNumber, nullptr, 16);
                        currentString += static_cast<wchar_t>(decimalNumber);
                        break;
                    }
                    default:
                        logError("Unknown escape character", it, 0, 1);
                        return;
                    }
                    it++;
                    continue;
                }
                currentString += character;
                continue;
            }
            else if (state.top() == kInNumber || state.top() == kInFloat) {
                // Still in number (or float)
                if (std::isdigit(character) || character == '-' || character == '+') {
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
                else if (character == 'e' || character == 'E') {
                    currentString += 'e';
                    continue;
                }
                else {
                    Components::ValueTypes valueType = state.top() == kInNumber ? Components::ValueTypes::kNumber : Components::ValueTypes::kFloat;
                    state.pop();
                    if (state.top() == kInDictionary) {
                        if (valueIncoming) {
                            std::shared_ptr<Components> child = std::make_shared<Components>(Components::kNone, currentComponent, key, keyType);
                            child->setValue(valueType, currentString);
                            currentComponent->addChild(child);
                        }
                        else {
                            key = currentString;
                            keyType = valueType;
                        }

                        valueIncoming = !valueIncoming;
                    }
                    else if (state.top() == kInArray) {
                        std::shared_ptr<Components> child = std::make_shared<Components>(Components::kNone, currentComponent, arrayIndex);
                        arrayIndex++;
                        child->setValue(valueType, currentString);
                        currentComponent->addChild(child);
                    }
                    currentString = L"";
                }
            }
        }

        if (std::isdigit(character) || character == '-' || character == '+') {
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
                        // Shouldn't happen
                        logError("Invalid state", it, 10, 2);
                        return;
                    }
                    child->setValue(Components::kBoolean, L"false");
                    currentComponent->addChild(child);
                }
                else {
                    key = L"false";
                    keyType = Components::kBoolean;
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
                        // Shouldn't happen too
                        logError("Invalid state", it, 10, 2);
                        return;
                    }
                    child->setValue(Components::kBoolean, L"true");
                    currentComponent->addChild(child);
                }
                else {
                    key = L"true";
                    keyType = Components::kBoolean;
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
                        logError("Null type must not be a key", it, 20, 2);
                        return;
                    }
                    child = std::make_shared<Components>(Components::kNone, currentComponent, key);
                    valueIncoming = false;
                }
                else if (state.top() == kInArray) {
                    child = std::make_shared<Components>(Components::kNone, currentComponent, arrayIndex);
                    arrayIndex++;
                }
                else {
                    logError("Invalid state", it, 20, 5);
                    return;
                }
                child->setValue(Components::kNull, L"null");
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

            if (currentComponent == nullptr) {
                throw new std::invalid_argument("Component pointer is null");
                return;
            }
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
                else {
                    logError("State is invalid", it, 20, 2);
                    return;
                }
                currentComponent->parent()->addChild(currentComponent);
            }
            arrayIndex = 0;
            state.push(kInArray);
            break;

        case ']':
            if (state.top() != kInArray) {
                logError("Expected array closure", it, 20,0);
                return;
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
                        std::shared_ptr<Components> child = std::make_shared<Components>(Components::kNone, currentComponent, key, keyType);
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
                keyType = Components::kString;
                currentString = L"";
            }
            else {
                state.push(kInString);
            }
            break;
        }
    }
    deleteMap();
    m_successfullyParsed = true;
    m_parseTime = std::round((clock() - startTime) * 1000 / CLOCKS_PER_SEC);
    return;
}

void JsonProcessor::Parse() {
    ParseJson();
    emit parsingComplete();
    return;
}

JsonModel* JsonProcessor::getModel() {
    return m_model.get();
}

char JsonProcessor::getCharacter(int index) {
    if (index > m_map.size())
        throw new std::invalid_argument("File index out of range");
    return m_map[index];
}

boolean JsonProcessor::wasSuccessfullyParsed() {
    return m_successfullyParsed;
}

std::string JsonProcessor::getErrorMessage() {
    if (m_successfullyParsed)
        return "";
    return m_errorMessage;
}


void JsonProcessor::logError(std::string message, int position, int captureBefore, int captureAfter) {
    std::string capturedString;
    // Captures last 5 characters
    for (int i = position - captureBefore; i != position + captureAfter + 1; i++) {
        if (i < 0 || i > m_map.size())
            continue;
        capturedString += getCharacter(i);
    }
    m_errorMessage = "Encountered an error while parsing the json file at position " + std::to_string(position) + "\nError message: " + message + "\nJson string: " + capturedString;
    return;
}

boolean JsonProcessor::setMap() {
    if (m_filePath == "") {
        m_errorMessage = "No file path";
        return false;
    }
    std::error_code error;
    m_map = mio::make_mmap_source(m_filePath, error);
    if (!error) {
        return true;
    }
    m_errorMessage = "Unable to map file";
    return false;
}

void JsonProcessor::deleteMap() {
    m_map.unmap();
}
