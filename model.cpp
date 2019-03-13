#include "model.h"

Model::Model(const QMap<QString, QString> &properties, QObject *parent)
    : QAbstractTableModel(parent),
      properties(properties)
{}

int Model::rowCount(const QModelIndex & /*parent*/) const
{
   return objectsData.size();
}

int Model::columnCount(const QModelIndex & /*parent*/) const
{
    return properties.size();
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return objectsData[index.row()][(properties.begin() + index.column()).key()];
    }

    return QVariant();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if(section < properties.size()) {
            return (properties.begin() + section).key();
        }
    }
    return QVariant();
}

bool Model::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole) {
        objectsData[index.row()][(properties.begin() + index.column()).key()] = value.toString();
    }
    return true;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void Model::addData(const QMap<QString, QString> &newData)
{
    objectsData.append(newData);
}
