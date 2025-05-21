#include "cdotswidget.h"


CDotsWidget::CDotsWidget(QWidget *parent) : QOpenGLWidget{parent} {
    setAutoFillBackground(false);
}

void CDotsWidget::setClearColor(QColor Color){
    m_ClearColor = Color;
}

void CDotsWidget::resizeEvent(QResizeEvent *event){
    QOpenGLWidget::resizeEvent(event);
    //update dots bounding box in update function
    m_pDotsMgr->setDotsBounds(geometry().size());
    //qDebug() << "Widget Logical Size:" << geometry().size();
    //qDebug() << "Widget Device Pixel Ratio:" << devicePixelRatio();
    //qDebug() << "Calculated Physical Size:" << geometry().width() * devicePixelRatio() << "x" << geometry().height() * devicePixelRatio();

}

void CDotsWidget::mousePressEvent(QMouseEvent *event)
{
    m_pDotsMgr->doCollisionCheck(event->pos());
}


void CDotsWidget::paintEvent(QPaintEvent *event)
{
    QPainter Painter(this); // Painter is now painting onto 'this' (the QOpenGLWidget)
    //
    // The 'device' property of the painter is the QOpenGLPaintDevice
    // that QOpenGLWidget created for it.
    //const QPaintDevice* paintDevice = Painter.device();
    //
    //if (paintDevice) {
    //    qDebug() << "QPainter Device Logical Width:" << paintDevice->width();
    //    qDebug() << "QPainter Device Logical Height:" << paintDevice->height();
    //    // QPaintDevice::width/height are *logical* if painting on a QWidget that scales
    //    // However, with QOpenGLWidget, they might reflect the internal render target's logical size.
    //    // You really want the *physical* size being drawn to.
    //    // This is harder to get directly from QPainter(this).
    //}

    QBrush brushBackground = QBrush(m_ClearColor);
    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.setRenderHint(QPainter::SmoothPixmapTransform);
    Painter.fillRect(event->rect(),brushBackground); // Fills logical rect, painter scales it

    m_pDotsMgr->drawDotsUsingPaintDevice(&Painter);

    Painter.end();
}


