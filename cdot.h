#ifndef CDOT_H
#define CDOT_H

#pragma once
#include <QPixmap>
#include <QPoint>
#include <QObject>

#include "cutility.h"

class CDot
{
public:
    enum class Movement
    {
        Positive = 0x0,
        Negative,
        None
    };

private:
    QPixmap* m_Pixmap = nullptr;

    eColor m_Color = eColor::red;

    int m_Size = 10;
    int m_Radius = 5;

    QRect m_Bounds = QRect(0,0,100,100);
    QPoint m_Pos;

    bool m_Resize = false;
    int  m_IncreasingSize = true;

    float m_ScaleMin = 0.01f;
    float m_ScaleMax = 3.0f;
    float m_Scale = 1.0f;

    bool m_Move = true;
    Movement m_XMovement = Movement::None;
    Movement m_YMovement = Movement::None;
    QPoint m_Speed = QPoint(6,6);

    bool m_Stopped = false;

    unsigned long long m_Id = 0;

    void updateMovement();
    void updateScaling();

public:
    explicit CDot();
    ~CDot(){}

    void setPixmap(QPixmap* Pixmap);
    QPixmap* getPixmap();

    void setColor(eColor Color);
    eColor getColor();

    void setPos(QPoint Pos);
    QPoint getPos();

    void setMovement(Movement X, Movement Y);
    Movement getXMovement();
    Movement getYMovement();

    void setSpeed(QPoint Speed);
    QPoint getSpeed();

    void setScaleMin(float Min);
    void setScaleMax(float Max);
    void setScale(float Scale);
    float getScaleMin();
    float getScaleMax();
    float getScale();

    void setSize(uint Size);
    uint getSize();

    bool isStopped();

    void startResizing();
    bool isResizing();

    void setBounds(QRect Rect);
    QRect getDrawRect();

    void setId(ulong Id);
    ulong getId();

    void update();

    CDot* operator= (CDot RHS);
    CDot* operator= (CDot* RHS);
};

#endif // CDot
