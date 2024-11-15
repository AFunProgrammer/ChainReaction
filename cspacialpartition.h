#ifndef CSPACIALPARTITION_H
#define CSPACIALPARTITION_H

#pragma once
#include <QHash>
#include <QMap>
#include <QObject>

#include "tdot.h"

typedef QHash<QString,QMap<QString,PTDot>> DotSpacialPartition;

class CSpacialPartition
{
public:
    CSpacialPartition();
};

#endif // CSPACIALPARTITION_H
