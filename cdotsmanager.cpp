#include "cdotsmanager.h"

#include <QPainter>
#include <math.h>

CDotsManager::CDotsManager(QWidget *parent) : QOpenGLWidget{parent}
{
    setAutoFillBackground(false);
    createDotPixmaps();
}

void CDotsManager::setClearColor(QColor Color)
{
    m_ClearColor = Color;
}

uint CDotsManager::getCollisionCount()
{
    return m_Collisions;
}

uint CDotsManager::getLastCollisionCount()
{
    return m_LastCollisionCount;
}

uint CDotsManager::getDotCount()
{
    static int lastCount = 0;
    if (!m_DotsLock.tryLockForRead(1)){
        return lastCount;
    }

    lastCount = m_Dots.count();

    m_DotsLock.unlock();

    return lastCount;
}

void CDotsManager::createDotPixmaps()
{
    for(eColor color: CUtility::m_ColorDictKeys)
    {
        QImage dotImage(100,100, QImage::Format_ARGB32);
        QPainter dotPainter(&dotImage);

        QPen dotPen = QPen(QColor::fromRgba(0x99000000), 1, Qt::SolidLine);
        QBrush dotBrush = QBrush(CUtility::m_ColorDict[color].m_QtColor, Qt::SolidPattern);

        dotPainter.setRenderHint(QPainter::Antialiasing);
        dotPainter.setRenderHint(QPainter::LosslessImageRendering);

        dotImage.fill(QColor::fromRgba(0x00000000));

        dotPainter.setPen(dotPen);
        dotPainter.setBrush(dotBrush);
        dotPainter.drawEllipse(0,0,100,100);
        dotPainter.end();

        QPixmap dotPixmap(100,100);
        dotPixmap.convertFromImage(dotImage);

        this->m_DotPixmaps.insert(std::pair<eColor,QPixmap>(color,dotPixmap));
    }
}

QList<PTDot> CDotsManager::getDots()
{
    return m_Dots;
}

void CDotsManager::addDot(PTDot Dot)
{
    // Done before any render timer created
    //  should probably be in the readWrite lock...
    if ( Dot != nullptr )
    {
        Dot->m_BoxBounds = this->geometry().size();
        setDotPixmap(Dot);
        m_Dots.append(Dot);
        m_RemovedDots.resize(m_Dots.count());
    }
}

void CDotsManager::setDotPixmap(PTDot Dot)
{
    Dot->m_MaxScale = 3.0f;
    Dot->m_Pixmap = m_DotPixmaps[Dot->m_Color];
}

void CDotsManager::clearDots()
{
    int tryLockAttempt = 0;
    while ( m_DotsLock.tryLockForWrite(1) == false ){
        tryLockAttempt++;
        if ( tryLockAttempt >= 5 ){
            return; //could not obtain a lock quick enough
        }
    }

    for(PTDot dot: m_Dots)
    {
        delete dot;
    }

    for(PTDot dot: m_RemovedDots)
    {
        delete dot;
    }

    m_Dots.clear();
    m_RemovedDots.clear();
    m_Manager.createManager(geometry().size());

    m_DotsLock.unlock();
}

void CDotsManager::setDotsSize(uint Size)
{
    m_UpdateDotsSize = Size;
}


void CDotsManager::updateDots()
{
    //
    //  what can I do architecturally different to always update without incurring
    //   speed penalties of writes or excessive checks?
    //
    static std::function<void(PTDot,int)> resizeDot = [](PTDot dot, int BaseSize){dot->setBaseSize(BaseSize);};
    static std::function<void(PTDot,int)> noResizeDot = [](PTDot, int){};
    static std::function<void(PTDot,int)>* resizeFunction = nullptr;
    static std::function<void(PTDot,QSize)> setBounds = [](PTDot dot, QSize Bounds){dot->m_BoxBounds = Bounds;};
    static std::function<void(PTDot,QSize)> noSetBounds = [](PTDot, QSize){};
    static std::function<void(PTDot,QSize)>* boundsFunction = nullptr;

    // Cache frequently accessed values
    bool updateBounds = m_UpdateBounds.width() || m_UpdateBounds.height();
    bool updateDotsSize = m_UpdateDotsSize;

    QList<PTDot> resizingDots;
    resizingDots.reserve(m_Dots.count());  // Reserve space to avoid reallocations

    // Determine which function to use based on the conditions
    boundsFunction = updateBounds ? &setBounds : &noSetBounds;
    resizeFunction = updateDotsSize ? &resizeDot : &noResizeDot;

    // !!!very important
    // attempt to lock, otherwise exit
    int tryLockAttempt = 0;
    while ( m_DotsLock.tryLockForWrite(1) == false ){
        tryLockAttempt++;
        if ( tryLockAttempt >= 3 ){
            return; //could not obtain a lock quick enough
        }
    }

    // [Input] touched dot is only updated in this function
    if ( m_MouseTouched ){
        m_MouseTouched->m_Touched = true;
        m_MouseTouched = nullptr;
    }

    uint uDots = m_Dots.count();

    for(uint uDot = 0; uDot < uDots; uDot++ )
    {
        if ( m_Dots[uDot]->m_Exploded )
        {
            m_RemovedDots.append(m_Dots[uDot]);
            m_Manager.removeDot(m_Dots[uDot]);

            m_Dots.removeAt(uDot);

            uDots--;
            uDot--;
            continue;
        }

        m_Dots[uDot]->update();
        m_Manager.updateDot(m_Dots[uDot]);
        if ( m_Dots[uDot]->m_Touched )
        {
            //setDotPixmap(m_Dots[uDot]);
            resizingDots.append(m_Dots[uDot]);
        }

        (*resizeFunction)(m_Dots[uDot],m_UpdateDotsSize);
        (*boundsFunction)(m_Dots[uDot],m_UpdateBounds);
    }

    checkForCollisions(resizingDots);

    m_UpdateBounds = QSize(0,0);
    m_UpdateDotsSize = 0;

    m_DotsLock.unlock();
}

void CDotsManager::checkForCollisions(QList<PTDot> resizingDots)
{
    // this is done only during the update function
    if ( resizingDots.count() > 0 )
    {
        for(PTDot dot: resizingDots)
        {
            QVector<PTDot> nearestDots = m_Manager.getNearestDots(dot);

            for( PTDot nearDot: nearestDots ){
                if ( nearDot->m_Touched )
                    continue;

                uint uiCrossProduct = CUtility::getCrossProduct(dot->m_Position, nearDot->m_Position);
                uint uiDotDistance = dot->m_Radius + nearDot->m_Radius;

                if (uiCrossProduct <= uiDotDistance)
                {
                    nearDot->m_Touched = true;
                    m_Collisions++;
                    //qDebug() << "Dot: " << dot << " Near Dot: " << nearDot;
                }
            }
        }
    }
    else //Collisions are reduced to 0
    {
        m_LastCollisionCount = m_Collisions;
        m_Collisions = 0;
    }
}

void CDotsManager::drawDots()
{
    int tryLockAttempt = 0;
    while ( m_DotsLock.tryLockForRead(1) == false ){
        tryLockAttempt++;
        if ( tryLockAttempt >= 3 ){
            return; //could not obtain a lock quick enough
        }
    }

    m_DrawBuffer.fill(Qt::transparent);
    QPainter bufferPainter(&m_DrawBuffer);

    for( PTDot dot: m_Dots )
    {
        bufferPainter.drawPixmap(dot->getDrawRect(), dot->m_Pixmap);
    }
    bufferPainter.end();

    m_DotsLock.unlock();
}

void CDotsManager::resizeEvent(QResizeEvent *event){
    QOpenGLWidget::resizeEvent(event);
    //update dots bounding box in update function
    m_UpdateBounds = this->geometry().size();
    //re-create draw buffer based upon resize
    m_DrawBuffer = QPixmap(event->size()*2);
    //re-create manager based upon resized window
    m_Manager.createManager(geometry().size());
}

void CDotsManager::mousePressEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();
    //Click area is considered the 'nice' area of being clicked in
    // allowing for 'fat' finger touch to select a specific spot
    // within a region and allowing for making mistakes but
    // still having the pleasure of using touch
    QRect clickArea = QRect(clickPos.x()-24,clickPos.y()-24,48,48);

    QVector<PTDot> Dots = m_Manager.getNearestDots(clickPos);
    PTDot closest = nullptr;

    int tryLockAttempt = 0;
    while ( m_DotsLock.tryLockForRead(3) == false ){
        tryLockAttempt++;
        if ( tryLockAttempt >= 5 ){
            return; //could not obtain a lock quick enough
        }
    }

    for( PTDot dot : Dots )
    {
        if ( clickArea.intersects(dot->getDrawRect()) )
        {
            if ( closest != nullptr )
            {
                ulong uNewDot = CUtility::getCrossProduct(dot->m_Position, clickPos);
                ulong uDot = CUtility::getCrossProduct(closest->m_Position, clickPos);
                if ( uNewDot < uDot )
                    closest = dot;
            }
            else
               closest = dot;
        }
    }


    if ( closest != nullptr){
        m_MouseTouched = closest;
    }

    m_DotsLock.unlock();
}


void CDotsManager::paintEvent(QPaintEvent *event)
{
    QPainter Painter(this);
    QBrush brushBackground = QBrush(m_ClearColor);

    //Painter.begin(this);
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_LINE_SMOOTH);
    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.setRenderHint(QPainter::SmoothPixmapTransform);
    Painter.fillRect(event->rect(),brushBackground);

    drawDots();

    Painter.drawPixmap(0,0,m_DrawBuffer);


#if defined(QT_DEBUG)
    if ( m_Dots.size() > 0 ){
        QRect cells = m_Manager.getDotCellsAsRect(m_Dots[0]);
        QVector<PTDot> nearestDots = m_Manager.getNearestDots(m_Dots[0]);
        QRect dotRect = m_Dots[0]->getDrawRect();

        Painter.setPen(Qt::red);
        Painter.drawRect(cells);
        Painter.setPen(Qt::darkGray);
        Painter.setBrush(Qt::darkGray);
        for( PTDot dot: nearestDots ){
            Painter.drawEllipse(dot->getDrawRect());
        }
        Painter.setPen(Qt::white);
        Painter.setBrush(Qt::white);
        Painter.drawEllipse(dotRect);
    }
#endif

    Painter.end();
}


