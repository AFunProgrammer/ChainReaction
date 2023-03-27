#include "cclickcap.h"

#include <QApplication>
#include <QGuiApplication>
#include <QDesktopServices>

#include <QMouseEvent>
#include <QPainter>
#include <QPaintDevice>


#include <chrono>

// simulation of Windows GetTickCount()
unsigned long long
GetTickCount()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

// Clock built upon Windows GetTickCount()
struct TickCountClock
{
    typedef unsigned long long                       rep;
    typedef std::milli                               period;
    typedef std::chrono::duration<rep, period>       duration;
    typedef std::chrono::time_point<TickCountClock>  time_point;
    static const bool is_steady =                    true;

    static time_point now() noexcept
    {
        return time_point(duration(GetTickCount()));
    }
};


CClickCap::CClickCap(QWidget *parent)
    : QWidget{parent}
{
    //this->setMouseTracking(true);
}

void CClickCap::setBounds(QRect Geometry)
{
    setGeometry(Geometry);
}

QRect CClickCap::getBounds()
{
    return geometry();
}

QPoint CClickCap::getLastClick()
{
    while( false == m_updateLock.tryLockForRead() );
    QPoint lastClick = m_lastClick;
    m_updateLock.unlock();

    return lastClick;
}

QRect CClickCap::getLastClickArea()
{
    while( false == m_updateLock.tryLockForRead() );
    QRect lastClickArea = m_lastClickArea;
    m_updateLock.unlock();

    return lastClickArea;
}

qulonglong CClickCap::getLastUpdateTick()
{
    while( false == m_updateLock.tryLockForRead() );
    int lastUpdateTick = m_lastUpdateTick;
    m_updateLock.unlock();

    return lastUpdateTick;
}


void CClickCap::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "[Mouse Moving]: " << event->pos();
}

void CClickCap::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "[CClickCap]: mousePressEvent" << event->pos();
    QPainter qpMeasure(this);

    int icount = 0;
    while( false == m_updateLock.tryLockForWrite() )
    {
        icount++;
        if (icount > 10000)
            Q_ASSERT(false);
    }

    m_lastUpdateTick = GetTickCount();

    m_lastClick = event->pos();

    if ( m_clickHistory.count() >= 30 )
    {
        m_clickHistory.removeLast();
    }

    m_clickHistory.insert(0,m_lastClick);

    float fDpcmX = (float)qpMeasure.device()->logicalDpiX() / 2.54f;
    float fDpcmY = (float)qpMeasure.device()->logicalDpiY() / 2.54f;

    //Click area is considered the 'nice' area of being clicked in
    // allowing for 'fat' finger touch to select a specific spot
    // within a region and allowing for making mistakes but
    // still having the pleasure of using touch
    m_lastClickArea.setX(m_lastClick.x() - (int)(fDpcmX/2.0f) );
    m_lastClickArea.setY(m_lastClick.y() - (int)(fDpcmY/2.0f) );
    m_lastClickArea.setWidth((int)(fDpcmX) );
    m_lastClickArea.setHeight((int)(fDpcmY) );

    m_updateLock.unlock();

    //qDebug() << "[Last Click] Time: " << m_lastUpdateTick << " Pos: " << m_lastClick << " Click/Touch Area: " << m_lastClickArea;
}

void CClickCap::paintEvent(QPaintEvent* event)
{
    /*
    QPainter Painter(this);
    QPen pen = QPen(Qt::red, 6);
    QBrush brush = QBrush(Qt::red);

    Painter.setPen(pen);
    Painter.setBrush(brush);

    Painter.save();
    Painter.drawRect(m_lastClickArea);
    Painter.restore();
    */
}

