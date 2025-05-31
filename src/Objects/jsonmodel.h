 #ifndef JSONMODEL_H
#define JSONMODEL_H

#include <QVariant>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QBrush>

class Components;
class JsonModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    Q_DISABLE_COPY_MOVE(JsonModel);

    JsonModel(std::shared_ptr<Components> rootElement);

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool isRealData(const QModelIndex &index) const;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

private:
    std::shared_ptr<Components> m_rootElement;

    QBrush generateBrush(std::string colourCode) const;
    int convertHexToDecimal(std::string hex) const;
};

#endif // JSONMODEL_H
