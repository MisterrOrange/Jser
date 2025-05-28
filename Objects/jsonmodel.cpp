#include "jsonmodel.h"
#include "components.h"
#include <QFont>
#include <QColor>
#include <QSettings>

JsonModel::JsonModel(std::shared_ptr<Components> rootElement) {
    m_rootElement = rootElement;
}

QModelIndex JsonModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return {};

    Components *parentItem = parent.isValid() ? static_cast<Components*>(parent.internalPointer()) : m_rootElement.get();

    Components *childItem = parentItem->child(row).get();
    if (childItem == nullptr)
        return {};
    return createIndex(row, column, childItem);
}

QVariant JsonModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    QSettings settings;
    const Components *item = static_cast<const Components*>(index.internalPointer());
    switch (role) {
    // Returns the actual data
    case Qt::DisplayRole: {
        if (item->isValuePresent() || index.column() == 0) {
            // Return data normally
            return item->data(index.column());
        }
        if (!settings.value("showStorageName", true).toBool())
            return {};
        // Return type of storage for column 1
        return item->convertStorageTypeToString(item->getGeneralType());
    }
    // What's being displayed as tooltips
    case Qt::ToolTipRole: {
        // Return normal data for everything besides column that indicates type
        if (!item->isValuePresent() && index.column() == 1)
            return {};
        return this->data(index, Qt::DisplayRole);
    }

    case Qt::FontRole: {
        QFont font;
        if (!item->isValuePresent() && index.column() == 1)
            font.setItalic(true);
        return font;
    }

    case Qt::ForegroundRole: {
        std::string colour;
        if (index.column() == 1 && !item->isValuePresent()) {
            if (item->getGeneralType() == Components::StorageTypes::kArray)
                colour = settings.value("arrayColour", "D4D4D4").toString().toStdString();
            else
                colour = settings.value("dictionaryColour", "D4D4D4").toString().toStdString();
            return generateBrush(colour);
        }
        Components::ValueTypes type;
        if (index.column() == 0)
            type = item->getNameType();
        else
            type = item->getValueType();
        switch(type) {
        case Components::ValueTypes::kString: {
            colour = settings.value("stringColour", "CE9178").toString().toStdString();
            break;
        }
        case Components::ValueTypes::kNumber: {
            colour = settings.value("numberColour", "B5CEA8").toString().toStdString();
            break;
        }
        case Components::ValueTypes::kFloat: {
            colour = settings.value("floatColour", "B5CEA8").toString().toStdString();
            break;
        }
        case Components::ValueTypes::kBoolean: {
            colour = settings.value("booleanColour", "569CD6").toString().toStdString();
            break;
        }
        case Components::ValueTypes::kNull: {
            colour = settings.value("nullColour", "569CD6").toString().toStdString();
            break;
        }
        case Components::ValueTypes::kArrayIndex: {
            colour = settings.value("arrayIndexColour", "B5CEA8").toString().toStdString();
            break;
        }

        default:
            colour = "FFFFFF";
        }
        return generateBrush(colour);
    }

    default:
        return {};
    }
}

bool JsonModel::isRealData(const QModelIndex &index) const {
    const Components *item = static_cast<const Components*>(index.internalPointer());
    if (item->columnCount() == 0)
        return true;
    return item->isValuePresent();
}


int JsonModel::rowCount(const QModelIndex &parent) const {
    // Only column 0 has children
    if (parent.column() > 0)
        return 0;

    const Components *parentItem = parent.isValid() ? static_cast<const Components*>(parent.internalPointer()) : m_rootElement.get();
    return parentItem->childCount();
}

int JsonModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<Components*>(parent.internalPointer())->columnCount();
    return m_rootElement->columnCount();
}

QModelIndex JsonModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return {};

    Components *childItem = static_cast<Components*>(index.internalPointer());
    Components *parentItem = childItem->parent().get();

    return parentItem != m_rootElement.get() ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}

QBrush JsonModel::generateBrush(std::string colourCode) const {
    int red =   convertHexToDecimal(colourCode.substr(0, 2));
    int green = convertHexToDecimal(colourCode.substr(2, 2));
    int blue =  convertHexToDecimal(colourCode.substr(4, 2));
    QBrush brush(QColor(red, green, blue));
    return brush;
}

int JsonModel::convertHexToDecimal(std::string hex) const {
    return std::stoul(hex, nullptr, 16);
}
