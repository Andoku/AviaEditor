#include <QFileDialog>
#include <QJsonDocument>
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
    ui->nextButton->setEnabled(row < modelManager.getCurrentModel()->rowCount() - 1);
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

    if(!modelManager.parse(modelDoc.object(), objectsDoc.object())) {
        return;
    }

    ui->classType->addItems(modelManager.getModelNames());
    ui->typeLabel->setVisible(true);
    ui->classType->setVisible(true);
    changeActiveModel(ui->classType->currentText());
}

void MainWindow::changeActiveModel(QString name)
{
    modelManager.setCurrentModel(name);
    Model *currentModel = modelManager.getCurrentModel();
    mapper->setModel(currentModel);
    clearLayout(ui->layout);

    for(int i = 0; i < currentModel->columnCount(); ++i) {
        QString labelName = currentModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        QLabel *nameLabel = new QLabel(labelName + ":");
        ui->layout->addWidget(nameLabel, i, 0);

        if(modelManager.getEnumModel(currentModel, i) != nullptr) {
            QComboBox *typeComboBox = new QComboBox();
            typeComboBox->setModel(modelManager.getEnumModel(currentModel, i));
            ui->layout->addWidget(typeComboBox, i, 1);
            mapper->addMapping(typeComboBox, i);
        } else {
            QLineEdit *nameEdit = new QLineEdit();
            ui->layout->addWidget(nameEdit, i, 1);
            mapper->addMapping(nameEdit, i);
        }
    }

    ui->previousButton->setVisible(currentModel->rowCount() > 1);
    ui->nextButton->setVisible(currentModel->rowCount() > 1);
    mapper->toFirst();
}
