#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->openModelButton, SIGNAL(released()), this, SLOT(openModel()));
    connect(ui->openObjectsButton, SIGNAL(released()), this, SLOT(openObjects()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openModel()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
        "Select a file to open...", QDir::homePath(), "Model File (*.json)");
    ui->modelFile->setText(fileName);
}

void MainWindow::openObjects()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
        "Select a file to open...", QDir::homePath(), "Objects File (*.json)");
    ui->objectsFile->setText(fileName);
}
