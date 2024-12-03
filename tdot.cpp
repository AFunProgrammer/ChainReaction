#include "tdot.h"

using namespace std;

void t_Dot::setBaseSize(int Size) {
    m_BaseSize = Size;
    m_Size = Size;
    m_Radius = Size / 2;
}

void t_Dot::setRandomColor() {
    static std::uniform_int_distribution<int> dist(1, 6);
    static std::mt19937 rng(std::random_device{}());
    m_Color = static_cast<eColor>(dist(rng));
}

void t_Dot::setRandomDirection() {
    static std::uniform_int_distribution<int> distDir(1, 5);
    static std::uniform_int_distribution<int> distSign(0, 1);
    static std::mt19937 rng(std::random_device{}());

    int rDirX = (distSign(rng) == 0) ? -1 : 1;
    int rDirY = (distSign(rng) == 0) ? -1 : 1;
    m_Direction = QPoint(rDirX * distDir(rng), rDirY * distDir(rng));
}

void t_Dot::setRandomPosition(QSize Bounds) {
    std::uniform_int_distribution<int> distX(0, Bounds.width());
    std::uniform_int_distribution<int> distY(0, Bounds.height());
    std::mt19937 rng(std::random_device{}());

    m_Position = QPoint(distX(rng), distY(rng));
}

void t_Dot::update() {
    if (!m_Touched) {
        m_Previous = m_Position;

        if (m_Direction.x() > 0 && (m_Position.x() + m_Radius) >= m_BoxBounds.width()) {
            m_Direction.setX(-m_Direction.x());
        } else if (m_Direction.x() < 0 && (m_Position.x() - m_Radius) <= 0) {
            m_Direction.setX(-m_Direction.x());
        } else {
            m_Position.setX(m_Position.x() + m_Direction.x());
        }

        if (m_Direction.y() > 0 && (m_Position.y() + m_Radius) >= m_BoxBounds.height()) {
            m_Direction.setY(-m_Direction.y());
        } else if (m_Direction.y() < 0 && (m_Position.y() - m_Radius) <= 0) {
            m_Direction.setY(-m_Direction.y());
        } else {
            m_Position.setY(m_Position.y() + m_Direction.y());
        }
    } else if (m_Touched && !m_Exploded) {
        if (m_Sizing > 0) {
            m_Scale += 0.25f;

            if (m_Scale >= m_MaxScale)
                m_Sizing = -1;
        } else {
            m_Scale -= 0.33f;

            if (m_Scale <= 0.01f) {
                m_Scale = 0.005f;
                m_Exploded = true;
            }
        }

        m_Size = m_BaseSize * m_Scale;
        m_Radius = m_Size / 2;
    }
}

QRect t_Dot::getDrawRect() {
    QRectF BoundsF(static_cast<float>(m_Position.x()) - m_Radius,
                   static_cast<float>(m_Position.y()) - m_Radius,
                   m_Size,
                   m_Size);

    return BoundsF.toAlignedRect();
}
