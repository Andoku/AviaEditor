#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDataWidgetMapper>

#include "model.h"
#include "modelmanager.h"

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

    ModelManager modelManager;

    void clearLayout(QLayout *layout);
};

#endif // MAINWINDOW_H
