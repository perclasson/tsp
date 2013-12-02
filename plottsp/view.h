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
    // View options.
    enum Option {
        CompleteGraph = 0x1, // Show the complete graph.
        ShowLengths = 0x2,   // Show edge lengths.
        PointSequence = 0x4, // Edges are specified as sequence of points.
        PointPairs = 0x8     // Edges are specified as pairs of points.
    };
    Q_DECLARE_FLAGS(Options, Option)

    explicit View(std::istream& in, Options options, QGraphicsScene *scene, QWidget *parent = 0);

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

    // Show/hide current leg of solution.
    void showCurrentLeg();
    void hideCurrentLeg();

    // Add a length label for the given edge.
    QGraphicsTextItem *addLengthLabel(double x1, double y1, double x2, double y2);

    QVector<QPointF> m_points;           // Points in the problem.
    QList<QGraphicsLineItem *> m_edges;  // Edges in the problem.
    QList<QGraphicsLineItem *> m_legs;   // Legs in the solution.
    QList<QGraphicsTextItem *> m_legLabels; // Legs length labels.

    QPen m_legPen;             // Pen used for legs.
    QPen m_currentLegPen;      // Pen used for current leg.
    int m_currentLeg;          // Index of current leg.

    QFont m_hudFont;       // Font for the HUD.
    QFont m_labelFont;     // Font for the edge length labels.
    int m_solutionLength;  // Total length of solution.
    Options m_options;     // View options.
};

Q_DECLARE_OPERATORS_FOR_FLAGS(View::Options)

#endif // VIEW_H
