#ifndef TDOT_H
#define TDOT_H

#include <random>
#pragma once
#include <QPixmap>
#include <QPoint>
#include <QObject>
#include <QVector>

#include "cutility.h"

#if defined(QT_DEBUG)
#include <QDebug>
#endif


typedef struct t_Dot{
    unsigned m_Id = 0;

    eColor m_Color = eColor::red;
    int m_BaseSize = 10;
    int m_Size = 10;
    int m_Radius = 5;
    float m_Scale = 1.0f;
    QPoint m_Position = QPoint(0,0);
    QPoint m_Previous = QPoint(0,0);

    bool m_Touched = false; // If !m_Touched then moving
    bool m_Exploded = false; // If m_Touched and !m_Exploded then currently exploding

    QPoint m_Direction = QPoint(1,1);
    float m_Sizing = 1;

    QSize m_BoxBounds = QSize(0,0);
    float m_MaxScale = 3.0f;

    QPixmap* m_Pixmap;

    // to efficiently track cell information
    TCharID m_CellLookup;

    void setBaseSize(int Size){
        m_BaseSize = Size;
        m_Size = Size;
        m_Radius = Size / 2;
    }

    void setRandomColor() {
        static std::uniform_int_distribution<int> dist(1, 6);
        static std::mt19937 rng(std::random_device{}());
        m_Color = (eColor)dist(rng);
    }

    void setRandomDirection() {
        static std::uniform_int_distribution<int> distDir(1, 5);
        static std::uniform_int_distribution<int> distSign(0, 1);  // Random choice for sign (0 or 1)
        static std::mt19937 rng(std::random_device{}());

        // Ensuring the direction is always either -1 or 1, never 0
        int rDirX = (distSign(rng) == 0) ? -1 : 1;  // Random -1 or 1 for X direction
        int rDirY = (distSign(rng) == 0) ? -1 : 1;  // Random -1 or 1 for Y direction
        m_Direction = QPoint(rDirX * distDir(rng), rDirY * distDir(rng));
    }

    void setRandomPosition(QSize Bounds) {
        std::uniform_int_distribution<int> distX(0, Bounds.width());
        std::uniform_int_distribution<int> distY(0, Bounds.height());
        std::mt19937 rng(std::random_device{}());

        m_Position = QPoint(distX(rng), distY(rng));
    }

    // Updates position, movement, size
    void update(){
        if ( !m_Touched ){
            //Update movement
            m_Previous = m_Position;

            // X check, is outside box bounds, if not then move in direction
            if ( m_Direction.x() > 0 && (m_Position.x() + m_Radius) >= m_BoxBounds.width()) {
                m_Direction.setX(m_Direction.x() * -1);
            }
            else if ( m_Direction.x() < 0 && (m_Position.x() - m_Radius) <= 0) {
                m_Direction.setX(m_Direction.x() * -1);
            }
            else {
                m_Position.setX(m_Position.x() + m_Direction.x());
            }

            // Y check, for hitting box bounds, if not then move in direction
            if ( m_Direction.y() > 0 && (m_Position.y() + m_Radius) >= m_BoxBounds.height() )
            {
                m_Direction.setY(m_Direction.y() * -1);
            }
            else if ( m_Direction.y() < 0 && (m_Position.y() - m_Radius) <= 0 )
            {
                m_Direction.setY(m_Direction.y() * -1);
            }
            else {
                m_Position.setY(m_Position.y() + m_Direction.y());
            }

        } else if( m_Touched && !m_Exploded ){
            //Update resize
            if ( m_Sizing > 0 )
            {
                m_Scale += .25f;

                if ( m_Scale >= m_MaxScale )
                    m_Sizing = -1;
            }
            else
            {
                m_Scale -= .33f;

                if ( m_Scale <= 0.01f )
                {
                    m_Scale = .005f;
                    m_Exploded = true;
                }
            }

            m_Size = m_BaseSize * m_Scale;
            m_Radius = m_Size / 2;
        }
    }

    QRect getDrawRect()
    {
        QRectF BoundsF((float)m_Position.x() - m_Radius,
                       (float)m_Position.y() - m_Radius,
                       m_Size,
                       m_Size);

        //qDebug() << " Touched: " << m_Touched << " Exploded: " << m_Exploded << " Scale: " << m_Scale << " Size: " << m_Size << " Size: " << BoundsF;

        return BoundsF.toAlignedRect();
    }
}TDot,*PTDot;


#endif // TDot
