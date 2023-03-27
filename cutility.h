#ifndef CUTILITY_H
#define CUTILITY_H

#pragma once

#include <string>
#include <map>
#include <list>

#include <QColor>
#include <qpoint.h>

enum class eColor
{
    black=0x0,
    red=0x1,
    orange,
    yellow,
    green,
    blue,
    purple,
    white
};

typedef struct _Color
{
    std::string m_Name;
    eColor m_Color;
    QColor m_QtColor;
}SColor;

class CUtility
{
public:
    CUtility();

    static inline ulong getCrossProduct(QPoint ptA, QPoint ptB)
    {
        float fXProduct = sqrt(((ptA.x() - ptB.x()) * (ptA.x() - ptB.x())) + ((ptA.y() - ptB.y()) * (ptA.y() - ptB.y())));

        return (ulong)fXProduct;
    }

    static std::map<eColor,SColor> m_ColorDict;
    static std::list<eColor> m_ColorDictKeys;
};

#endif // CUTILITY_H
