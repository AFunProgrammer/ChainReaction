#ifndef CDOTSMANAGER_H
#define CDOTSMANAGER_H

#pragma once
#include <QPixmap>
#include <QtSvg/QtSvg>
#include <QPoint>
#include <QObject>
#include <QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "cdot.h"

class CDotsManager : public QOpenGLWidget
{
    Q_OBJECT

public:
    enum class Direction
    {
        Positive = 0x0,
        Negative,
        None
    };

//single class needs
private:
    QString m_SvgFilePath;
    QString m_SvgFile;
    QSvgRenderer m_SvgRenderer;

    uint m_Collisions = 0;
    uint m_LastCollisionCount = 0;

    uint m_UpdateDotsSize = 0;

    QColor m_ClearColor = Qt::black;

private:
    QList<CDot*> m_Dots;
    QList<CDot*> m_RemovedDots;

    std::map<eColor, QPixmap> m_DotPixmaps;

    static bool m_ClickLock;

    QPixmap getSVGPixmap(eColor Color, QPointF Scale, uint Size);
    void createDotPixmaps();

    void setDotPixmap(CDot* Dot);

    void setDotDrawBoundary(CDot* Dot);

    void setDotSize(CDot* Dot);

    void drawDots(QPainter* Painter);
    void checkForCollisions(QList<CDot*> resizingDots);

public:
    explicit CDotsManager(QWidget *parent = nullptr);
    ~CDotsManager(){}//{if(m_SvgRenderer!=nullptr){delete m_SvgRenderer;}}

    void setSVGFile(QString SvgFilePath);
    QString getSVGFilePath();

    QList<CDot*> getDots();
    void addDot(CDot* Dot);
    void delDot(int Index);
    uint getDotCount();
    void clearDots();

    void setDotsSize(uint Size);

    void updateDots();

    void setBounds(QRect Rect);

    uint getCollisionCount();
    uint getLastCollisionCount();

    void setClearColor(QColor Color);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
signals:

};

#endif // CDOTSDRAWER_H
