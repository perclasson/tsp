#include <iostream>

#include <QtGui>
#include <QtSvg>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

#include "view.h"
#include "vertex.h"

class QGraphicsScene;

// Construct a view showing problem/solution from the given input stream.
View::View(std::istream &in, Options options, QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent),
    m_legPen(Qt::black, 1.0),
    m_currentLegPen(QColor(Qt::green).darker(), 1.0),
    m_currentLeg(0),
    m_hudFont("Courier", 10),
    m_labelFont("Courier", 6),
    m_solutionLength(0),
    m_options(options)
{
    resize(900, 600);
    setRenderHints(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // :(
    setBackgroundBrush(Qt::white);

    m_legPen.setCosmetic(true);
    m_currentLegPen.setCosmetic(true);

    // Read points.
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
        Vertex *item;
        if (numPoints < 100) {
            // If we have less than 100 vertices, we label them.
            item = new Vertex(x, y, 8, i, Vertex::ShowLabel);
        } else {
            // We just draw them as dots.
            item = new Vertex(x, y, 2, i, Vertex::NoLabel);
        }
        scene->addItem(item);
        item->setZValue(1); // On top of edges.

    }

    if (m_options & CompleteGraph) {
        // Add the complete set of edges.
        QPen edgePen(Qt::gray);
        edgePen.setCosmetic(true);
        for (int i = 0; i < numPoints; ++i) {
            for (int j = 0; j < numPoints; ++j) {
                if (i == j)
                    continue;
                float x1 = m_points[i].x();
                float y1 = m_points[i].y();
                float x2 = m_points[j].x();
                float y2 = m_points[j].y();

                QGraphicsLineItem *edgeItem = scene->addLine(x1, y1, x2, y2, edgePen);
                m_edges.append(edgeItem);

                if (m_options & ShowLengths) {
                    addLengthLabel(x1, y1, x2, y2);
                }
            }
        }
    }

    // Read solution, if any.
    if (m_options & PointPairs) {
        readPointPairs(in);
    } else {
        readPointSequence(in);
    }

    // Center view on mean coordinate and zoom to fit entire scene.
    const float margin = qAbs(maxX - minX) / 30;
    setSceneRect(minX - margin, minY - margin, maxX - minX + 2*margin, maxY - minY + 2*margin);
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

            // Temporarily give legs and edges non-cosmetic pens while rendering.
            foreach (QGraphicsLineItem *leg, m_legs) {
                QPen pen = leg->pen();
                pen.setCosmetic(false);
                pen.setWidthF(1.0/transform().m11());
                leg->setPen(pen);
            }
            foreach (QGraphicsLineItem *edge, m_edges) {
                QPen pen = edge->pen();
                pen.setCosmetic(false);
                pen.setWidthF(1.0/transform().m11());
                edge->setPen(pen);
            }

            QPainter painter(&generator);
            render(&painter);

            // Restore the cosmetic pens for legs and edges.
            foreach (QGraphicsLineItem *leg, m_legs) {
                QPen pen = leg->pen();
                pen.setCosmetic(true);
                pen.setWidthF(1.0);
                leg->setPen(pen);
            }
            foreach (QGraphicsLineItem *edge, m_edges) {
                QPen pen = edge->pen();
                pen.setCosmetic(true);
                pen.setWidthF(1.0);
                edge->setPen(pen);
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
    painter->setFont(m_hudFont);
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
            m_legs[m_currentLeg++]->setPen(m_legPen);
            showCurrentLeg();
        }
    }
}

void View::moveBackward(int steps) {
    for (int i = 0; i < steps; ++i) {
        if (m_currentLeg > 0) {
            hideCurrentLeg();
            m_legs[--m_currentLeg]->setPen(m_currentLegPen);
        }
    }
}

void View::readPointSequence(std::istream& in) {
    // Read solution points.
    QList<int> solutionPoints;
    while (in.good()) {
        int point;
        in >> point;
        if (in.good()) {
            solutionPoints.append(point);
        }
    }
    if (solutionPoints.size() < 2)
        return; // Need at least two points.

    // Create lines / length labels.
    for (int i = 0, j = 1; i < solutionPoints.size(); ++i, ++j) {
        QPointF start = m_points[solutionPoints[i]];
        QPointF end = m_points[solutionPoints[j % solutionPoints.size()]];
        QGraphicsLineItem *leg = scene()->addLine(QLineF(start, end), m_legPen);
        leg->setVisible(false);
        m_legs.append(leg);
        m_solutionLength += qRound(leg->line().length());
        if (m_options & ShowLengths) {
            QGraphicsTextItem *label = addLengthLabel(start.x(), start.y(), end.x(), end.y());
            m_legLabels.append(label);
            label->setVisible(false);
        }
    }

    showCurrentLeg();
}

void View::readPointPairs(std::istream& in) {
    while (in.good()) {
        // Read point pair.
        int startIndex;
        int endIndex;
        in >> startIndex >> endIndex;
        if (in.good()) {
            // Create line / length label.
            QPointF start = m_points[startIndex];
            QPointF end = m_points[endIndex];
            QGraphicsLineItem *leg = scene()->addLine(QLineF(start, end), m_legPen);
            leg->setVisible(false);
            m_legs.append(leg);
            m_solutionLength += qRound(leg->line().length());
            if (m_options & ShowLengths) {
                QGraphicsTextItem *label = addLengthLabel(start.x(), start.y(), end.x(), end.y());
                m_legLabels.append(label);
                label->setVisible(false);
            }
        }
    }
    showCurrentLeg();
}

void View::showCurrentLeg() {
    if (m_legs.isEmpty())
        return;
    m_legs[m_currentLeg]->setPen(m_currentLegPen);
    m_legs[m_currentLeg]->setVisible(true);
    if (m_options & ShowLengths) {
        m_legLabels[m_currentLeg]->setVisible(true);
    }
}

void View::hideCurrentLeg() {
    if (m_legs.isEmpty())
        return;
    m_legs[m_currentLeg]->setVisible(false);
    m_legs[m_currentLeg]->setPen(m_legPen);
    if (m_options & ShowLengths) {
        m_legLabels[m_currentLeg]->setVisible(false);
    }
}

QGraphicsTextItem *View::addLengthLabel(double x1, double y1, double x2, double y2) {
    const double length = qRound(qSqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)));
    QGraphicsTextItem *item = scene()->addText(QString::number(length), m_labelFont);
    item->setPos((x1 + x2) / 2, (y1 + y2) / 2);
    item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    return item;
}

