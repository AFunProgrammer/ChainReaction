#include "cdotsmanager.h"

#include <QPainter>
#include <math.h>

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


QPixmap CDotsManager::getSVGPixmap(eColor Color, uint Size = 0)
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

void CDotsManager::setDotPixmap(PTDot Dot)
{
    Dot->m_MaxScale = 3.0f;
    Dot->m_Pixmap = m_DotPixmaps[Dot->m_Color];
}

void CDotsManager::setDotsDrawBoundary()
{
    QSize Bounds = this->geometry().size();

    for(PTDot dot: m_Dots)
    {
        dot->m_BoxBounds = Bounds;
    }
}

void CDotsManager::setDotSize(PTDot Dot)
{
    setDotPixmap(Dot);
}


QList<PTDot> CDotsManager::getDots()
{
    return m_Dots;
}

void CDotsManager::addDot(PTDot Dot)
{
    if ( Dot != nullptr )
    {
        Dot->m_BoxBounds = this->geometry().size();
        setDotPixmap(Dot);
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
    for(PTDot dot: m_Dots)
    {
        delete dot;
    }

    for(PTDot dot: m_RemovedDots)
    {
        delete dot;
    }

    m_Dots.clear();
    m_RemovedDots.clear();
    m_Manager.createManager(geometry().size());
}

void CDotsManager::setDotsSize(uint Size)
{
    m_UpdateDotsSize = Size;
}


void CDotsManager::updateDots()
{
    QList<PTDot> resizingDots;

    uint uDots = m_Dots.count();

    for(uint uDot = 0; uDot < uDots; uDot++ )
    {
        if ( m_Dots[uDot]->m_Exploded )
        {
            m_RemovedDots.append(m_Dots[uDot]);
            m_Manager.removeDot(m_Dots[uDot]);

            m_Dots.removeAt(uDot);

            uDots--;
            uDot--;
        }
        else
        {
            m_Dots[uDot]->update();
            m_Manager.updateDot(m_Dots[uDot]);
            if ( m_Dots[uDot]->m_Touched )
            {
                //setDotPixmap(m_Dots[uDot]);
                resizingDots.append(m_Dots[uDot]);
            }
        }
    }

    if ( m_UpdateDotsSize > 0 )
    {
        for(PTDot dot: m_Dots)
        {
            dot->setBaseSize(m_UpdateDotsSize);
        }

        m_UpdateDotsSize = 0;
    }

    checkForCollisions(resizingDots);
}

void CDotsManager::drawDots(QPainter* Painter)
{
    //QBrush colorBrush = QBrush(Qt::red);
    //Painter->setPen(QPen(m_ClearColor,1,Qt::PenStyle::SolidLine));
    //Painter->setBrush(colorBrush);
    for( PTDot dot: m_Dots )
    {
        //colorBrush.setColor(dot->getColor());
        //Painter->drawEllipse(dot->getDrawRect());
        Painter->drawPixmap(dot->getDrawRect(), dot->m_Pixmap);

        //QImage image = dot->getPixmap()->toImage();
        //QString format = QString("bpp: ") + std::to_string(image.pixelFormat().bitsPerPixel()).c_str() + " format: " + std::to_string(image.format()).c_str();
        //qDebug() << "Draw Rect: " << dot->getDrawRect() << " format: " << format;

    }
}

void CDotsManager::checkForCollisions(QList<PTDot> resizingDots)
{
    if ( resizingDots.count() > 0 )
    {
        for(PTDot dot: resizingDots)
        {
            QVector<PTDot> nearestDots = m_Manager.getNearestDots(dot);

            for( PTDot nearDot: nearestDots ){
                if ( nearDot->m_Touched )
                    continue;

                uint uiCrossProduct = CUtility::getCrossProduct(dot->m_Position, nearDot->m_Position);
                uint uiDotDistance = dot->m_Radius + nearDot->m_Radius;

                if (uiCrossProduct <= uiDotDistance)
                {
                    nearDot->m_Touched = true;
                    m_Collisions++;
                    //qDebug() << "Dot: " << dot << " Near Dot: " << nearDot;
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


void CDotsManager::resizeEvent(QResizeEvent *event){
    QOpenGLWidget::resizeEvent(event);
    setDotsDrawBoundary();
    //re-create manager based upon resized window
    m_Manager.createManager(geometry().size());
}

void CDotsManager::mousePressEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();
    //Click area is considered the 'nice' area of being clicked in
    // allowing for 'fat' finger touch to select a specific spot
    // within a region and allowing for making mistakes but
    // still having the pleasure of using touch
    QRect clickArea = QRect(clickPos.x()-24,clickPos.y()-24,48,48);

    QVector<PTDot> Dots = m_Manager.getNearestDots(clickPos);
    PTDot closest = nullptr;

    for( PTDot dot : Dots )
    {
        if ( clickArea.intersects(dot->getDrawRect()) )
        {
            if ( closest != nullptr )
            {
                ulong uNewDot = CUtility::getCrossProduct(dot->m_Position, clickPos);
                ulong uDot = CUtility::getCrossProduct(closest->m_Position, clickPos);
                if ( uNewDot < uDot )
                    closest = dot;
            }
            else
               closest = dot;
        }
    }

    if ( closest != nullptr)
        closest->m_Touched = true;

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

#if defined(QT_DEBUG)
    if ( m_Dots.size() > 0 ){
        QRect cell = m_Manager.getCenterPointCell(m_Dots[0]);
        QRect cells = m_Manager.getDotCellsAsRect(m_Dots[0]);
        QRect dot = m_Dots[0]->getDrawRect();
        QRect intersection = cell.intersected(dot);

        Painter.setPen(Qt::red);
        Painter.drawRect(cells);
        Painter.setPen(Qt::white);
        Painter.drawRect(cell);
        Painter.setPen(Qt::white);
        Painter.drawRect(dot);
        Painter.setPen(Qt::yellow);
        Painter.setBrush(Qt::darkYellow);
        Painter.drawRect(intersection);
    }
#endif

    Painter.end();
}


