#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>
#include <math.h>

#include <QObject>
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopServices>

#include <QSlider>

void MainWindow::CreateDots(uint Count, uint Size)
{
    /***** TODO: Optimize Cleanup Code *********/
    this->RenderTimer.stop();
    ui->oglDots->clearDots();
    /************ End of Cleanup Code **********/

    QSize oglBoundary = ui->oglDots->geometry().size();
    // Account for radius
    oglBoundary = QSize(oglBoundary.width() - Size, oglBoundary.height() - Size);

    for ( uint udot = 0; udot < Count; udot++ )
    {
        PTDot dot = new TDot;
        dot->setRandomPosition(oglBoundary);
        dot->setRandomDirection();
        dot->setRandomColor();
        //int rSize = (int)((float)Size * (5.0f/(float)(rand()%20+1)));
        int rSize = Size;
        dot->setBaseSize(rSize);
        dot->m_Id = udot;

        ui->oglDots->addDot(dot);
    }

    RenderTimer.start(33);
}

uint g_Collisions = 0;
QEvent resizeEvent = QEvent(QEvent::Resize);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnNextBackColor->connect(ui->btnNextBackColor,&QPushButton::clicked,[this]()
    {
        static Qt::GlobalColor color = Qt::black;

        QColor newColor = Qt::transparent;
        QColor nextColor = Qt::transparent;

        switch (color)
        {
            case Qt::black:
                color = Qt::white;
                newColor = Qt::white;
                nextColor = Qt::cyan;
            break;
            case Qt::white:
                color = Qt::cyan;
                newColor = Qt::cyan;
                nextColor = Qt::darkCyan;
            break;
            case Qt::cyan:
                color = Qt::darkCyan;
                newColor = Qt::darkCyan;
                nextColor = Qt::magenta;
            break;
            case Qt::darkCyan:
                color = Qt::magenta;
                newColor = Qt::magenta;
                nextColor = Qt::darkMagenta;
            break;
            case Qt::magenta:
                color = Qt::darkMagenta;
                newColor = Qt::darkMagenta;
                nextColor = Qt::lightGray;
            break;
            case Qt::darkMagenta:
                color = Qt::lightGray;
                newColor = Qt::lightGray;
                nextColor = Qt::darkGray;
            break;
            case Qt::lightGray:
                color = Qt::darkGray;
                newColor = Qt::darkGray;
                nextColor = Qt::black;
            break;
            case Qt::darkGray:
                color = Qt::black;
                newColor = Qt::black;
                nextColor = Qt::white;
            break;
            default:
                color = Qt::black;
                newColor = Qt::black;
                nextColor = Qt::white;
            break;
        }

        ui->oglDots->setClearColor(newColor);
    });

    ui->sldrDots->connect(ui->sldrDots,&QSlider::valueChanged,[this](int value)
    {
        CreateDots((uint)value, (uint)(ui->sldrDotSize->value()));
    });

    ui->sldrDotSize->connect(ui->sldrDotSize,&QSlider::valueChanged,[this](int value)
    {
        //CreateDots((uint)(ui->sldrDots->value()), (uint)value);
        ui->oglDots->setDotsSize(value);
    });

    // Setup the click capture area, trying to select the dots seems to lag
    //  in comparison the the frame output so have to decide where the user
    //  is in fact trying to click to help make the game a more enjoyable
    //  experience (and allows for better control... <crosses fingers/>)

    ui->oglDots->show();
    ui->oglDots->raise();

    // Setup the reset button for more pleasure
    ui->btnReset->connect(ui->btnReset, &QPushButton::clicked, [this]()
    {
        CreateDots((uint)(ui->sldrDots->value()), (uint)(ui->sldrDotSize->value()));
    });

    RenderTimer.setInterval(33);
    RenderTimer.connect(&RenderTimer, &QTimer::timeout, [this]()
    {
        ui->oglDots->updateDots();
        ui->oglDots->update();

        ui->lblNumberOfDots->setText(std::to_string(ui->oglDots->getDotCount()).c_str());

        if ( ui->oglDots->getCollisionCount() == 0 && ui->oglDots->getLastCollisionCount() > 0 )
        {
            int CollisionCount = ui->oglDots->getLastCollisionCount();
            QString strYouCaused = QString(std::to_string(CollisionCount).c_str()) + QString(" Collisions - ");
            if ( CollisionCount < 5 )
                strYouCaused += "Good";
            if ( CollisionCount < 20 && CollisionCount >= 5 )
                strYouCaused += "Nice!";
            if ( CollisionCount < 40 && CollisionCount >= 20 )
                strYouCaused += "Stockpile Those Up!";
            if ( CollisionCount < 60 && CollisionCount >= 40 )
                strYouCaused += "Terrific Collisions!";
            if ( CollisionCount < 80 && CollisionCount >= 60 )
                strYouCaused += "Wow, nice click!";
            if ( CollisionCount < 100 && g_Collisions >= 80 )
                strYouCaused += "HOLY COW!";
            if ( CollisionCount >= 100  )
                strYouCaused += "Collision Masta!";

            ui->lblMessage->setText(strYouCaused);
        }

    });//end of timer loop

    ui->btnQuit->connect(ui->btnQuit, &QPushButton::clicked, qApp, &QCoreApplication::quit);


    // Start the timer with the awesome lambda inside :-)
    //RenderTimer.start(33);
}


void MainWindow::showEvent(QShowEvent *event) {
    // Create the initial set of dots, doing this in the showEvent
    //  ensures the geometry is sized by the layout manager first
    CreateDots((uint)(ui->sldrDots->value()), (uint)(ui->sldrDotSize->value()));
    QWidget::showEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}


