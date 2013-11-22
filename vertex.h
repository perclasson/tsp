#ifndef VERTEX_H
#define VERTEX_H

#include <QtWidgets>

class Vertex : public QGraphicsEllipseItem {
    public:
        enum LabelMode {
            ShowLabel,
            NoLabel
        };

        Vertex(double x, double y, double radius,
                int vertexNum, LabelMode labelMode = NoLabel,
                QGraphicsItem *parent = 0) :
            QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius, parent),
            m_labelMode(labelMode),
            m_vertexNum(vertexNum)
        {
            setPos(x, y);
            setFlag(QGraphicsItem::ItemIgnoresTransformations);
            m_labelFont.setPointSize(6);

            if (m_labelMode == ShowLabel) {
                setPen(QPen(Qt::black));
                setBrush(Qt::white);
            } else {
                setPen(Qt::NoPen);
                setBrush(Qt::black);
            }
        }

    public:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0)
        {
            QGraphicsEllipseItem::paint(painter, option, widget);
            if (m_labelMode == ShowLabel) {
                painter->setFont(m_labelFont);
                painter->drawText(boundingRect(),
                        Qt::AlignHCenter | Qt::AlignVCenter,
                        QString::number(m_vertexNum));
            }
        }

    private:
        LabelMode m_labelMode;
        QFont m_labelFont;
        int m_vertexNum;
};

#endif // VERTEX_H
