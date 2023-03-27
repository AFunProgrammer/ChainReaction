#include "cdotsmanager.h"

#include <QPainter>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>

bool CDotsManager::m_ClickLock = false;

CDotsManager::CDotsManager(QWidget *parent) : QOpenGLWidget{parent}
{
    setAutoFillBackground(false);

    createDotPixmaps();
}

void CDotsManager::setSVGFile(QString SvgFilePath)
{
    m_SvgFilePath = SvgFilePath;

    QFile svgImageFile = QFile(SvgFilePath);
    svgImageFile.open(QIODeviceBase::ReadOnly);
    m_SvgFile = QString(svgImageFile.readAll());
}

QString CDotsManager::getSVGFilePath()
{
    return m_SvgFilePath;
}

void CDotsManager::setClearColor(QColor Color)
{
    m_ClearColor = Color;
}


void CDotsManager::setBounds(QRect Rect)
{
    setGeometry(Rect);

    for(CDot* dot: m_Dots)
    {
        setDotDrawBoundary(dot);
    }
}

QPixmap CDotsManager::getSVGPixmap(eColor Color, QPointF Scale, uint Size)
{
    QPixmap svgToPixmap;
    QString svgColorSizeChange = m_SvgFile;

    switch(Color)
    {
    case eColor::yellow:
        svgColorSizeChange.replace("red","yellow");
        break;
    case eColor::orange:
        svgColorSizeChange.replace("red","orange");
        break;
    case eColor::green:
        svgColorSizeChange.replace("red","green");
        break;
    case eColor::blue:
        svgColorSizeChange.replace("red","blue");
        break;
    case eColor::purple:
        svgColorSizeChange.replace("red","purple");
        break;
    default:
        break;
    }

    svgColorSizeChange.replace(" 12", QString(" ") + QString(std::to_string(Size*2).c_str()));
    svgColorSizeChange.replace("\"6", QString("\"") + QString(std::to_string(Size).c_str()));

    QByteArray baSvgColorChanged = QByteArray(svgColorSizeChange.toUtf8());
    m_SvgRenderer.load(baSvgColorChanged);

    QPoint pixmapMaxSize( m_SvgRenderer.defaultSize().width()*2, m_SvgRenderer.defaultSize().height()*2 );
    svgToPixmap = QPixmap(pixmapMaxSize.x(), pixmapMaxSize.y());

    svgToPixmap.fill(QColor::fromRgba(0x00000000));

    QPainter paintPixmap(&svgToPixmap);

    m_SvgRenderer.render(&paintPixmap, svgToPixmap.rect());

    return svgToPixmap;
}

void CDotsManager::createDotPixmaps()
{
    for(eColor color: CUtility::m_ColorDictKeys)
    {
        QImage dotImage(200,200, QImage::Format_ARGB32);
        QPainter dotPainter(&dotImage);

        QPen dotPen = QPen(QColor::fromRgba(0x99000000), 1, Qt::SolidLine);
        QBrush dotBrush = QBrush(CUtility::m_ColorDict[color].m_QtColor, Qt::SolidPattern);

        dotPainter.setRenderHint(QPainter::Antialiasing);
        dotPainter.setRenderHint(QPainter::LosslessImageRendering);

        dotImage.fill(QColor::fromRgba(0x00000000));

        dotPainter.begin(&dotImage);
        dotPainter.setPen(dotPen);
        dotPainter.setBrush(dotBrush);
        dotPainter.drawEllipse(0,0,200,200);
        dotPainter.end();

        QPixmap dotPixmap(200,200);
        dotPixmap.convertFromImage(dotImage);

        this->m_DotPixmaps.insert(std::pair<eColor,QPixmap>(color,dotPixmap));
    }
}

void CDotsManager::setDotPixmap(CDot* Dot)
{
    uint size = Dot->getSize();

    Dot->setScale(1.0f);
    Dot->setScaleMax(3.0f);

    Dot->setPixmap(&m_DotPixmaps[Dot->getColor()]);
}

void CDotsManager::setDotDrawBoundary(CDot* Dot)
{
    //QRect rectBounds = geometry();
    QRect rectBounds = QRect(0,0,geometry().width(),geometry().height());
    Dot->setBounds(rectBounds);
}

void CDotsManager::setDotSize(CDot* Dot)
{
    setDotPixmap(Dot);
}


QList<CDot*> CDotsManager::getDots()
{
    return m_Dots;
}

void CDotsManager::addDot(CDot* Dot)
{
    if ( Dot != nullptr )
    {
        setDotPixmap(Dot);
        setDotDrawBoundary(Dot);
        m_Dots.append(Dot);
        m_RemovedDots.resize(m_Dots.count());
    }
}

void CDotsManager::delDot(int Index)
{
    if ( m_Dots.count() > Index )
    {
        delete m_Dots[Index];
        m_Dots.removeAt(Index);
    }
}

uint CDotsManager::getDotCount()
{
    return m_Dots.count();
}

void CDotsManager::clearDots()
{
    for(CDot* dot: m_Dots)
    {
        delete dot;
    }

    for(CDot* dot: m_RemovedDots)
    {
        delete dot;
    }

    m_Dots.clear();
    m_RemovedDots.clear();
}

void CDotsManager::setDotsSize(uint Size)
{
    m_UpdateDotsSize = Size;
}


void CDotsManager::updateDots()
{
    QList<CDot*> resizingDots;

    uint uDots = m_Dots.count();

    for(uint uDot = 0; uDot < uDots; uDot++ )
    {
        if ( m_Dots[uDot]->isStopped() )
        {
            m_RemovedDots.append(m_Dots[uDot]);
            m_Dots.removeAt(uDot);
            uDots--;
            uDot--;
        }
        else
        {
            m_Dots[uDot]->update();
            if ( m_Dots[uDot]->isResizing() )
            {
                //setDotPixmap(m_Dots[uDot]);
                resizingDots.append(m_Dots[uDot]);
            }
        }
    }

    if ( m_UpdateDotsSize > 0 )
    {
        for(CDot* dot: m_Dots)
        {
            dot->setSize(m_UpdateDotsSize);
        }
    }

    checkForCollisions(resizingDots);
}

void CDotsManager::drawDots(QPainter* Painter)
{
    //QBrush colorBrush = QBrush(Qt::red);
    //Painter->setPen(QPen(m_ClearColor,1,Qt::PenStyle::SolidLine));
    //Painter->setBrush(colorBrush);
    for( CDot* dot: m_Dots )
    {
        //colorBrush.setColor(dot->getColor());
        //Painter->drawEllipse(dot->getDrawRect());
        Painter->drawPixmap(dot->getDrawRect(), *(dot->getPixmap()));

        //QImage image = dot->getPixmap()->toImage();
        //QString format = QString("bpp: ") + std::to_string(image.pixelFormat().bitsPerPixel()).c_str() + " format: " + std::to_string(image.format()).c_str();
        //qDebug() << "Draw Rect: " << dot->getDrawRect() << " format: " << format;

    }
}

void CDotsManager::checkForCollisions(QList<CDot*> resizingDots)
{
    if ( resizingDots.count() > 0 )
    {
        for(CDot* dot: resizingDots)
        {
            for(CDot* cmp: m_Dots)
            {
                if ( dot->getId() != cmp->getId() && cmp->isResizing() == false )
                {
                    uint uiCrossProduct = CUtility::getCrossProduct(dot->getPos(), cmp->getPos());
                    //float fXProduct = sqrt(pow((dot->getPos().x() - cmp->getPos().x()),2) +
                    //                       pow((dot->getPos().y() - cmp->getPos().y()),2));

                    uint uiDotDistance = dot->getSize() / 2 + cmp->getSize() / 2;

                    if (uiCrossProduct <= uiDotDistance)
                    {
                        cmp->startResizing();
                        m_Collisions++;
                        //qDebug() << "Dot: " << dot->objectName() << " cross: " << fXProduct << " " << cmp->objectName() << " bounds: " << cmp->getBounds() << " " << dot->objectName() << " bounds: " << dot->getBounds();
                    }
                }
            }
        }
    }
    else //Collisions are reduced to 0
    {
        m_LastCollisionCount = m_Collisions;
        m_Collisions = 0;
    }
}


uint CDotsManager::getCollisionCount()
{
    return m_Collisions;
}

uint CDotsManager::getLastCollisionCount()
{
    return m_LastCollisionCount;
}

void CDotsManager::mousePressEvent(QMouseEvent *event)
{
    QPainter qpMeasure(this);
    QPoint clickPos = event->pos();
    QRect clickArea = QRect(0,0,0,0);

    float fDpcmX = (float)qpMeasure.device()->logicalDpiX() / 2.54f;
    float fDpcmY = (float)qpMeasure.device()->logicalDpiY() / 2.54f;

    //Click area is considered the 'nice' area of being clicked in
    // allowing for 'fat' finger touch to select a specific spot
    // within a region and allowing for making mistakes but
    // still having the pleasure of using touch
    clickArea.setX(clickPos.x() - (int)(fDpcmX/2.0f) );
    clickArea.setY(clickPos.y() - (int)(fDpcmY/2.0f) );
    clickArea.setWidth((int)(fDpcmX) );
    clickArea.setHeight((int)(fDpcmY) );

    unsigned int iDots = m_Dots.count();
    int closestDotToClick = -1;

    for( uint iDot = 0; iDot < iDots; iDot++ )
    {
        if ( clickArea.intersects(m_Dots[iDot]->getDrawRect()) )
        {
            if ( closestDotToClick != -1 )
            {
                ulong uNewDot = CUtility::getCrossProduct(m_Dots[iDot]->getPos(), clickPos);
                ulong uDot = CUtility::getCrossProduct(m_Dots[closestDotToClick]->getPos(), clickPos);
                if ( uNewDot < uDot )
                    closestDotToClick = iDot;
            }
            else
               closestDotToClick = iDot;
        }
    }

    if ( closestDotToClick >= 0 )
        m_Dots[closestDotToClick]->startResizing();

}


void CDotsManager::paintEvent(QPaintEvent *event)
{
    QPainter Painter(this);
    QBrush brushBackground = QBrush(m_ClearColor);

    //Painter.begin(this);
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_LINE_SMOOTH);
    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.setRenderHint(QPainter::SmoothPixmapTransform);
    Painter.fillRect(event->rect(),brushBackground);

    drawDots(&Painter);

    Painter.end();
}


