#include "components.h"
#include <stdexcept>
#include <QString>

Components::Components(StorageTypes type) {
    m_generalType = type;
    m_name = "root";
    m_isValuePresent = false;
}

Components::Components(StorageTypes type, std::shared_ptr<Components> parent, std::wstring name, ValueTypes nameType) {
    m_generalType = type;
    m_nameType = nameType;
    m_parent = parent; // Pointing parent to child inside here doesn't work (for some reason), so it has to be done outside

    m_name = QVariant(QString::fromStdWString(name));;
    m_isValuePresent = false;
}


Components::Components(StorageTypes type, std::shared_ptr<Components> parent, int index)
    : Components(type, parent, L"[" + std::to_wstring(index) + L"]") {
    m_nameType = kArrayIndex;
}

Components::Components(StorageTypes type, std::shared_ptr<Components> parent, bool value)
    : Components(type, parent, value ? std::wstring(L"true"): std::wstring(L"false")) {
    m_nameType = kBoolean;
}


void Components::setValue(ValueTypes valueType, std::wstring value) {
    if (m_children.size() != 0)
        throw std::invalid_argument("Value can't be assigned if children are present");

    m_valueType = valueType;
    m_value = QVariant(QString::fromStdWString(value));
    m_isValuePresent = true;
}

void Components::addChild(std::shared_ptr<Components> child) {
    if (m_isValuePresent)
        throw std::invalid_argument("Can't add child if value is present");
    m_children.push_back(child);
    return;
}


std::shared_ptr<Components> Components::child(int row) const {
    if (row >= m_children.size())
        return nullptr;

    return m_children[row];
}

std::shared_ptr<Components> Components::parent() const {
    if (m_parent.expired()) {
        throw new std::invalid_argument("Parent pointer doesn't exist");
    }
    return m_parent.lock();
}

int Components::childCount() const {
    return m_children.size();
}

int Components::columnCount() const {
    // Apparently this goes for all children
    return 2;
}

QVariant Components::data(int column) const {
    if (column == 0)
        return m_name;
    if (column == 1 && m_isValuePresent)
        return m_value;
    return QVariant();
}

int Components::row() const {
    if (m_parent.expired())
        return 0;
    Components *parent = this->parent().get();
    for (int i = 0; i < parent->childCount(); ++i) {
        Components *child = parent->m_children[i].get();
        if (child == this)
            return i;
    }
    // Shouldn't happen
    return 0;
}


bool Components::isValuePresent() const {
    return m_isValuePresent;
}

Components::StorageTypes Components::getGeneralType() const {
    return m_generalType;
}

Components::ValueTypes Components::getNameType() const {
    return m_nameType;
}

Components::ValueTypes Components::getValueType() const {
    return m_valueType;
}

QString Components::convertValueTypeToString(ValueTypes type) const {
    switch (type) {
    case kString:
        return QString("String");
    case kNumber:
        return QString("Integer");
    case kFloat:
        return QString("Float");
    case kBoolean:
        return QString("Boolean");
    case kNull:
        return QString("Null");
    case kArrayIndex:
        return QString("ArrayIndex");
    }
    return QString();
}
QString Components::convertStorageTypeToString(StorageTypes type) const {
    switch (type) {
    case kDictionary:
        return QString("Dictionary");
    case kArray:
        return QString("Array");
    case kNone:
        return QString("None");
    }
    return QString();
}
