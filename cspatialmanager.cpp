#include "cspatialmanager.h"

#include <QSet>
#include <QtMath>


// Define qHash for t_CharID without using QString
inline uint qHash(const t_CharID& key, uint seed = 0) {
    // Start with the initial seed
    uint hash = seed;

    // Simple hash computation based on m_CellLookup array
    for (int i = 0; i < 3; ++i) {
        hash = hash * 31 + key.m_CharID[i]; // Using a small prime (31) for hashing
    }

    return hash;
}


CSpatialManager::CSpatialManager(QSize WindowSize, QObject *parent)
    : QObject{parent}
{
    CSpatialManager::createManager(WindowSize);
}


void CSpatialManager::createManager(QSize WindowSize){
    bool bPortrait = (WindowSize.height() > WindowSize.width() ? true : false);
    float fRatio = (float)WindowSize.width() / (float)WindowSize.height();
    unsigned maxRect = 5;

    fRatio = (bPortrait ? fRatio : 1/fRatio);

    unsigned cols = (unsigned)qCeil((float)maxRect*(bPortrait ? fRatio : 1));
    unsigned rows = (unsigned)qCeil((float)maxRect*(bPortrait ? 1 : fRatio));

    m_WindowSize = WindowSize;
    m_CellSize = QSize(WindowSize.width() / cols, WindowSize.height() / rows);
    m_GridSize = QPoint(cols,rows);

    m_Space.clear();
    m_Space.reserve(cols*rows);

    for( unsigned iCol=0; iCol < cols; iCol++ ){
        for( unsigned iRow=0; iRow < rows; iRow++ ){
            TCharID lookup = std::make_tuple<int,int>(iCol,iRow);

            m_Space[lookup] = QHash<int,PTDot>();
            m_Cells[lookup] = QRect(iCol * m_CellSize.width(),
                                    iRow * m_CellSize.height(),
                                    m_CellSize.width(),
                                    m_CellSize.height());
        }
    }
}

void CSpatialManager::updateDot(PTDot Dot){
    TCharID cell = getCellFromPoint(Dot);
    TCharID lastCell = Dot->m_CellLookup;

    if ( cell != lastCell ){
        Dot->m_CellLookup = cell;
        m_Space[lastCell].remove(Dot->m_Id);
        m_Space[cell][Dot->m_Id] = Dot;
    }
}

void CSpatialManager::removeDot(PTDot Dot){
    TCharID cell = Dot->m_CellLookup;

    m_Space[cell].remove(Dot->m_Id);
}


QVector<PTDot> CSpatialManager::getNearestDots(PTDot Dot){
    QVector<PTDot> nearestDots = QVector<PTDot>();
    QVector<TCharID> cells = getCellsFromBB(Dot);

    for( TCharID cellLookup: cells ){
        nearestDots.append(m_Space[cellLookup].values());
    }

    return nearestDots;
}

QVector<PTDot> CSpatialManager::getNearestDots(QPoint Point){
    QVector<PTDot> nearestDots = QVector<PTDot>();
    TCharID cellLookup = getCellFromPoint(Point);

    nearestDots.append(m_Space[cellLookup].values());

    return nearestDots;
}


QVector<TCharID> CSpatialManager::getCellsFromBB(PTDot Dot){
    QVector<TCharID> cells = QVector<TCharID>();
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
        cells.append(std::make_tuple<int,int>(pt.x(),pt.y()));
    }

    return cells;
}


TCharID CSpatialManager::getCellFromPoint(PTDot Dot){
    unsigned colGuess = Dot->m_Position.x() / m_CellSize.width();
    unsigned rowGuess = Dot->m_Position.y() / m_CellSize.height();

    TCharID lookup = std::make_tuple<int,int>(colGuess,rowGuess);

    return lookup;
}

TCharID CSpatialManager::getCellFromPoint(QPoint Point){
    unsigned colGuess = Point.x() / m_CellSize.width();
    unsigned rowGuess = Point.y() / m_CellSize.height();

    TCharID lookup = std::make_tuple<int,int>(colGuess,rowGuess);

    return lookup;
}

QRect CSpatialManager::getCenterPointCell(PTDot Dot){
    unsigned colGuess = Dot->m_Position.x() / m_CellSize.width();
    unsigned rowGuess = Dot->m_Position.y() / m_CellSize.height();

    TCharID lookup = std::make_tuple<int,int>(colGuess,rowGuess);

    return m_Cells[lookup];
}


QRect CSpatialManager::getDotCellsAsRect(PTDot Dot){
    int top = 999999;
    int bottom = 0;
    int left = 999999;
    int right = 0;
    QVector<TCharID> lookups = getCellsFromBB(Dot);

    for( TCharID lookup: lookups ){
        QRect rect;
#if defined(QT_DEBUG)
        if (m_Cells.contains(lookup))
            rect = m_Cells[lookup];
        else
            qDebug() << "Do a Breakpoint Here If You See This Message";
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

