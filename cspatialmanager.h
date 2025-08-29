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

#ifndef CSPATIALMANAGER_H
#define CSPATIALMANAGER_H

#pragma once
#include <QHash>
#include <QObject>

#include "tdot.h"

typedef QHash<TCharID,QHash<int,PTDot>> t_DotsSpaceData;

class CSpatialManager : public QObject
{
    Q_OBJECT

    t_DotsSpaceData m_Space;    

    QSize m_WindowSize;
    QPoint m_GridSize;
    QSize m_CellSize;

    QHash<TCharID,QRect> m_Cells;

    QVector<TCharID> getCellsFromBB(PTDot Dot);

    TCharID getCellFromPoint(PTDot Dot);
    TCharID getCellFromPoint(QPoint Point);
public:
    explicit CSpatialManager(QSize WindowSize = QSize(1,1), QObject *parent = nullptr);

    void createManager(QSize WindowSize);

    void updateDot(PTDot Dot);
    void removeDot(PTDot Dot);

    QVector<PTDot> getNearestDots(PTDot Dot);
    QVector<PTDot> getNearestDots(QPoint Point);

    ////////////////////////////////////////////
    // Mainly for debugging and visualization
    ////////////////////////////////////////////
    QRect getCenterPointCell(PTDot Dot);
    QRect getDotCellsAsRect(PTDot Dot);

    static CSpatialManager* getGlobalInstance();
signals:
};

#endif // CSPATIALMANAGER_H
