#include "cspatialmanager.h"

#include <QSet>
#include <QtMath>

CSpatialManager::CSpatialManager(QSize WindowSize, QObject *parent)
    : QObject{parent}
{
    CSpatialManager::createManager(WindowSize);
}


void CSpatialManager::createManager(QSize WindowSize){
    bool bPortrait = (WindowSize.height() > WindowSize.width() ? true : false);
    float fRatio = (float)WindowSize.width() / (float)WindowSize.height();
    unsigned maxRect = 4;

    fRatio = (bPortrait ? fRatio : 1/fRatio);

    unsigned cols = (unsigned)qCeil((float)maxRect*(bPortrait ? fRatio : 1));
    unsigned rows = (unsigned)qCeil((float)maxRect*(bPortrait ? 1 : fRatio));

    m_WindowSize = WindowSize;
    m_CellSize = QSize(WindowSize.width() / cols, WindowSize.height() / rows);
    m_GridSize = QPoint(cols,rows);

    m_Space.clear();

    for( unsigned iCol=0; iCol < cols; iCol++ ){
        for( unsigned iRow=0; iRow < rows; iRow++ ){
            QString lookup = QString("%0.%1").arg(iCol).arg(iRow);

            m_Space[lookup] = QMap<int,PTDot>();
            m_Cells[lookup] = QRect(iCol * m_CellSize.width(),
                                    iRow * m_CellSize.height(),
                                    m_CellSize.width(),
                                    m_CellSize.height());
        }
    }
}

void CSpatialManager::updateDot(PTDot Dot){
    QVector<QString> touchedCells = getTouchedCellsLookup(Dot);
    QVector<QString> savedCells = getCellLookupsFromDot(Dot);
    QSet<QString> newCells = QSet<QString>(touchedCells.begin(),touchedCells.end());
    QSet<QString> prevCells = QSet<QString>(savedCells.begin(),savedCells.end());

    saveCellLookupsToDot(touchedCells, Dot);


    // Find unique elements in touchedCells that aren't in prevCells
    QSet<QString> nowInCells = newCells - prevCells;

    // Find unique elements in prevCells that aren't in touchedCells
    QSet<QString> notInCells = prevCells - newCells;

    for( QString cellLookup: nowInCells ){
        m_Space[cellLookup][Dot->m_Id] = Dot;
    }

    for( QString cellLookup: notInCells ){
        m_Space[cellLookup].remove(Dot->m_Id);
    }
}

void CSpatialManager::removeDot(PTDot Dot){
    QVector<QString> cells = getCellLookupsFromDot(Dot);

    for( QString cellLookup: cells ){
        m_Space[cellLookup].remove(Dot->m_Id);
    }
}


QVector<PTDot> CSpatialManager::getNearestDots(PTDot Dot){
    QVector<PTDot> nearestDots = QVector<PTDot>();

    QVector<QString> cells = getCellLookupsFromDot(Dot);

    for( QString cellLookup: cells ){
        nearestDots.append(m_Space[cellLookup].values());
    }

    return nearestDots;
}

QVector<PTDot> CSpatialManager::getNearestDots(QPoint Point){
    QVector<PTDot> nearestDots = QVector<PTDot>();
    QString cellLookup = getCellFromPoint(Point);

    nearestDots.append(m_Space[cellLookup].values());

    return nearestDots;
}



QVector<QString> CSpatialManager::getTouchedCellsLookup(PTDot Dot){
    QVector<QString> cells = QVector<QString>();
    QVector<QPoint> corners;
    QRect dotRect = Dot->getDrawRect();

    corners.append(QPoint(dotRect.topLeft().x()/m_CellSize.width(), dotRect.topLeft().y()/m_CellSize.height()));
    corners.append(QPoint(dotRect.topRight().x()/m_CellSize.width(), dotRect.topRight().y()/m_CellSize.height()));
    corners.append(QPoint(dotRect.bottomLeft().x()/m_CellSize.width(), dotRect.bottomLeft().y()/m_CellSize.height()));
    corners.append(QPoint(dotRect.bottomRight().x()/m_CellSize.width(), dotRect.bottomRight().y()/m_CellSize.height()));

    QSet<QPoint> unique = QSet<QPoint>(corners.begin(),corners.end());
    for( QPoint pt: unique ){
        if ( pt.x() < 0 || pt.x() >= m_GridSize.x() ){
            continue;
        }
        if ( pt.y() < 0 || pt.y() >= m_GridSize.y() ){
            continue;
        }
        cells.append(QString("%0.%1").arg(pt.x()).arg(pt.y()));
    }

//#if defined(QT_DEBUG)
//    for( QString str: cells ){
//        if ( str[0] == QString("4") || str[2] == QString("4") )
//            asm("int $3");
//    }
//    Q_ASSERT(cells.size() < 5);
//#endif

    return cells;
}


QString CSpatialManager::getCellFromPoint(QPoint Point){
    unsigned colGuess = Point.x() / m_CellSize.width();
    unsigned rowGuess = Point.y() / m_CellSize.height();

    QString lookup = QString("%0.%1").arg(colGuess).arg(rowGuess);

    return lookup;
}

QRect CSpatialManager::getCenterPointCell(PTDot Dot){
    unsigned colGuess = Dot->m_Position.x() / m_CellSize.width();
    unsigned rowGuess = Dot->m_Position.y() / m_CellSize.height();

    QString lookup = QString("%0.%1").arg(colGuess).arg(rowGuess);

    return m_Cells[lookup];
}


QRect CSpatialManager::getDotCellsAsRect(PTDot Dot){
    int top = 999999;
    int bottom = 0;
    int left = 999999;
    int right = 0;
    QVector<QString> lookups = getTouchedCellsLookup(Dot);

    for( QString lookup: lookups ){
        QRect rect;
#if defined(QT_DEBUG)
        if (m_Cells.contains(lookup))
            rect = m_Cells[lookup];
        else
            asm("int $3");
#else
        rect = m_Cells[lookup];
#endif

        top = qMin(top,rect.top());
        left = qMin(left,rect.left());
        bottom = qMax(bottom,rect.bottom());
        right = qMax(right,rect.right());
    }

    Q_ASSERT(right-left < m_CellSize.width()*3);
    Q_ASSERT(bottom-top < m_CellSize.height()*3);

    return QRect(left,top,right-left,bottom-top);
}

QVector<QString> CSpatialManager::getCellLookupsFromDot(PTDot Dot){
    QVector<QString> lookups;

    for( int lookup = 0; lookup < Dot->m_Cells; lookup++ ){
        lookups.append(QString::fromWCharArray(Dot->m_CellLookups[lookup],3));
    }

    return lookups;
}


void CSpatialManager::saveCellLookupsToDot(QVector<QString> Lookups, PTDot Dot){
    int count = 0;

    Dot->m_Cells = Lookups.size();

    for ( QString lookup: Lookups ){
        lookup.toWCharArray(Dot->m_CellLookups[count]);
        count++;
    }
}


