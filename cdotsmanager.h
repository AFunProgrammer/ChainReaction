#ifndef CDOTSMANAGER_H
#define CDOTSMANAGER_H

#pragma once
#include <QPixmap>
#include <QtSvg/QtSvg>
#include <QPoint>
#include <QObject>
#include <QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "tdot.h"
#include "cspatialmanager.h"


class CDotsManager : public QOpenGLWidget
{
    Q_OBJECT

//single class needs
private:
    uint m_Collisions = 0;
    uint m_LastCollisionCount = 0;

    uint m_UpdateDotsSize = 0;

    QColor m_ClearColor = Qt::black;

    QRect m_Rect = QRect(0,0,0,0);

    QPixmap m_DrawBuffer;

private:
    QList<PTDot> m_Dots;
    QList<PTDot> m_RemovedDots;

    CSpatialManager m_Manager;

    std::map<eColor, QPixmap> m_DotPixmaps;

    void createDotPixmaps();

    void setDotPixmap(PTDot Dot);

    void setDotsDrawBoundary();

    void setDotSize(PTDot Dot);

    void drawDots(QPainter* Painter);
    void checkForCollisions(QList<PTDot> resizingDots);

public:
    explicit CDotsManager(QWidget *parent = nullptr);
    ~CDotsManager(){}

    QList<PTDot> getDots();
    void addDot(PTDot Dot);
    void delDot(int Index);
    uint getDotCount();
    void clearDots();

    void setDotsSize(uint Size);

    void updateDots();

    uint getCollisionCount();
    uint getLastCollisionCount();

    void setClearColor(QColor Color);

    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
signals:

};

#endif // CDOTSDRAWER_H
