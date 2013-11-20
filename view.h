#ifndef VIEW_H
#define VIEW_H

#include <iostream>

#include <QtGui>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

class View : public QGraphicsView
{
    Q_OBJECT

public:
    enum EdgeFormat {
        Pairs,      // Edges are specified as pairs of points.
        Sequence    // Edges are specified as sequence of points (oldkattis:tsp format).
    };

    explicit View(std::istream& in, EdgeFormat edgeFormat, QGraphicsScene *scene, QWidget *parent = 0);

protected:
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void drawForeground(QPainter *painter, const QRectF& rect);

private:
    // Reads edges following the problem.
    void readPointSequence(std::istream& in);
    void readPointPairs(std::istream& in);

    // Move forward/backward in the display of the solution.
    void moveForward(int steps);
    void moveBackward(int steps);

    QVector<QPointF> m_points;           // Points in the problem.
    QList<QGraphicsLineItem *> m_legs;   // Legs in the solution.

    QPen m_legPen;             // Pen used for legs.
    QPen m_currentLegPen;      // Pen used for current leg.
    int m_currentLeg;          // Index of current leg.

    QFont hudFont;         // Font for the HUD.
    int m_solutionLength;  // Total length of solution.
};

#endif // VIEW_H
