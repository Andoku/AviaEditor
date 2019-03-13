#ifndef MODEL_H
#define MODEL_H

#include <QAbstractTableModel>

class Model : public QAbstractTableModel
{
    Q_OBJECT
public:
    Model(const QMap<QString, QString> &properties, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addData(const QMap<QString, QString> &newData);
private:
    QMap<QString, QString> properties;
    QVector<QMap<QString, QString> > objectsData;
};

#endif // MODEL_H
