/*
 * This file is part of chainreaction.
 *
 * Copyright (C) 2023-2025 InspiringFamily.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

#include "cdotsmanager.h"

#include <QPainter>
#include <math.h>


// Global Instance
CDotsManager* CDotsManager::getGlobalInstance(){
    static CDotsManager g_DotsManager;
    return &g_DotsManager;
}

CDotsManager::CDotsManager(QObject *parent) : QObject{parent}
{
    createDotPixmaps();
}

uint CDotsManager::getCollisionCount()
{
    return m_Collisions;
}

uint CDotsManager::getLastCollisionCount()
{
    return m_LastCollisionCount;
}

uint CDotsManager::getRemainingDotCount()
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

QVector<PTDot> CDotsManager::getDots()
{
    return m_Dots;
}

void CDotsManager::setupDots(int Count, QSize Bounds, uint Size){
    // Done before any render timer created
    //  should probably be in the readWrite lock...
    PTDot dot = nullptr;

    // remove all dots first
    clearDots();

    // setup allocations
    m_Dots.reserve(Count);
    m_RemovedDots.reserve(Count);

    for ( int iDot = 0; iDot < Count; iDot++ )
    {
        dot = new TDot;
        dot->setRandomPosition(Bounds);
        dot->setRandomDirection();
        dot->setRandomColor();
        dot->m_BoxBounds = Bounds;
        dot->m_Pixmap = getDotPixmapByColor(dot->m_Color);
        //int rSize = (int)((float)Size * (5.0f/(float)(rand()%20+1)));
        int rSize = Size;
        dot->setBaseSize(rSize);
        dot->m_Id = (unsigned)iDot;
        m_Dots.push_back(dot);
    }

    m_pManager->createManager(Bounds);
}

QPixmap* CDotsManager::getDotPixmapByColor(eColor Color)
{
    return &(m_DotPixmaps[Color]);
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

    for ( PTDot dot: m_Dots ){
        delete dot;
    }

    for ( PTDot dot: m_RemovedDots ){
        delete dot;
    }

    m_Dots.clear();
    m_RemovedDots.clear();

    m_DotsLock.unlock();
}

void CDotsManager::setDotsSize(uint Size)
{
    if ( Size == m_Size )
        return;

    m_UpdateDotsSize = Size;
}

void CDotsManager::setDotsBounds(QSize Bounds){
    if ( Bounds == m_Bounds)
        return;

    m_UpdateBounds = Bounds;
    // Need to setup new spatial manager
    m_pManager->createManager(Bounds);

}


void CDotsManager::updateDotsMovement(QVector<PTDot>* DotsForPhysics)
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

    QVector<PTDot> resizingDots;
    QVector<PTDot>* physicsNeededOnDots = nullptr;

    if ( DotsForPhysics ){
        physicsNeededOnDots = DotsForPhysics;
        physicsNeededOnDots->clear();
    } else {
        physicsNeededOnDots = &resizingDots;
    }

    physicsNeededOnDots->reserve(m_Dots.count());  // Reserve space to avoid reallocations

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
    if ( m_dotTouched ){
        m_dotTouched->m_Touched = true;
        m_dotTouched = nullptr;
    }

    uint uDots = m_Dots.count();

    for(uint uDot = 0; uDot < uDots; uDot++ )
    {
        if ( m_Dots[uDot]->m_Exploded )
        {
            m_RemovedDots.append(m_Dots[uDot]);
            m_pManager->removeDot(m_Dots[uDot]);

            m_Dots.removeAt(uDot);

            uDots--;
            uDot--;
            continue;
        }

        m_Dots[uDot]->update();
        m_pManager->updateDot(m_Dots[uDot]);
        if ( m_Dots[uDot]->m_Touched )
        {
            physicsNeededOnDots->append(m_Dots[uDot]);
        }

        (*resizeFunction)(m_Dots[uDot],m_UpdateDotsSize);
        (*boundsFunction)(m_Dots[uDot],m_UpdateBounds);
    }

    if ( updateBounds ){
        m_Bounds = m_UpdateBounds;
    }

    if ( updateDotsSize )
        m_Size = m_UpdateDotsSize;

    m_UpdateBounds = QSize(0,0);
    m_UpdateDotsSize = 0;

    m_DotsLock.unlock();
}

void CDotsManager::updateDotsPhysics(QVector<PTDot>* PhysicsDots)
{
    if ( !PhysicsDots ){
        return; //no list to process
    }

    // this is done only during the update function
    if ( PhysicsDots->count() > 0 )
    {
        // !!!very important
        // attempt to lock, otherwise exit
        int tryLockAttempt = 0;
        while ( m_DotsLock.tryLockForWrite(1) == false ){
            tryLockAttempt++;
            if ( tryLockAttempt >= 3 ){
                return; //could not obtain a lock quick enough
            }
        }

        for(PTDot dot: *PhysicsDots)
        {
            QVector<PTDot> nearestDots = m_pManager->getNearestDots(dot);

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

        m_DotsLock.unlock();
    }
    else //Collisions are reduced to 0
    {
        m_LastCollisionCount = m_Collisions;
        m_Collisions = 0;
    }
}

void CDotsManager::doCollisionCheck(QPoint Point){
    QPoint clickPos = Point;
    //Click area is considered the 'nice' area of being clicked in
    // allowing for 'fat' finger touch to select a specific spot
    // within a region and allowing for making mistakes but
    // still having the pleasure of using touch
    QRect clickArea = QRect(clickPos.x()-24,clickPos.y()-24,48,48);

    QVector<PTDot> Dots = m_pManager->getNearestDots(clickPos);
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
        m_dotTouched = closest;
    }

    m_DotsLock.unlock();
}


void CDotsManager::drawDotsUsingPaintDevice(QPainter* Painter){
    int tryLockAttempt = 0;
    while ( m_DotsLock.tryLockForRead(1) == false ){
        tryLockAttempt++;
        if ( tryLockAttempt >= 3 ){
            return; //could not obtain a lock quick enough
        }
    }

    for( PTDot dot: m_Dots )
    {
        Painter->drawPixmap(dot->getDrawRect(), *(dot->m_Pixmap));
    }

#if defined(QT_DEBUG)
    if ( m_Dots.size() > 0 ){
        QRect cells = m_pManager->getDotCellsAsRect(m_Dots[0]);
        QVector<PTDot> nearestDots = m_pManager->getNearestDots(m_Dots[0]);
        QRect dotRect = m_Dots[0]->getDrawRect();

        Painter->setPen(Qt::red);
        Painter->drawRect(cells);
        Painter->setPen(Qt::darkGray);
        Painter->setBrush(Qt::darkGray);
        for( PTDot dot: nearestDots ){
            Painter->drawEllipse(dot->getDrawRect());
        }
        Painter->setPen(Qt::white);
        Painter->setBrush(Qt::white);
        Painter->drawEllipse(dotRect);
    }
#endif

    m_DotsLock.unlock();
}

void CDotsManager::drawDotsToBuffer()
{
    int tryLockAttempt = 0;
    while ( m_DotsLock.tryLockForRead(1) == false ){
        tryLockAttempt++;
        if ( tryLockAttempt >= 3 ){
            return; //could not obtain a lock quick enough
        }
    }

    m_DrawBuffer.fill(Qt::transparent);
    QPainter BufferPainter(&m_DrawBuffer);

    for( PTDot dot: m_Dots )
    {
        BufferPainter.drawPixmap(dot->getDrawRect(), *(dot->m_Pixmap));
    }

#if defined(QT_DEBUG)
    if ( m_Dots.size() > 0 ){
        QRect cells = m_pManager->getDotCellsAsRect(m_Dots[0]);
        QVector<PTDot> nearestDots = m_pManager->getNearestDots(m_Dots[0]);
        QRect dotRect = m_Dots[0]->getDrawRect();

        BufferPainter.setPen(Qt::red);
        BufferPainter.drawRect(cells);
        BufferPainter.setPen(Qt::darkGray);
        BufferPainter.setBrush(Qt::darkGray);
        for( PTDot dot: nearestDots ){
            BufferPainter.drawEllipse(dot->getDrawRect());
        }
        BufferPainter.setPen(Qt::white);
        BufferPainter.setBrush(Qt::white);
        BufferPainter.drawEllipse(dotRect);
    }
#endif
    BufferPainter.end();

    m_DotsLock.unlock();
}

QPixmap CDotsManager::getDrawBuffer(){
    return m_DrawBuffer;
}


