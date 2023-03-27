#include "cdot.h"

#include <QPainter>
#include <time.h>
#include <ctime>
#include <chrono>

using namespace std;

static  auto time_start = std::chrono::high_resolution_clock::time_point();

CDot::CDot()
{
    //auto time_in_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(time_start.time_since_epoch()).count();

    m_Id = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-time_start).count();

    //qDebug() << "CDot Id: " << m_Id;
}

CDot* CDot::operator= (CDot RHS)
{
    m_Pixmap = RHS.m_Pixmap;

    m_Color = RHS.m_Color;

    m_Size = RHS.m_Size;
    m_Radius = RHS.m_Radius;

    m_Bounds = RHS.m_Bounds;
    m_Pos = RHS.m_Pos;

    m_Resize = RHS.m_Resize;
    m_IncreasingSize = RHS.m_IncreasingSize;

    m_ScaleMin = RHS.m_ScaleMin;
    m_ScaleMax = RHS.m_ScaleMax;
    m_Scale = RHS.m_Scale;

    m_Move = RHS.m_Move;
    m_XMovement = RHS.m_XMovement;
    m_YMovement = RHS.m_YMovement;
    m_Speed = RHS.m_Speed;

    m_Stopped = RHS.m_Stopped;

    m_Id = RHS.m_Id;

    return this;
}

CDot* CDot::operator= (CDot* RHS)
{
    m_Pixmap = RHS->m_Pixmap;

    m_Color = RHS->m_Color;

    m_Size = RHS->m_Size;
    m_Radius = RHS->m_Radius;

    m_Bounds = RHS->m_Bounds;
    m_Pos = RHS->m_Pos;

    m_Resize = RHS->m_Resize;
    m_IncreasingSize = RHS->m_IncreasingSize;

    m_ScaleMin = RHS->m_ScaleMin;
    m_ScaleMax = RHS->m_ScaleMax;
    m_Scale = RHS->m_Scale;

    m_Move = RHS->m_Move;
    m_XMovement = RHS->m_XMovement;
    m_YMovement = RHS->m_YMovement;
    m_Speed = RHS->m_Speed;

    m_Stopped = RHS->m_Stopped;

    m_Id = RHS->m_Id;

    return this;
}



void CDot::setPos(QPoint Pos)
{
    m_Pos = Pos;
}

QPoint CDot::getPos()
{
    return m_Pos;
}

void CDot::setMovement(Movement X, Movement Y)
{
    m_XMovement = X;
    m_YMovement = Y;
}

CDot::Movement CDot::getXMovement()
{
    return m_XMovement;
}

CDot::Movement CDot::getYMovement()
{
    return m_YMovement;
}

void CDot::setColor(eColor Color)
{
    m_Color = Color;
}

eColor CDot::getColor()
{
    return m_Color;
}

void CDot::setSize(uint Size)
{
    m_Size = Size;

    if ( m_Size < 10)
        m_Size = 10;

    m_Radius = m_Size / 2;
}

uint CDot::getSize()
{
    return m_Size;
}

void CDot::setPixmap(QPixmap* Pixmap)
{
    //Pixmap will be the maximum size needed in order to have smooth scaling
    m_Pixmap = Pixmap;
}

QPixmap* CDot::getPixmap()
{
    return m_Pixmap;
}


void CDot::setScale(float Scale)
{
    m_Scale = Scale;
}

void CDot::setScaleMin(float Min)
{
    m_ScaleMin = Min;
}

void CDot::setScaleMax(float Max)
{
    m_ScaleMax = Max;
}

float CDot::getScale()
{
    return m_Scale;
}

float CDot::getScaleMin()
{
    return m_ScaleMin;
}

float CDot::getScaleMax()
{
    return m_ScaleMax;
}

void CDot::updateMovement()
{
    if ( m_XMovement == Movement::Positive )
    {
        if ( (m_Pos.x() + m_Radius) < m_Bounds.right() )
            m_Pos.setX(m_Pos.x() + m_Speed.x());
        else
            m_XMovement = Movement::Negative;
    }
    else if (m_XMovement == Movement::Negative)
    {
        if ( (m_Pos.x() - m_Radius) > m_Bounds.left() )
            m_Pos.setX(m_Pos.x() - m_Speed.x());
        else
            m_XMovement = Movement::Positive;
    }

    //TODO: figure out what happened that caused the need to add
    //      an extra equation to remove half again the radius
    //      of the circle from up-down movements
    if ( m_YMovement == Movement::Positive )
    {
        if ( (m_Pos.y() + m_Radius + (int)((float)m_Radius/2.0f)) < m_Bounds.bottom() )
            m_Pos.setY(m_Pos.y() + m_Speed.y());
        else
            m_YMovement = Movement::Negative;
    }
    else if (m_YMovement == Movement::Negative)
    {
        if ( (m_Pos.y() - m_Radius - (int)((float)m_Radius/2.0f)) > m_Bounds.top() )
            m_Pos.setY(m_Pos.y() - m_Speed.y());
        else
            m_YMovement = Movement::Positive;
    }
}

void CDot::updateScaling()
{
    if ( m_IncreasingSize )
    {
        m_Scale += .25f;

        if ( m_Scale >= m_ScaleMax )
            m_IncreasingSize = false;
    }
    else
    {
        m_Scale -= .33f;

        if ( m_Scale <= 0.01f )
        {
            m_Scale = .005f;
            m_Resize = false;
            m_IncreasingSize = true;
            m_Move = false;
            m_Stopped = true;
        }
    }
}

void CDot::update()
{

    if ( m_Move )
    {
        updateMovement();
    }

    if ( m_Resize )
    {
        updateScaling();
    }
}


void CDot::setSpeed(QPoint Speed)
{

    if ( Speed.x() > 10 )
        Speed.setX(10);
    else if ( Speed.x() < 5 )
        Speed.setX(5);

    if ( Speed.y() > 10 )
        Speed.setY(10);
    else if ( Speed.y() < 5 )
        Speed.setY(5);

    m_Speed = Speed;//Speed;
}


QPoint CDot::getSpeed()
{
    return m_Speed;
}

void CDot::startResizing()
{
    if ( m_Resize == true || m_Move == false )
        return;

    m_Resize = true;
    m_Move = false;
}

bool CDot::isResizing()
{
    if ( m_Resize == true && m_Move == false )
        return true;

    return false;
}

void CDot::setBounds(QRect Rect)
{
    m_Bounds = Rect;
}

void CDot::setId(ulong Id)
{
    m_Id = Id;
}

ulong CDot::getId()
{
    return m_Id;
}


QRect CDot::getDrawRect()
{
    QRectF HalfPixelMeasure((float)m_Pos.x() - ((float)m_Radius*m_Scale),
                            (float)m_Pos.y() - ((float)m_Radius*m_Scale),
                            ((float)m_Size*m_Scale),
                            ((float)m_Size*m_Scale));

    QRect Bounds = QRect((int)HalfPixelMeasure.x(), (int)HalfPixelMeasure.y(), (int)HalfPixelMeasure.width(), (int)HalfPixelMeasure.height());

    //qDebug() << "Dot: " << objectName() << " Position: " << m_Pos << " Draw Rect: " << Bounds << " Half Pixel Measure: " << HalfPixelMeasure ;

    return Bounds;
}

bool CDot::isStopped()
{
    return m_Stopped;
}



