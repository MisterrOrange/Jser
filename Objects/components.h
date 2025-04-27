#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <string>
#include <memory>

class Components
{
public:
    enum Types {
        kString,
        knumber,
        kDictionary,
        kArray
    };

    Components(Types type, Components *parent = nullptr, std::string name = "root");
    void setValue(std::string value);

    void addChild(std::shared_ptr<Components> child);
    Components *child(int row) const;
    Components *parent() const;
    int childCount() const;
    int columnCount() const;

private:
    Components *m_parent;
    enum Types m_type;

    std::string m_name; // "key" in dictionary terms
    std::string m_value;
    bool m_isValuePresent;

    std::vector<std::shared_ptr<Components>> m_children;
};

#endif
