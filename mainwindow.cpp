#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdlib.h>
#include <math.h>

#include <QObject>
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopServices>

void MainWindow::CreateDots(uint Count, uint Size)
{
    /***** TODO: Optimize Cleanup Code *********/
    this->RenderTimer.stop();
    //QThread::msleep(100); //wait for timer to finish processing

    ui->oglDots->clearDots();
    QSize oglDotsSize = ui->oglDots->geometry().size();

    //QThread::msleep(10);
    /************ End of Cleanup Code **********/
    QPoint ptExtent = QPoint(oglDotsSize.width()-(Size/2), oglDotsSize.height()-(Size/2));

    for ( uint udot = 0; udot < Count; udot++ )
    {
        CDot* dot = new CDot();
        dot->setPos(QPoint(rand()%ptExtent.x(),rand()%ptExtent.y()));
        dot->setMovement((CDot::Movement)(rand()%2), (CDot::Movement)(rand()%2));
        dot->setSpeed(QPoint(rand()%15 - 5, rand()%15 - 5));
        dot->setColor((eColor)(rand()%6+1));
        dot->setSize(Size);
        dot->setId(udot);
        //qDebug() << "Dot#" << dot->getId() << " Random Speed: " << dot->getSpeed() << " Random Color: " << (int)dot->getColor();
        //qDebug() << "Dot#" << dot->getId() << ": Speed: " << Speed << " Dir: " << (uint)dot->getXDirection() << "," << (uint)dot->getYDirection() << " Pos: " << dot->getLocation();

        ui->oglDots->addDot(dot);
    }

    //qDebug() << "Number of Dots: " << _ListOfDots.count();
    //_ClickCap.raise();
    //_ClickCap.show();
    //_ClickCap.update();
    RenderTimer.start(33);
}

uint g_Collisions = 0;
QEvent resizeEvent = QEvent(QEvent::Resize);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QEvent resizeEvent = QEvent(QEvent::Resize);

    QPalette appPalette = this->palette();
    appPalette.setColor(QPalette::Base, Qt::black);
    appPalette.setColor(QPalette::Window, Qt::black);
    this->setPalette(appPalette);

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
        QPalette appPalette = ui->btnNextBackColor->palette();
        appPalette.setColor(QPalette::Button, nextColor);
        appPalette.setColor(QPalette::ColorRole::Window, nextColor);
        appPalette.setColor(QPalette::ColorRole::AlternateBase, nextColor);
        appPalette.setColor(QPalette::ColorRole::Light, nextColor);
        appPalette.setColor(QPalette::ColorRole::Dark, nextColor);
        appPalette.setColor(QPalette::ColorRole::Mid, nextColor);
        appPalette.setColor(QPalette::ColorRole::Base, nextColor);

        if ( nextColor == Qt::black )
            appPalette.setColor(QPalette::ButtonText, Qt::white);
        else
            appPalette.setColor(QPalette::ButtonText, Qt::black);

        appPalette.setCurrentColorGroup(QPalette::ColorGroup::All);
        ui->btnNextBackColor->setPalette(appPalette);
        ui->btnNextBackColor->setAutoFillBackground(true);
        ui->btnNextBackColor->update();

    });

    ui->sldrDots->connect(ui->sldrDots,&QScrollBar::valueChanged,[this](int value)
    {
        CreateDots((uint)value, (uint)(ui->sldrDotSize->value()));
    });

    ui->sldrDotSize->connect(ui->sldrDotSize,&QScrollBar::valueChanged,[this](int value)
    {
        //CreateDots((uint)(ui->sldrDots->value()), (uint)value);
        ui->oglDots->setDotsSize(value);
    });

    // Setup the click capture area, trying to select the dots seems to lag
    //  in comparison the the frame output so have to decide where the user
    //  is in fact trying to click to help make the game a more enjoyable
    //  experience (and allows for better control... <crosses fingers/>)

    //ui->oglDots->setParent(ui->frmDrawBox);
    //ui->oglDots->setBounds(ui->frmDrawBox->geometry().size());
    //ui->oglDots->setLayout(ui->gridLayout);
    ui->oglDots->setSVGFile(":/images/circle.svg");
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

    // Create the initial set of dots
    CreateDots((uint)(ui->sldrDots->value()), (uint)(ui->sldrDotSize->value()));

    // Start the timer with the awesome lambda inside :-)
    RenderTimer.start(33);
}

MainWindow::~MainWindow()
{
    delete ui;
}


