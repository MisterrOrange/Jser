 #ifndef JSONMODEL_H
#define JSONMODEL_H

#include <QVariant>
#include <QModelIndex>
#include <QAbstractItemModel>
#include "components.h"

class Components;
class JsonModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    JsonModel(std::unique_ptr<Components> &&rootElement);

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

private:
    std::unique_ptr<Components> m_rootElement;
};

#endif // JSONMODEL_H
