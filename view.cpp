#include <iostream>

#include <QtGui>
#include <QtSvg>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

#include "view.h"

class QGraphicsScene;

// Construct a view showing problem/solution from the given input stream.
View::View(std::istream &in, EdgeFormat edgeFormat, QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent),
    m_legPen(Qt::black, 1.0),
    m_currentLegPen(QColor(Qt::green).darker(), 1.0),
    m_currentLeg(0),
    hudFont("Courier", 10),
    m_solutionLength(0)
{
    resize(900, 600);
    setRenderHints(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // :(

    m_legPen.setCosmetic(true);
    m_currentLegPen.setCosmetic(true);
    // Read problem.
    int numPoints;
    in >> numPoints;
    m_points.resize(numPoints);
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    double sumX = 0;
    double sumY = 0;
    for (int i = 0; i < numPoints; ++i) {
        float x, y;
        in >> x >> y;
        m_points[i].setX(x);
        m_points[i].setY(y);
        minX = qMin(minX, x);
        minY = qMin(minY, y);
        maxX = qMax(maxX, x);
        maxY = qMax(maxY, y);
        sumX += x;
        sumY += y;
        QGraphicsEllipseItem *item = scene->addEllipse(-2.0, -2.0, 4.0, 4.0);
        item->setPos(x, y);
        item->setPen(Qt::NoPen);
        item->setBrush(Qt::black);
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        item->setZValue(1); // On top of edges.
    }

    const float margin = qAbs(maxX - minX) / 30;
    setSceneRect(minX - margin, minY - margin, maxX - minX + 2*margin, maxY - minY + 2*margin);

    // Read solution, if any.
    if (edgeFormat == Sequence) {
        readPointSequence(in);
    } else {
        readPointPairs(in);
    }

    // Center view on mean coordinate and zoom to fit entire scene.
    centerOn(QPointF(sumX / numPoints, sumY / numPoints));
    fitInView(sceneRect(), Qt::KeepAspectRatio);
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
        // Zoom in.
        int steps = 1;
        if (event->modifiers() == Qt::ControlModifier)
            steps = 10;
        if (event->modifiers() == Qt::ShiftModifier)
            steps = 100;
        moveForward(steps);
    } else if (event->key() == Qt::Key_Left) {
        // Zoom out.
        int steps = 1;
        if (event->modifiers() == Qt::ControlModifier)
            steps = 10;
        if (event->modifiers() == Qt::ShiftModifier)
            steps = 100;
        moveBackward(steps);
    } else if (event->key() == Qt::Key_S) {
        if (event->modifiers() == Qt::ControlModifier) {
            /*
             * Render SVG.
             */
            QString fileName = QFileDialog::getSaveFileName(this,
                   "Save SVG", QString(), "SVG Files (*.svg)");
            QSvgGenerator generator;
            generator.setFileName(fileName);
            generator.setSize(viewport()->rect().size());
            generator.setViewBox(viewport()->rect());

            // Temporarily give legs non-cosmetic pens while rendering.
            foreach (QGraphicsLineItem *leg, m_legs) {
                QPen pen = leg->pen();
                pen.setCosmetic(false);
                pen.setWidthF(1.0/transform().m11());
                leg->setPen(pen);
            }

            QPainter painter(&generator);
            render(&painter);

            // Restore the cosmetic pens for legs.
            foreach (QGraphicsLineItem *leg, m_legs) {
                QPen pen = leg->pen();
                pen.setCosmetic(true);
                pen.setWidthF(1.0);
                leg->setPen(pen);
            }
        }
    } else if (event->key() == Qt::Key_Escape) {
        // Exit
        qApp->quit();
    }
}

void View::drawForeground(QPainter *painter, const QRectF& rect) {
    painter->setMatrixEnabled(false);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(hudFont);
    painter->setPen(Qt::gray);

    // Draw solution length HUD text.
    const QString solutionHud = QString("Solution length: %1").arg(m_solutionLength);
    const QPointF solutionHudPos = rect.bottomLeft() +
        QPointF(10/transform().m11(), -15/transform().m22());
    painter->drawText(mapFromScene(solutionHudPos), solutionHud);

    if (!m_legs.isEmpty()) {
        // Draw leg length HUD text.
        const int legLength = qRound(m_legs[m_currentLeg]->line().length());
        const QString legHud = QString("Leg length: %1").arg(legLength);
        const QPointF legHudPos = rect.bottomLeft() +
            QPointF(10/transform().m11(), -30/transform().m22());
        painter->drawText(mapFromScene(legHudPos), legHud);
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

void View::readPointSequence(std::istream& in) {
    int firstIndex, startIndex, endIndex;
    endIndex = -1;
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
            QGraphicsLineItem *leg = scene()->addLine(QLineF(start, end), m_legPen);
            leg->setVisible(false);
            m_legs.append(leg);
            m_solutionLength += qRound(leg->line().length());
            startIndex = endIndex;
        }
        m_legs[m_currentLeg]->setPen(m_currentLegPen);
        m_legs[m_currentLeg]->setVisible(true);
    }
}

void View::readPointPairs(std::istream& in) {
    while (!in.eof()) {
        int startIndex, endIndex;
        in >> startIndex >> endIndex;
        if (in.eof())
            break; // Hack: To avoid last edge being added twice.
        QPointF start = m_points[startIndex];
        QPointF end = m_points[endIndex];
        QGraphicsLineItem *leg = scene()->addLine(QLineF(start, end), m_legPen);
        leg->setVisible(false);
        m_legs.append(leg);
        m_solutionLength += qRound(leg->line().length());
    }
    if (!m_legs.isEmpty()) {
        m_legs[m_currentLeg]->setPen(m_currentLegPen);
        m_legs[m_currentLeg]->setVisible(true);
    }
}
