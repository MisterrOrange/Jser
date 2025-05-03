#include "components.h"
#include <stdexcept>
#include <QString>

Components::Components(StorageTypes type) {
    m_generalType = type;
    m_name = "root";
    m_isValuePresent = false;
}

Components::Components(StorageTypes type, std::shared_ptr<Components> parent, std::string name, ValueTypes nameType) {
    m_generalType = type;
    m_nameType = nameType;
    m_parent = parent; // Pointing parent to child inside here doesn't work (for some reason), so it has to be done outside

    m_name = QVariant(QString::fromStdString(name));;
    m_isValuePresent = false;
}


Components::Components(StorageTypes type, std::shared_ptr<Components> parent, int index)
    : Components(type, parent, "[" + std::to_string(index) + "]") {
    m_nameType = kNumber;
}

Components::Components(StorageTypes type, std::shared_ptr<Components> parent, bool value)
    : Components(type, parent, value ? std::string("true"): std::string("false")) {
    m_nameType = kBoolean;
}


void Components::setValue(ValueTypes valueType, std::string value) {
    if (m_children.size() != 0)
        throw std::invalid_argument("Value can't be assigned if children are present");

    m_valueType = valueType;
    m_value = QVariant(QString::fromStdString(value));
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
