#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <string>
#include <memory>
#include <QVariant>

class Components
{
public:
    enum Types {
        kString,
        knumber,
        kDictionary,
        kArray
    };

    Components(Types type);
    Components(Types type, std::shared_ptr<Components> parent, std::string name);
    void setValue(std::string value);

    void addChild(std::shared_ptr<Components> child);
    Components *child(int row) const;
    Components *parent() const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;

private:
    std::weak_ptr<Components> m_parent;
    enum Types m_type;

    QVariant m_name; // "key" in dictionary terms
    QVariant m_value;
    bool m_isValuePresent;

    std::vector<std::shared_ptr<Components>> m_children;
};

#endif
