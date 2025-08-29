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

#ifndef CDOTSMANAGER_H
#define CDOTSMANAGER_H

#pragma once
#include <QPicture>
#include <QPixmap>
#include <QPoint>
#include <QObject>
#include <QReadWriteLock>

#include "cutility.h"
#include "tdot.h"
#include "cspatialmanager.h"

class CDotsManager : public QObject
{
    Q_OBJECT

//single class needs
private:
    QReadWriteLock m_DotsLock;

    uint m_Collisions = 0;
    uint m_LastCollisionCount = 0;

    uint m_UpdateDotsSize = 0;
    QSize m_UpdateBounds = QSize(0,0);
    PTDot m_dotTouched = nullptr;

    QVector<PTDot> m_Dots;
    QVector<PTDot> m_RemovedDots;
    uint m_Size = 10;
    QSize m_Bounds = QSize(100,100);

    CSpatialManager* m_pManager = CSpatialManager::getGlobalInstance();

    std::map<eColor, QPixmap> m_DotPixmaps;

    void createDotPixmaps();
    QPixmap* getDotPixmapByColor(eColor Color);

    QPixmap m_DrawBuffer;
public:
    explicit CDotsManager(QObject *parent = nullptr);
    ~CDotsManager(){}

    QVector<PTDot> getDots();
    void setupDots(int Count, QSize Bounds = QSize(100,100), uint Size = 10);
    uint getRemainingDotCount();
    void clearDots();

    void setDotsSize(uint Size);
    void setDotsBounds(QSize Bounds);

    void updateDotsMovement(QVector<PTDot>* DotsForPhysics);
    void updateDotsPhysics(QVector<PTDot>* PhysicsDots);

    void doCollisionCheck(QPoint Point);

    uint getCollisionCount();
    uint getLastCollisionCount();

    void drawDotsToBuffer();
    void drawDotsUsingPaintDevice(QPainter* Painter);

    QPixmap getDrawBuffer();

    static CDotsManager* getGlobalInstance();
signals:

};

#endif // CDOTSMANAGER_H
