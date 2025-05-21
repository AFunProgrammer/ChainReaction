#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QMetaEnum>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QObject>
#include <QSlider>

typedef struct _ColorScheme {
    Qt::GlobalColor m_BackColor = Qt::black;
    Qt::GlobalColor m_FontColor = Qt::white;
}TColorScheme;

TColorScheme g_ChangeColors[] = {
    {Qt::black,Qt::white},
    {Qt::white,Qt::black},
    {Qt::cyan,Qt::black},
    {Qt::darkCyan,Qt::white},
    {Qt::magenta,Qt::black},
    {Qt::darkMagenta,Qt::white},
    {Qt::lightGray,Qt::black},
    {Qt::darkGray,Qt::white}
};

uint g_Collisions = 0;
QEvent resizeEvent = QEvent(QEvent::Resize);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnChangeBackColor->connect(ui->btnChangeBackColor,&QPushButton::clicked,[this]()
    {
        changeColorScheme();
    });

    ui->sldrDots->connect(ui->sldrDots,&QSlider::valueChanged,[this](int value)
    {
        createDots((uint)value, ui->oglDots->geometry().size(), (uint)(ui->sldrDotSize->value()));
    });

    ui->sldrDotSize->connect(ui->sldrDotSize,&QSlider::valueChanged,[](int value)
    {
        //CreateDots((uint)(ui->sldrDots->value()), (uint)value);
        CDotsManager::getGlobalInstance()->setDotsSize(value);
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
        createDots((uint)(ui->sldrDots->value()), ui->oglDots->geometry().size(), (uint)(ui->sldrDotSize->value()));
    });

    RenderTimer.setInterval(33);
    RenderTimer.connect(&RenderTimer, &QTimer::timeout, [this]()
    {
        QVector<PTDot> PhysicsDots;
        CDotsManager::getGlobalInstance()->updateDotsMovement(&PhysicsDots);
        CDotsManager::getGlobalInstance()->updateDotsPhysics(&PhysicsDots);

        ui->oglDots->update();

        uint remainingDots = CDotsManager::getGlobalInstance()->getRemainingDotCount();
        ui->lblNumberOfDots->setText(std::to_string(remainingDots).c_str());

        if (  CDotsManager::getGlobalInstance()->getCollisionCount() == 0 &&
              CDotsManager::getGlobalInstance()->getLastCollisionCount() > 0 )
        {
            int CollisionCount =  CDotsManager::getGlobalInstance()->getLastCollisionCount();
            QString strYouCaused = QString(std::to_string(CollisionCount).c_str()) + QString(" Collisions - ");
            if ( CollisionCount < 100 )
                strYouCaused += "Good";
            if ( CollisionCount < 300 && CollisionCount >= 100 )
                strYouCaused += "Awesome!";
            if ( CollisionCount < 500 && CollisionCount >= 300 )
                strYouCaused += "Fantastic!";
            if ( CollisionCount < 800 && CollisionCount >= 500 )
                strYouCaused += "Tubular!";
            if ( CollisionCount < 1000 && CollisionCount >= 800 )
                strYouCaused += "Gnarly!";
            if ( CollisionCount < 1500 && CollisionCount >= 1000 )
                strYouCaused += "Master!";
            if ( CollisionCount >= 1500  )
                strYouCaused += "Guru!";

            ui->lblMessage->setText(strYouCaused);
        }
    });//end of timer loop

    ui->btnQuit->connect(ui->btnQuit, &QPushButton::clicked, qApp, &QCoreApplication::quit);

    // change the backcolor to the initial backcolor
    changeColorScheme();
}


void MainWindow::showEvent(QShowEvent *event) {
    // Create the initial set of dots, doing this in the showEvent
    //  ensures the geometry is sized by the layout manager first
    QTimer::singleShot(100, [this](){
        createDots((uint)(ui->sldrDots->value()), ui->oglDots->geometry().size(), (uint)(ui->sldrDotSize->value()));
    });
    QWidget::showEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeColorScheme()
{
    static uint changeColorsIndex = sizeof(g_ChangeColors)/sizeof(TColorScheme);

    changeColorsIndex++;

    // get the next backcolor in the list
    if (changeColorsIndex >= sizeof(g_ChangeColors)/sizeof(TColorScheme) )
    {
        changeColorsIndex = 0;
    }

    // convert colors to strings for using in the style sheets
    const char* backColor = QMetaEnum::fromType<Qt::GlobalColor>().valueToKey(g_ChangeColors[changeColorsIndex].m_BackColor);
    const char* fontColor = QMetaEnum::fromType<Qt::GlobalColor>().valueToKey(g_ChangeColors[changeColorsIndex].m_FontColor);

    // Generate stylesheet string
    QString backgroundStyle = QString("background-color: %1;").arg(backColor);
    QString textStyle = QString("color: %1;").arg(fontColor);

    // change the clear color (or back color of the ogl device)
    ui->oglDots->setClearColor(g_ChangeColors[changeColorsIndex].m_BackColor);

    // change the font color to be visible no matter the back color
    ui->lblDotSize->setStyleSheet(textStyle);
    ui->lblDots->setStyleSheet(textStyle);
    ui->lblDotsLeft->setStyleSheet(textStyle);
    ui->lblNumberOfDots->setStyleSheet(textStyle);
    ui->lblMessage->setStyleSheet(textStyle);
    ui->btnReset->setStyleSheet(textStyle);
    ui->btnChangeBackColor->setStyleSheet(textStyle);
    ui->btnQuit->setStyleSheet(textStyle);

    // change the background color to match the render color
    // BUG: BUG - there's discrepancy between ogl render color and back color
    ui->centralwidget->setStyleSheet(backgroundStyle);
}

void MainWindow::createDots(uint Count, QSize Bounds, uint Size)
{
    /***** TODO: Optimize Cleanup Code *********/
    this->RenderTimer.stop();
    //ui->oglDots->clearDots();
    /************ End of Cleanup Code **********/

    CDotsManager::getGlobalInstance()->setupDots(Count,Bounds,Size);

    RenderTimer.start(33);
}


