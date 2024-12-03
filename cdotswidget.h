#ifndef CDOTSWIDGET_H
#define CDOTSWIDGET_H

#pragma once
#include <QPixmap>
#include <QtSvg/QtSvg>
#include <QPoint>
#include <QObject>
#include <QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "cdotsmanager.h"


class CDotsWidget : public QOpenGLWidget
{
public:
    explicit CDotsWidget(QWidget *parent = nullptr);
    ~CDotsWidget(){}

private:
    QColor m_ClearColor = Qt::black;
    QRect m_Rect = QRect(0,0,0,0);

    CDotsManager* m_pDotsMgr = CDotsManager::getGlobalInstance();
public:
    void setClearColor(QColor Color);

    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
signals:
};


#endif // CDOTSWIDGET_H
