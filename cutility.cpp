#include "cutility.h"

CUtility::CUtility()
{

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

/*
double inline __fastcall fsqrt(double n)
{
    _asm fld qword ptr[esp + 4]
    _asm fsqrt
    _asm ret 8
}
*/
