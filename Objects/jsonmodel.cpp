#include "jsonmodel.h"
#include "components.h"

JsonModel::JsonModel(std::shared_ptr<Components> rootElement) {
    m_rootElement = rootElement;
}

QModelIndex JsonModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return {};

    Components *parentItem = parent.isValid() ? static_cast<Components*>(parent.internalPointer()) : m_rootElement.get();

    Components *childItem = parentItem->child(row);
    if (childItem == nullptr)
        return {};
    return createIndex(row, column, childItem);
}

QVariant JsonModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const Components *item = static_cast<const Components*>(index.internalPointer());
    return item->data(index.column());
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
    Components *parentItem = childItem->parent();

    return parentItem != m_rootElement.get() ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}
