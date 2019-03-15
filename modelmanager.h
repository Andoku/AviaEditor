#ifndef MODELPARSER_H
#define MODELPARSER_H

#include <QMap>
#include <QJsonObject>
#include <QStringListModel>

#include "model.h"

class ModelManager
{
public:
    ModelManager();

    bool parse(const QJsonObject &model, const QJsonObject &objects);

    Model *getCurrentModel();
    QStringListModel *getEnumModel(Model *model, int column);
    void setCurrentModel(QString model);
    QStringList getModelNames();

private:
    QMap<QString, Model *> models;
    QMap<QString, QStringListModel *> enumModels;
    QString currentModel;

    bool parseModelsProperties(const QJsonObject &model);
    bool parseModelsObjects(const QJsonObject &objects);
    bool parseTypeModels(const QJsonObject &model);
};

#endif // MODELPARSER_H
