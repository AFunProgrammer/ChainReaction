#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include <QtSvg/QtSvg>

#include "cdotsmanager.h"
#include "cclickcap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


    void sizeGUIElements();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QPixmap _circle;
    CDotsManager _DotsManager;
    //QList<CDot*> _ListOfDots;
    //CClickCap _ClickCap;
    QTimer RenderTimer;

    void resizeEvent(QResizeEvent* event);

public slots:
    void CreateDots(uint Count = 50, uint Size = 6);
};
#endif // MAINWINDOW_H
