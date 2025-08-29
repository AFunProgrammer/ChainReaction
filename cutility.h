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

#ifndef CUTILITY_H
#define CUTILITY_H

#pragma once

#include <list>
#include <map>
#include <string>
#include <tuple>

#include <QColor>
#include <QPoint>

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

struct t_CharID {
    char m_CharID[3] = {}; // Default initialize the array

    // Default constructors
    explicit t_CharID(char c1 = '\0', char c2 = '\0', char c3 = '\0');
    t_CharID(int Col, int Row);

    // Copy and move constructors
    t_CharID(const t_CharID& other);
    t_CharID(t_CharID&& other) noexcept;

    // Constructor that takes a std::tuple<int, int>
    t_CharID(const std::tuple<int, int>& rhs);

    // Assignment operators
    t_CharID& operator=(const t_CharID& rhs);
    t_CharID& operator=(const std::tuple<int, int>& rhs);

    // Comparison operators
    bool operator==(const t_CharID& rhs) const;
    bool operator!=(const t_CharID& rhs) const;
};

using TCharID = t_CharID;
using PTCharID = t_CharID*;

struct t_Color
{
    std::string m_Name;
    eColor m_Color;
    QColor m_QtColor;
};

using SColor = t_Color;

class CUtility
{
public:
    CUtility(){}

    static inline ulong getCrossProduct(QPoint ptA, QPoint ptB)
    {
        float fXProduct = sqrt(((ptA.x() - ptB.x()) * (ptA.x() - ptB.x())) + ((ptA.y() - ptB.y()) * (ptA.y() - ptB.y())));

        return (ulong)fXProduct;
    }

    static std::map<eColor,SColor> m_ColorDict;
    static std::list<eColor> m_ColorDictKeys;
};

#endif // CUTILITY_H
