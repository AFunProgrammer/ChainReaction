#include "cspacialmanager.h"

#include <QtMath>

CSpacialManager::CSpacialManager(QSize WindowSize, QObject *parent)
    : QObject{parent}
{
    CSpacialManager::createManager(WindowSize);
}


void CSpacialManager::createManager(QSize WindowSize){
    bool bPortrait = (WindowSize.height() > WindowSize.width() ? true : false);
    float fRatio = (float)WindowSize.width() / (float)WindowSize.height();
    unsigned maxRect = 4;

    fRatio = (bPortrait ? fRatio : 1/fRatio);

    unsigned cols = (unsigned)qCeil((float)maxRect*(bPortrait ? fRatio : 1));
    unsigned rows = (unsigned)qCeil((float)maxRect*(bPortrait ? 1 : fRatio));

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

void CSpacialManager::updateDot(PTDot Dot){

}

void CSpacialManager::removeDot(PTDot Dot){

}


QVector<PTDot> CSpacialManager::getNearestDots(PTDot Dot){
    QVector<PTDot> nearestDots = QVector<PTDot>();


    return nearestDots;
}


QVector<QString> CSpacialManager::getTouchedCellsLookup(PTDot Dot){
    QVector<QString> cells = QVector<QString>();
    unsigned colGuess = Dot->m_Position.x() / m_CellSize.width();
    unsigned rowGuess = Dot->m_Position.y() / m_CellSize.height();

    QString lookup = QString("%0.%1").arg(colGuess).arg(rowGuess);

    QRect cell = m_Cells[lookup];
    QRect dot = Dot->getDrawRect();

    QRect intersect = cell.intersected(dot);

    int x = dot.left() - intersect.left() + dot.right() - intersect.right();
    int y = dot.top() - intersect.top() + dot.bottom() - intersect.bottom();

    cells.append(lookup);

    // shortcut, nothing else is needed
    if ( x == 0 && y == 0 )
        return cells;

    if ( x < 0 ){
        if ( y < 0 ){
            cells.append(QString("%0.%1").arg(colGuess-1).arg(rowGuess-1));
        }else if ( y > 0 ) {
            cells.append(QString("%0.%1").arg(colGuess-1).arg(rowGuess+1));
        }
        cells.append(QString("%0.%1").arg(colGuess-1).arg(rowGuess));
    }else if ( x > 0 ){
        if ( y < 0 ){
            cells.append(QString("%0.%1").arg(colGuess+1).arg(rowGuess-1));
        }else if ( y > 0 ) {
            cells.append(QString("%0.%1").arg(colGuess+1).arg(rowGuess+1));
        }
        cells.append(QString("%0.%1").arg(colGuess+1).arg(rowGuess));
    }else {
        if ( y < 0 ){
            cells.append(QString("%0.%1").arg(colGuess).arg(rowGuess-1));
        }else if ( y > 0 ){
            cells.append(QString("%0.%1").arg(colGuess).arg(rowGuess+1));
        }
    }

    return cells;
}

QRect CSpacialManager::getCenterPointCell(PTDot Dot){
    unsigned colGuess = Dot->m_Position.x() / m_CellSize.width();
    unsigned rowGuess = Dot->m_Position.y() / m_CellSize.height();

    QString lookup = QString("%0.%1").arg(colGuess).arg(rowGuess);

    return m_Cells[lookup];
}

QRect CSpacialManager::getDotCellsAsRect(PTDot Dot){
    int top = 999999;
    int bottom = 0;
    int left = 999999;
    int right = 0;
    QVector<QString> lookups = getTouchedCellsLookup(Dot);

    for( QString lookup: lookups ){
        QRect rect = m_Cells[lookup];

        top = qMin(top,rect.top());
        left = qMin(left,rect.left());
        bottom = qMax(bottom,rect.bottom());
        right = qMax(right,rect.right());
    }

    return QRect(left,top,right-left,bottom-top);
}

QVector<QString> CSpacialManager::getCellLookupsFromDot(PTDot Dot){
    QVector<QString> lookups;
    for( int lookup = 0; lookup < Dot->m_Cells; lookup++ ){
        lookups.append(QString::fromWCharArray(Dot->m_CellLookups[lookup]));
    }

    return lookups;
}


void CSpacialManager::saveCellLookupsToDot(QVector<QString> Lookups, PTDot Dot){
    int count = 0;
    Dot->m_Cells = Lookups.size();
    for ( QString lookup: Lookups ){
        lookup.toWCharArray(Dot->m_CellLookups[count]);
        count++;
    }
}


