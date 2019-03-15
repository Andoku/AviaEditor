#include <QJsonArray>
#include <QStringListModel>

#include "modelmanager.h"

ModelManager::ModelManager()
{

}

Model *ModelManager::getCurrentModel()
{
    return models.count(currentModel) ? models[currentModel] : nullptr;
}

QStringListModel *ModelManager::getEnumModel(Model *model, int column)
{
    if(enumModels.count(model->getType(column))) {
        return enumModels[model->getType(column)];
    }

    return nullptr;
}

QStringList ModelManager::getModelNames()
{
    return models.keys();
}

void ModelManager::setCurrentModel(QString model)
{
    if(models.count(model)) {
        currentModel = model;
    }
}

bool ModelManager::parse(const QJsonObject &model, const QJsonObject &objects)
{
    return parseTypeModels(model) && parseModelsProperties(model) && parseModelsObjects(objects);
}

bool ModelManager::parseTypeModels(const QJsonObject &model)
{
    if(!model.contains("types") || !model["types"].isArray()) {
        qWarning("Incorrect model file");
        return false;
    }

    QMap<QString, QStringList> enumerations;
    if(model.contains("enumerations") && model["enumerations"].isArray()) {
        const QJsonArray enumerationsArray = model["enumerations"].toArray();
        for (int i = 0; i < enumerationsArray.size(); ++i) {
            const QJsonObject enumeration = enumerationsArray[i].toObject();
            const QString name = enumeration["name"].toString();
            const QJsonArray values = enumeration["values"].toArray();
            for (int j = 0; j < values.size(); ++j) {
                enumerations[name].append(values[j].toString());
            }
        }
    }

    const QJsonArray typesArray = model["types"].toArray();
    for (int i = 0; i < typesArray.size(); ++i) {
        if(typesArray[i].isString()) {
            const QString typeName = typesArray[i].toString();
            if(enumerations.count(typeName)) {
                enumModels[typeName] = new QStringListModel(enumerations[typeName]);
            }
        }
    }

    return true;
}

bool ModelManager::parseModelsProperties(const QJsonObject &model)
{
    if(!model.contains("classes") || !model["classes"].isArray()) {
        qWarning("Incorrect model file");
        return false;
    }

    const QJsonArray classesArray = model["classes"].toArray();
    for (int i = 0; i < classesArray.size(); ++i) {
        const QJsonObject classesObject = classesArray[i].toObject();
        if(!classesObject.contains("name") || !classesObject["name"].isString() ||
                !classesObject.contains("properties") || !classesObject["properties"].isArray()) {
            continue;
        }

        QMap<QString, QString> properties;
        const QJsonArray propertiesArray = classesObject["properties"].toArray();
        for (int propertiesIndex = 0; propertiesIndex < propertiesArray.size(); ++propertiesIndex) {
            const QJsonObject propertiesObject = propertiesArray[propertiesIndex].toObject();
            if(propertiesObject.contains("name") && propertiesObject["name"].isString() &&
                    propertiesObject.contains("type") && propertiesObject["type"].isString()) {
                const QString propertyName = propertiesObject["name"].toString();
                const QString propertyType = propertiesObject["type"].toString();
                properties[propertyName] = propertyType;
            }
        }
        const QString className = classesObject["name"].toString();
        models[className] = new Model(properties);
    }

    return true;
}

bool ModelManager::parseModelsObjects(const QJsonObject &objects)
{
    if(!objects.contains("features") || !objects["features"].isObject()) {
        qWarning("Incorrect objects file");
        return false;
    }

    const QJsonObject featuresObject = objects["features"].toObject();
    if(!featuresObject.contains("feature") || !featuresObject["feature"].isArray()) {
        qWarning("Incorrect objects file");
        return false;
    }

    const QJsonArray featureArray = featuresObject["feature"].toArray();
    for (int featureIndex = 0; featureIndex < featureArray.size(); ++featureIndex) {
        const QJsonObject featureObject = featureArray[featureIndex].toObject();
        const QStringList keys = featureObject.keys();
        if(keys.size() != 1 || !featureObject[keys[0]].isObject()) {
            continue;
        }
        const QJsonObject object = featureObject[keys[0]].toObject();
        const QStringList objectProperties = object.keys();
        QMap<QString, QString> data;
        for (int i = 0; i < objectProperties.size(); ++i) {
            if(object[objectProperties[i]].isString()) {
                data[objectProperties[i]] = object[objectProperties[i]].toString();
            } else if(object[objectProperties[i]].isObject()) {
                const QJsonObject property = object[objectProperties[i]].toObject();
                if(property.contains("lat_lon") && property["lat_lon"].isString()) {
                    data[objectProperties[i]] = property["lat_lon"].toString();
                } else if(property.contains("points") && property["points"].isObject()) {
                    const QJsonObject points = property["points"].toObject();
                    if(points.contains("lat_lon") && points["lat_lon"].isArray()) {
                        const QJsonArray pointsArray = points["lat_lon"].toArray();
                        for (int j = 0; j < pointsArray.size(); ++j) {
                            data[objectProperties[i]] += pointsArray[j].toString() + "; ";
                        }
                    }
                }
            }
        }
        models[keys[0]]->addData(data);
    }

    return true;
}
