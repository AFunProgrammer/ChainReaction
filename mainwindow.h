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

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::MainWindow *ui;
    QTimer RenderTimer;

    void CreateDots(uint Count, QSize Bounds, uint Size);
};
#endif // MAINWINDOW_H
