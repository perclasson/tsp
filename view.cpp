#include <iostream>

#include <QtGui>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

#include "view.h"

class QGraphicsScene;

// Construct a view showing problem/solution from the given input stream.
View::View(std::istream &in, QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent),
    m_legPen(Qt::black, 0),
    m_currentLegPen(QColor(Qt::green).darker(), 0),
    m_currentLeg(0)
{
    resize(900, 600);
    setRenderHints(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // :(

    // Read problem.
    int numPoints;
    in >> numPoints;
    m_points.resize(numPoints);
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    for (int i = 0; i < numPoints; ++i) {
        float x, y;
        in >> x >> y;
        m_points[i].setX(x);
        m_points[i].setY(y);
        minX = qMin(minX, x);
        minY = qMin(minY, y);
        maxX = qMax(maxX, x);
        maxY = qMax(maxY, y);
    }
    setSceneRect(minX - 10, minY - 10, maxX - minX + 10, maxY - minY + 10);

    // Read solution, if any.
    int firstIndex, startIndex, endIndex;
    in >> firstIndex;
    if (!in.eof()) {
        startIndex = firstIndex;
        while (endIndex != firstIndex) {
            if (in.eof())
                endIndex = firstIndex;
            else
                in >> endIndex;
            QPointF start = m_points[startIndex];
            QPointF end = m_points[endIndex];
            QGraphicsLineItem *leg = scene->addLine(QLineF(start, end), m_legPen);
            leg->setVisible(false);
            m_legs.append(leg);
            startIndex = endIndex;
        }
        m_legs[m_currentLeg]->setPen(m_currentLegPen);
        m_legs[m_currentLeg]->setVisible(true);
        fitInView(m_legs[m_currentLeg], Qt::KeepAspectRatio);
    }
}

void View::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() == Qt::ControlModifier) {
        if (event->delta() > 0) {
            scale(2, 2);
        } else if (event->delta() < 0){
            scale(0.5, 0.5);
        }
        return;
    }
    QGraphicsView::wheelEvent(event);
}

void View::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Right) {
        int steps = 1;
        if (event->modifiers() == Qt::ControlModifier)
            steps = 10;
        if (event->modifiers() == Qt::ShiftModifier)
            steps = 100;
        moveForward(steps);
    } else if (event->key() == Qt::Key_Left) {
        int steps = 1;
        if (event->modifiers() == Qt::ControlModifier)
            steps = 10;
        if (event->modifiers() == Qt::ShiftModifier)
            steps = 100;
        moveBackward(steps);
    }
}

void View::drawForeground(QPainter *painter, const QRectF& rect) {
    const int radius = 2.0;

    painter->setMatrixEnabled(false);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::black);
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF biggerRect = rect.adjusted(-100, -100, 100, 100);
    QPointF viewPoint;
    foreach (const QPointF& point, m_points) {
        if (biggerRect.contains(point)) {
            viewPoint = mapFromScene(point);
            painter->drawEllipse(viewPoint, radius, radius);
        }
    }
}

void View::moveForward(int steps) {
    for (int i = 0; i < steps; ++i) {
        if (m_currentLeg < m_legs.size() - 1) {
            m_legs[m_currentLeg]->setPen(m_legPen);
            ++m_currentLeg;
            m_legs[m_currentLeg]->setVisible(true);
            m_legs[m_currentLeg]->setPen(m_currentLegPen);
        }
    }
}

void View::moveBackward(int steps) {
    for (int i = 0; i < steps; ++i) {
        if (m_currentLeg > 0) {
            m_legs[m_currentLeg]->setVisible(false);
            m_legs[m_currentLeg]->setPen(m_legPen);
            --m_currentLeg;
            m_legs[m_currentLeg]->setPen(m_currentLegPen);
        }
    }
}

