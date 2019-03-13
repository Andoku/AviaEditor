#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>

#include <QDataWidgetMapper>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>

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
    void changeModel(QString name);

private:
    Ui::MainWindow *ui;
    QString modelFilename;
    QString objectsFilename;

    QMap<QString, Model *> models;
    QMap<QString, QGridLayout *> layouts;

    QDataWidgetMapper *mapper;

    void setupModel(const QJsonObject &model, const QJsonObject &objects);
    void clearLayout(QLayout *layout);
};

#endif // MAINWINDOW_H
