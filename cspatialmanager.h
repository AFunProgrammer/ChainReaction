#ifndef CSPATIALMANAGER_H
#define CSPATIALMANAGER_H

#pragma once
#include <QHash>
#include <QMap>
#include <QObject>

#include "tdot.h"

typedef QHash<QString,QMap<int,PTDot>> t_DotsSpaceData;

class CSpatialManager : public QObject
{
    Q_OBJECT

    t_DotsSpaceData m_Space;

    QSize m_WindowSize;
    QPoint m_GridSize;
    QSize m_CellSize;

    QHash<QString,QRect> m_Cells;


    QVector<QString> getCellsFromBB(PTDot Dot);
    QString getCellFromPoint(PTDot Dot);
    QString getCellFromPoint(QPoint Point);

    void saveCellLookupToDot(QString Lookup, PTDot Dot);
    QString getCellLookupFromDot(PTDot Dot);
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
signals:
};

#endif // CSPATIALMANAGER_H
