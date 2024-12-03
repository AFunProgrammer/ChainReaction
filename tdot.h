#ifndef TDOT_H
#define TDOT_H

#pragma once


#include "cutility.h"
#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <random>


struct t_Dot {
    unsigned m_Id = 0;

    eColor m_Color = eColor::red;
    int m_BaseSize = 10;
    int m_Size = 10;
    int m_Radius = 5;
    float m_Scale = 1.0f;
    QPoint m_Position = QPoint(0, 0);
    QPoint m_Previous = QPoint(0, 0);

    bool m_Touched = false; // If !m_Touched then moving
    bool m_Exploded = false; // If m_Touched and !m_Exploded then currently exploding

    QPoint m_Direction = QPoint(1, 1);
    float m_Sizing = 1;

    QSize m_BoxBounds = QSize(0, 0);
    float m_MaxScale = 3.0f;

    QPixmap* m_Pixmap;

    TCharID m_CellLookup;

    // Member function declarations
    void setBaseSize(int Size);
    void setRandomColor();
    void setRandomDirection();
    void setRandomPosition(QSize Bounds);
    void update();
    QRect getDrawRect();
};

// Typedefs for convenience
typedef t_Dot TDot, *PTDot;

#endif // TDOT_H
