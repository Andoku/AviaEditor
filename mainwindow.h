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
    QString modelFilename;
    QString objectsFilename;

    QMap<QString, Model *> models;
    QMap<QString, QStringListModel *> enumModels;
    QString currentModel;

    QDataWidgetMapper *mapper;

    void setupModel(const QJsonObject &model, const QJsonObject &objects);
    void setupTypesModel(const QJsonObject &model);
    void clearLayout(QLayout *layout);
};

#endif // MAINWINDOW_H
