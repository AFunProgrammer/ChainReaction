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
}

void CDotsWidget::mousePressEvent(QMouseEvent *event)
{
    m_pDotsMgr->doCollisionCheck(event->pos());
}


void CDotsWidget::paintEvent(QPaintEvent *event)
{
    QPainter Painter(this);
    QBrush brushBackground = QBrush(m_ClearColor);

    //Painter.begin(this);
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_LINE_SMOOTH);
    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.setRenderHint(QPainter::SmoothPixmapTransform);
    Painter.fillRect(event->rect(),brushBackground);

    m_pDotsMgr->drawDotsUsingPaintDevice(&Painter);

    Painter.end();
}


