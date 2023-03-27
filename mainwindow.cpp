#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <typeinfo>

#include <QObject>
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopServices>

void MainWindow::CreateDots(uint Count, uint Size)
{
    /***** TODO: Optimize Cleanup Code *********/
    this->RenderTimer.stop();
    //QThread::msleep(100); //wait for timer to finish processing

    _DotsManager.clearDots();

    //QThread::msleep(10);
    /************ End of Cleanup Code **********/
    QPoint ptExtent = QPoint(_DotsManager.geometry().width()-(Size/2), _DotsManager.geometry().height()-(Size/2));

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

        _DotsManager.addDot(dot);
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

        _DotsManager.setClearColor(newColor);
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
        _DotsManager.setDotsSize(value);
    });

    // CClickCap ... awesome class
    // Setup the click capture area, trying to select the dots seems to lag
    //  in comparison the the frame output so have to decide where the user
    //  is in fact trying to click to help make the game a more enjoyable
    //  experience (and allows for better control... <crosses fingers/>)

    _DotsManager.setParent(ui->centralwidget);
    _DotsManager.setBounds(ui->frmDrawBox->geometry());
    _DotsManager.setSVGFile(":/images/circle.svg");
    _DotsManager.show();
    _DotsManager.raise();

    // Setup the reset button for more pleasure
    ui->btnReset->connect(ui->btnReset, &QPushButton::clicked, [this]()
    {
        CreateDots((uint)(ui->sldrDots->value()), (uint)(ui->sldrDotSize->value()));
    });

    RenderTimer.setInterval(33);
    RenderTimer.connect(&RenderTimer, &QTimer::timeout, [this]()
    {
        _DotsManager.updateDots();
        _DotsManager.update();

        ui->lblNumberOfDots->setText(std::to_string(_DotsManager.getDotCount()).c_str());

        if ( _DotsManager.getCollisionCount() == 0 && _DotsManager.getLastCollisionCount() > 0 )
        {
            int CollisionCount = _DotsManager.getLastCollisionCount();
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

    // Resize GUI
    qGuiApp->sendEvent(this, &resizeEvent);

    // Create the initial set of dots
    CreateDots((uint)(ui->sldrDots->value()), (uint)(ui->sldrDotSize->value()));

    // Start the timer with the awesome lambda inside :-)
    RenderTimer.start(33);
}

void MainWindow::sizeGUIElements()
{
#if defined(Q_OS_ANDROID)
    int iWidth = this->screen()->availableGeometry().width();
    int iHeight = this->screen()->availableGeometry().height();
    this->setGeometry(QRect(0,0,iWidth,iHeight));
#else
    int iWidth = this->window()->childrenRect().width();
    int iHeight = this->window()->childrenRect().height();
#endif

    int iOneThird = (int)((float)iWidth/3.0f);
    int iOneSixth = (int)((float)iOneThird/2.0f);
    int iOneTwelth = (int)((float)iOneSixth/2.0f);
    int iOneFifth = (int)((float)iWidth/5.0f);
    int iOneTenth = (int)((float)iOneFifth/2.0f);
    int iOneTwentieth = (int)((float)iOneFifth/2.0f);

    QRect rctMessage = QRect(0,
                             0,
                             iWidth-iOneThird-iOneSixth,
                             20);

    QRect rctDotsLeft = QRect(iOneTwelth + rctMessage.left() + rctMessage.width(),
                              0,
                              80,
                              20);

    QRect rctNumberDotsLeft = QRect(rctDotsLeft.left() + rctDotsLeft.width(),
                                    0,
                                    40,
                                    20);

    QRect rctDrawBox = QRect(0,
                             20,
                             iWidth,
                             iHeight-120);

    QRect rctDotSize = QRect(iOneTwelth/2,
                             rctDrawBox.top()+rctDrawBox.height()+10,
                             70,
                             20);

    QRect rctDotSizeSlider = QRect(rctDotSize.left()+rctDotSize.width(),
                                   rctDrawBox.top()+rctDrawBox.height()+10,
                                   iOneThird - iOneTwelth,
                                   20);

    QRect rctDots = QRect(rctDotSizeSlider.left()+rctDotSizeSlider.width()+iOneTwelth,
                          rctDrawBox.top()+rctDrawBox.height()+10,
                          40,
                          20);

    QRect rctDotsSlider = QRect(rctDots.left()+rctDots.width(),
                                rctDrawBox.top()+rctDrawBox.height()+10,
                                iOneThird - iOneTwelth,
                                20);

    QRect rctReset = QRect(iOneTwelth/2,
                           rctDotSize.top()+rctDotSize.height() + 10,
                           iOneTenth+iOneTwentieth,
                           30);

    QRect rctBackColor = QRect(iOneTwelth/2 + rctReset.width() + rctReset.left(),
                               rctDotSize.top()+rctDotSize.height() + 10,
                               2*iOneFifth + iOneTwentieth/2,
                               30);

    QRect rctQuit = QRect(iOneTwelth/2 + rctBackColor.width() + rctBackColor.left(),
                          rctDotSize.top()+rctDotSize.height() + 10,
                          iOneTenth+iOneTwentieth,
                          30);

    ui->lblMessage->setGeometry(rctMessage);
    ui->lblDotsLeft->setGeometry(rctDotsLeft);
    ui->lblNumberOfDots->setGeometry(rctNumberDotsLeft);
    ui->frmDrawBox->setGeometry(rctDrawBox);
    ui->lblDotSize->setGeometry(rctDotSize);
    ui->sldrDotSize->setGeometry(rctDotSizeSlider);
    ui->lblDots->setGeometry(rctDots);
    ui->sldrDots->setGeometry(rctDotsSlider);
    ui->btnReset->setGeometry(rctReset);
    ui->btnNextBackColor->setGeometry(rctBackColor);
    ui->btnQuit->setGeometry(rctQuit);

    _DotsManager.setGeometry(rctDrawBox);
    _DotsManager.setBounds(rctDrawBox);

    ui->frmDrawBox->lower();
    ui->frmDrawBox->update();
    _DotsManager.raise();
    _DotsManager.update();

    ui->lblMessage->update();
    ui->lblDotsLeft->update();
    ui->lblNumberOfDots->update();
    ui->frmDrawBox->update();
    ui->lblDotSize->update();
    ui->sldrDotSize->update();
    ui->lblDots->update();
    ui->sldrDots->update();
    ui->btnReset->raise();
    ui->btnReset->update();
    ui->btnNextBackColor->raise();
    ui->btnNextBackColor->update();
    ui->btnQuit->raise();
    ui->btnQuit->update();
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    //qDebug() << "MainWindow::resizeEvent: ";
    sizeGUIElements();
}


MainWindow::~MainWindow()
{
    delete ui;
}


