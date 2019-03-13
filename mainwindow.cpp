#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>

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
    connect(ui->classType, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeModel(QString)));

    mapper = new QDataWidgetMapper(this);
    connect(ui->previousButton, &QAbstractButton::clicked, mapper, &QDataWidgetMapper::toPrevious);
    connect(ui->nextButton, &QAbstractButton::clicked, mapper, &QDataWidgetMapper::toNext);
    connect(mapper, &QDataWidgetMapper::currentIndexChanged, this, &MainWindow::updateButtons);

    ui->openObjectsButton->setVisible(false);
    ui->objectsFile->setVisible(false);
    ui->classType->setVisible(false);
    ui->classLabel->setVisible(false);
    ui->previousButton->setVisible(false);
    ui->nextButton->setVisible(false);
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
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
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
    setupModel(modelDoc.object(), objectsDoc.object());

    ui->classType->setVisible(true);
    ui->classLabel->setVisible(true);
}

void MainWindow::changeModel(QString name)
{
    currentModel = name;
    mapper->setModel(models[name]);
    clearLayout(ui->layout);

    for(int i = 0; i < models[name]->columnCount(); ++i) {
        QString labelName = models[name]->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        QLabel *nameLabel = new QLabel(labelName + ":");
        QLineEdit *nameEdit = new QLineEdit();

        ui->layout->addWidget(nameLabel, i, 0, Qt::AlignTop);
        ui->layout->addWidget(nameEdit, i, 1, Qt::AlignTop);
        mapper->addMapping(nameEdit, i);
    }

    ui->previousButton->setVisible(models[name]->rowCount() > 1);
    ui->nextButton->setVisible(models[name]->rowCount() > 1);
    mapper->toFirst();
}

void MainWindow::setupModel(const QJsonObject &model, const QJsonObject &objects)
{
    if(!model.contains("classes") || !model["classes"].isArray()) {
        qWarning("Incorrect model file");
        return;
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

    if(!objects.contains("features") || !objects["features"].isObject()) {
        qWarning("Incorrect objects file");
        return;
    }

    const QJsonObject featuresObject = objects["features"].toObject();
    if(!featuresObject.contains("feature") || !featuresObject["feature"].isArray()) {
        qWarning("Incorrect objects file");
        return;
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
}
