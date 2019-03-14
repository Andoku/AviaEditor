#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QLabel>
#include <QComboBox>

#include <QTextEdit>
#include <QSpinBox>
#include <QPushButton>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->openModelButton, SIGNAL(released()), this, SLOT(openModel()));
    connect(ui->openObjectsButton, SIGNAL(released()), this, SLOT(openObjects()));
    connect(ui->classType, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeActiveModel(QString)));

    mapper = new QDataWidgetMapper(this);
    connect(ui->previousButton, &QAbstractButton::clicked, mapper, &QDataWidgetMapper::toPrevious);
    connect(ui->nextButton, &QAbstractButton::clicked, mapper, &QDataWidgetMapper::toNext);
    connect(mapper, &QDataWidgetMapper::currentIndexChanged, this, &MainWindow::updateButtons);

    ui->openObjectsButton->setVisible(false);
    ui->objectsFile->setVisible(false);
    ui->classType->setVisible(false);
    ui->previousButton->setVisible(false);
    ui->nextButton->setVisible(false);
    ui->typeLabel->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateButtons(int row)
{
    ui->previousButton->setEnabled(row > 0);
    ui->nextButton->setEnabled(row < models[currentModel]->rowCount() - 1);
}

void MainWindow::clearLayout(QLayout *layout)
{
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if(item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if(item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void MainWindow::openModel()
{
    modelFilename = QFileDialog::getOpenFileName(this,
        "Select a file to open...", QDir::homePath(), "Model File (*.json)");
    if(!modelFilename.isEmpty()) {
        ui->modelFile->setText(modelFilename);
        ui->openObjectsButton->setVisible(true);
        ui->objectsFile->setVisible(true);
    }
}

void MainWindow::openObjects()
{
    objectsFilename = QFileDialog::getOpenFileName(this,
        "Select a file to open...", QDir::homePath(), "Objects File (*.json)");
    ui->objectsFile->setText(objectsFilename);

    QFile modelFile(modelFilename);
    QFile objectsFile(objectsFilename);
    if (!modelFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open model file.");
        return;
    } else if (!objectsFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open objects file.");
        return;
    }

    const QByteArray modelData = modelFile.readAll();
    const QByteArray objectsData = objectsFile.readAll();
    const QJsonDocument modelDoc(QJsonDocument::fromJson(modelData));
    const QJsonDocument objectsDoc(QJsonDocument::fromJson(objectsData));
    setupModels(modelDoc.object(), objectsDoc.object());

    ui->typeLabel->setVisible(true);
    ui->classType->setVisible(true);
}

void MainWindow::changeActiveModel(QString name)
{
    currentModel = name;
    mapper->setModel(models[name]);
    clearLayout(ui->layout);

    for(int i = 0; i < models[name]->columnCount(); ++i) {
        QString labelName = models[name]->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        QLabel *nameLabel = new QLabel(labelName + ":");
        ui->layout->addWidget(nameLabel, i, 0);

        if(enumModels.count(models[name]->getType(i))) {
            QComboBox *typeComboBox = new QComboBox();
            typeComboBox->setModel(enumModels[models[name]->getType(i)]);
            ui->layout->addWidget(typeComboBox, i, 1);
            mapper->addMapping(typeComboBox, i);
        } else {
            QLineEdit *nameEdit = new QLineEdit();
            ui->layout->addWidget(nameEdit, i, 1);
            mapper->addMapping(nameEdit, i);
        }
    }

    ui->previousButton->setVisible(models[name]->rowCount() > 1);
    ui->nextButton->setVisible(models[name]->rowCount() > 1);
    mapper->toFirst();
}

void MainWindow::setupModels(const QJsonObject &model, const QJsonObject &objects)
{
    if(setupTypeModels(model) && setupModelsProperties(model) && setupModelsObjects(objects)) {
        changeActiveModel(ui->classType->currentText());
    }
}

bool MainWindow::setupTypeModels(const QJsonObject &model)
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
        if(typesArray[i].isString()){
            const QString typeName = typesArray[i].toString();
            if(enumerations.count(typeName)) {
                enumModels[typeName] = new QStringListModel(enumerations[typeName], this);
            }
        }
    }

    return true;
}

bool MainWindow::setupModelsProperties(const QJsonObject &model)
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
        ui->classType->addItem(className);
    }

    return true;
}

bool MainWindow::setupModelsObjects(const QJsonObject &objects)
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
            }
        }
        models[keys[0]]->addData(data);
    }
    return true;
}
