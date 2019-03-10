#include "model.h"

Model::Model(int rows, int cols, QObject *parent)
    : QAbstractTableModel(parent),
      cols(cols),
      rows(rows)
{
    for (int i = 0; i < rows; i++) {
        m_gridData.append(QVector<QString>());
        for(int j = 0; j < cols; j++) {
            m_gridData[i].append("---");
        }
    }
}

int Model::rowCount(const QModelIndex & /*parent*/) const
{
   return rows;
}

int Model::columnCount(const QModelIndex & /*parent*/) const
{
    return cols;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_gridData[index.row()][index.column()];
    }

    return QVariant();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("first");
        case 1:
            return QString("second");
        case 2:
            return QString("third");
        }
    }
    return QVariant();
}

bool Model::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole) {
        m_gridData[index.row()][index.column()] = value.toString();
    }
    return true;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}
