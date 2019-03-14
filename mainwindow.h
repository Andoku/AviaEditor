#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QDataWidgetMapper>
#include <QStringListModel>

#include "model.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openModel();
    void openObjects();
    void changeActiveModel(QString name);
    void updateButtons(int row);

private:
    Ui::MainWindow *ui;
    QDataWidgetMapper *mapper;

    QString modelFilename;
    QString objectsFilename;

    QMap<QString, Model *> models;
    QMap<QString, QStringListModel *> enumModels;
    QString currentModel;

    void setupModels(const QJsonObject &model, const QJsonObject &objects);
    bool setupModelsProperties(const QJsonObject &model);
    bool setupModelsObjects(const QJsonObject &objects);
    bool setupTypeModels(const QJsonObject &model);
    void clearLayout(QLayout *layout);
};

#endif // MAINWINDOW_H
