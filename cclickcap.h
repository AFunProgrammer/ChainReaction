#ifndef CCLICKCAP_H
#define CCLICKCAP_H

#include <QWidget>
#include <QReadWriteLock>

class CClickCap : public QWidget
{
    Q_OBJECT

    QReadWriteLock m_updateLock; //only allow one item to be resized at a time

    QPoint m_lastClick = QPoint(0,0);
    QList<QPoint> m_clickHistory;
    QRect m_lastClickArea = QRect(0,0,0,0);
    qulonglong m_lastUpdateTick = 0;

public:
    explicit CClickCap(QWidget *parent = nullptr);

    void setBounds(QRect Geometry);
    QRect getBounds();

    QPoint getLastClick();
    QRect getLastClickArea();
    qulonglong getLastUpdateTick();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent* event) override;
signals:

};

#endif // CCLICKCAP_H
