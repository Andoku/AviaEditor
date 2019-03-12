#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>

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

    void setupModel(const QJsonObject &model, const QJsonObject &objects);
};

#endif // MAINWINDOW_H
