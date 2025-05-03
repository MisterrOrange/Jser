#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <string>
#include <memory>
#include <QVariant>

class Components
{
public:
    enum ValueTypes {
        kString,
        kNumber,
        kFloat,
        kBoolean,
        kNull
    };
    enum StorageTypes {
        kDictionary,
        kArray,
        kNone
    };

    Components(StorageTypes type);
    explicit Components(StorageTypes type, std::shared_ptr<Components> parent, std::string name, ValueTypes nameType = kString);
    explicit Components(StorageTypes type, std::shared_ptr<Components> parent, int index);
    explicit Components(StorageTypes type, std::shared_ptr<Components> parent, bool value);
    void setValue(ValueTypes valueType, std::string value);

    void addChild(std::shared_ptr<Components> child); // Adds a child to `this`
    std::shared_ptr<Components> child(int row) const;
    std::shared_ptr<Components> parent() const;
    int childCount() const;
    int columnCount() const;
    int row() const;
    QVariant data(int column) const;

private:
    std::weak_ptr<Components> m_parent;

    // General type of object
    enum StorageTypes m_generalType;

    // Types of name and value
    enum ValueTypes m_nameType;
    enum ValueTypes m_valueType;

    QVariant m_name; // "key" in dictionary terms
    QVariant m_value;
    bool m_isValuePresent;

    std::vector<std::shared_ptr<Components>> m_children;
};

#endif
