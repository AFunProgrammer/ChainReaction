#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include <QtSvg/QtSvg>

#include "cdotsmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QPixmap _circle;
    CDotsManager _DotsManager;
    QTimer RenderTimer;

public slots:
    void CreateDots(uint Count = 50, uint Size = 6);
};
#endif // MAINWINDOW_H
