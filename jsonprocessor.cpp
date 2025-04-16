#include "jsonprocessor.h"
#include "Objects/DataStructures/dictionary.h"
#include "Objects/DataTypes/jsonString.h"

#include <stack>


JsonProcessor::JsonProcessor(std::string path) {
    std::error_code error;
    mmap = mio::make_mmap_source(path, error);


    m_rootElement = HandleDictionary(0);
}

Dictionary* JsonProcessor::HandleDictionary(int startIndex) {
    Dictionary *dic = new Dictionary();

    std::stack<char> state;
    std::string key;
    std::string currentString;
    // Stores wheter next item is value or not
    bool valueIncoming = false;
    for (int it = startIndex; mmap[it] != '}'; ++it) {
        char character = mmap[it];

        if (!state.empty() && state.top() == '{') {
            if (character == '}') { state.pop(); }
            continue;
        }
        if (character == '"') {
            // If is closing quote
            if (!state.empty() && state.top() == '"') {
                state.pop();
                valueIncoming = !valueIncoming;

                // If key and value have been captured
                if (!valueIncoming) {
                    dic->AddValue(key, new JsonString(currentString));
                }
                key = currentString;
                currentString = "";
            }
            else {
                state.push('"');
            }
            continue;
        }

        if (valueIncoming && character == '{') {
            dic->AddValue(key, HandleDictionary(it));
            state.push('{');
            valueIncoming = false;
            continue;
        }

        if (state.empty())
            continue;

        if (state.top() == '"') {
            currentString += mmap[it];
            continue;
        }
    }
    return dic;
}
