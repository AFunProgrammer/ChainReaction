#ifndef TDOT_H
#define TDOT_H

#pragma once
#include <QPixmap>
#include <QPoint>
#include <QObject>
#include <QVector>

#include "cutility.h"


typedef struct t_Dot{
    unsigned m_Id = 0;

    eColor m_Color = eColor::red;
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

    // to efficiently track cell information
    int m_Cells = 0;
    wchar_t m_CellLookups[4][10];

    QPixmap m_Pixmap;

    void setRandomColor(){
        m_Color = (eColor)(rand()%6+1);
    }

    void setRandomDirection(){
        int rDirX = rand()%2 * 2 - 1;
        int rDirY = rand()%2 * 2 - 1;
        int DirX = rand()%6 + 2;
        int DirY = rand()%6 + 2;
        m_Direction = QPoint(rDirX * DirX, rDirY * DirY);
    }

    void setRandomPosition(QSize Bounds){
        m_Position = QPoint(rand()%Bounds.width(),rand()%Bounds.height());
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
        }
    }


    QRect getDrawRect()
    {
        float scaledSize = m_Size * m_Scale;
        float scaledRadius = scaledSize * 0.5f;

        QRectF BoundsF((float)m_Position.x() - scaledRadius,
                       (float)m_Position.y() - scaledRadius,
                       scaledSize,
                       scaledSize);

        return BoundsF.toAlignedRect();
    }
}TDot,*PTDot;


#endif // TDot
