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

#ifndef CDOTSWIDGET_H
#define CDOTSWIDGET_H

#pragma once
#include <QPixmap>
#include <QtSvg/QtSvg>
#include <QPoint>
#include <QObject>
#include <QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "cdotsmanager.h"


class CDotsWidget : public QOpenGLWidget
{
public:
    explicit CDotsWidget(QWidget *parent = nullptr);
    ~CDotsWidget(){}

private:
    QColor m_ClearColor = Qt::black;
    QRect m_Rect = QRect(0,0,0,0);

    CDotsManager* m_pDotsMgr = CDotsManager::getGlobalInstance();
public:
    void setClearColor(QColor Color);

    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
signals:
};


#endif // CDOTSWIDGET_H
