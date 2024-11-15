#ifndef CSPACIALMANAGER_H
#define CSPACIALMANAGER_H

#pragma once
#include <QHash>
#include <QMap>
#include <QObject>

#include "tdot.h"

typedef QHash<QString,QMap<int,PTDot>> t_DotsSpaceData;

class CSpacialManager : public QObject
{
    Q_OBJECT

    t_DotsSpaceData m_Space;
    QPoint m_GridSize;
    QSize m_CellSize;

    QHash<QString,QRect> m_Cells;


    QVector<QString> getTouchedCellsLookup(PTDot Dot);

    void saveCellLookupsToDot(QVector<QString> Lookups, PTDot Dot);
    QVector<QString> getCellLookupsFromDot(PTDot Dot);
public:
    explicit CSpacialManager(QSize WindowSize = QSize(1,1), QObject *parent = nullptr);

    void createManager(QSize WindowSize);

    void updateDot(PTDot Dot);
    void removeDot(PTDot Dot);

    QVector<PTDot> getNearestDots(PTDot Dot);

    ////////////////////////////////////////////
    // Mainly for debugging and visualization
    ////////////////////////////////////////////
    QRect getCenterPointCell(PTDot Dot);
    QRect getDotCellsAsRect(PTDot Dot);
signals:
};

#endif // CSPACIALMANAGER_H
