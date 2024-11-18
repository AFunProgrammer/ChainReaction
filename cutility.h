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

typedef struct t_CharID {
    char m_CharID[3] = {}; // Default initialize the array

    // Default constructor
    explicit t_CharID(char c1 = '\0', char c2 = '\0', char c3 = '\0') {
        m_CharID[0] = c1;
        m_CharID[1] = c2;
        m_CharID[2] = c3;
    }

    // Default constructor
    t_CharID(int Col, int Row) {
        m_CharID[0] = '0' + Col;
        m_CharID[1] = '.';
        m_CharID[2] = '0' + Row;
    }

    // Copy constructor (needed because you have a custom assignment operator)
    t_CharID(const t_CharID& other) {
        m_CharID[0] = other.m_CharID[0];
        m_CharID[1] = other.m_CharID[1];
        m_CharID[2] = other.m_CharID[2];
    }

    // Move constructor (needed to support emplace or move operations)
    t_CharID(t_CharID&& other) noexcept {
        m_CharID[0] = std::move(other.m_CharID[0]);
        m_CharID[1] = std::move(other.m_CharID[1]);
        m_CharID[2] = std::move(other.m_CharID[2]);
    }

    // Constructor that takes a std::tuple<int, int>
    inline t_CharID(const std::tuple<int, int>& rhs) {
        m_CharID[0] = '0' + std::get<0>(rhs);  // Convert first element to char
        m_CharID[1] = '.';                      // Dot separator
        m_CharID[2] = '0' + std::get<1>(rhs);  // Convert second element to char
    }

    // Assignment from another t_CharID object
    inline t_CharID& operator=(const t_CharID& rhs) {
        m_CharID[0] = rhs.m_CharID[0];
        m_CharID[1] = rhs.m_CharID[1];
        m_CharID[2] = rhs.m_CharID[2];
        return *this;  // Return the current object (this)
    }

    // Assignment from a tuple<int, int>
    inline t_CharID& operator=(const std::tuple<int, int>& rhs) {
        m_CharID[0] = '0' + std::get<0>(rhs); // Get first item of tuple
        m_CharID[1] = '.';                    // Dot separator
        m_CharID[2] = '0' + std::get<1>(rhs); // Get second item of tuple
        return *this;  // Return the current object (this)
    }

    // Equality comparison operator (operator==)
    inline bool operator==(const t_CharID& rhs) const {
        return m_CharID[0] == rhs.m_CharID[0] &&
               m_CharID[1] == rhs.m_CharID[1] &&
               m_CharID[2] == rhs.m_CharID[2];
    }

    // Inequality comparison operator (operator!=)
    inline bool operator!=(const t_CharID& rhs) const {
        return !(*this == rhs);  // Negate the result of operator==
    }
} TCharID, *PTCharID;


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
