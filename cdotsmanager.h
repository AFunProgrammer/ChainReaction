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
#include "cspacialmanager.h"


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

    QRect m_Rect = QRect(0,0,0,0);

private:
    QList<PTDot> m_Dots;
    QList<PTDot> m_RemovedDots;

    CSpacialManager m_Manager;

    std::map<eColor, QPixmap> m_DotPixmaps;

    static bool m_ClickLock;

    QPixmap getSVGPixmap(eColor Color, uint Size);
    void createDotPixmaps();

    void setDotPixmap(PTDot Dot);

    void setDotsDrawBoundary();

    void setDotSize(PTDot Dot);

    void drawDots(QPainter* Painter);
    void checkForCollisions(QList<PTDot> resizingDots);

public:
    explicit CDotsManager(QWidget *parent = nullptr);
    ~CDotsManager(){}//{if(m_SvgRenderer!=nullptr){delete m_SvgRenderer;}}

    void setSVGFile(QString SvgFilePath);
    QString getSVGFilePath();

    QList<PTDot> getDots();
    void addDot(PTDot Dot);
    void delDot(int Index);
    uint getDotCount();
    void clearDots();

    void setDotsSize(uint Size);

    void updateDots();

    void setBounds(QSize size);

    uint getCollisionCount();
    uint getLastCollisionCount();

    void setClearColor(QColor Color);

    void resizeEvent(QResizeEvent *event) override;
    //void setGeometry(const QRect &r);
    //QRect geometry();

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
signals:

};

#endif // CDOTSDRAWER_H
