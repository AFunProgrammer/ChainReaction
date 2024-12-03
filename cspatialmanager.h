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
