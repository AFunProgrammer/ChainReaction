#include "cutility.h"

t_CharID::t_CharID(char c1, char c2, char c3) {
    m_CharID[0] = c1;
    m_CharID[1] = c2;
    m_CharID[2] = c3;
}

t_CharID::t_CharID(int Col, int Row) {
    m_CharID[0] = '0' + Col;
    m_CharID[1] = '.';
    m_CharID[2] = '0' + Row;
}

t_CharID::t_CharID(const t_CharID& other) {
    m_CharID[0] = other.m_CharID[0];
    m_CharID[1] = other.m_CharID[1];
    m_CharID[2] = other.m_CharID[2];
}

t_CharID::t_CharID(t_CharID&& other) noexcept {
    m_CharID[0] = std::move(other.m_CharID[0]);
    m_CharID[1] = std::move(other.m_CharID[1]);
    m_CharID[2] = std::move(other.m_CharID[2]);
}

t_CharID::t_CharID(const std::tuple<int, int>& rhs) {
    m_CharID[0] = '0' + std::get<0>(rhs);  // Convert first element to char
    m_CharID[1] = '.';                    // Dot separator
    m_CharID[2] = '0' + std::get<1>(rhs); // Convert second element to char
}

t_CharID& t_CharID::operator=(const t_CharID& rhs) {
    m_CharID[0] = rhs.m_CharID[0];
    m_CharID[1] = rhs.m_CharID[1];
    m_CharID[2] = rhs.m_CharID[2];
    return *this;
}

t_CharID& t_CharID::operator=(const std::tuple<int, int>& rhs) {
    m_CharID[0] = '0' + std::get<0>(rhs);
    m_CharID[1] = '.';
    m_CharID[2] = '0' + std::get<1>(rhs);
    return *this;
}

bool t_CharID::operator==(const t_CharID& rhs) const {
    return m_CharID[0] == rhs.m_CharID[0] &&
           m_CharID[1] == rhs.m_CharID[1] &&
           m_CharID[2] == rhs.m_CharID[2];
}

bool t_CharID::operator!=(const t_CharID& rhs) const {
    return !(*this == rhs);
}

std::list<eColor> CUtility::m_ColorDictKeys = std::list<eColor>(
{
    eColor::black,
    eColor::red,
    eColor::yellow,
    eColor::orange,
    eColor::green,
    eColor::blue,
    eColor::purple,
    eColor::white
});

std::map<eColor,SColor> CUtility::m_ColorDict = std::map<eColor,SColor>(
{
    {eColor::black,         {"black",        eColor::black,        QColor::fromRgba(0x99000000)}},
    {eColor::red,           {"red",          eColor::red,          QColor::fromRgba(0x99FF0000)}},
    {eColor::yellow,        {"yellow",       eColor::yellow,       QColor::fromRgba(0x99FFFF00)}},
    {eColor::orange,        {"orange",       eColor::orange,       QColor::fromRgba(0x99FF8000)}},
    {eColor::green,         {"green",        eColor::green,        QColor::fromRgba(0x9900FF00)}},
    {eColor::blue,          {"blue",         eColor::blue,         QColor::fromRgba(0x990000FF)}},
    {eColor::purple,        {"purple",       eColor::purple,       QColor::fromRgba(0x99FF00FF)}},
    {eColor::white,         {"white",        eColor::white,        QColor::fromRgba(0x99FFFFFF)}}
});
