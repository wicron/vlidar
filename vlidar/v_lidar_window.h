#ifndef V_LIDAR_WINDOW_H
#define V_LIDAR_WINDOW_H

#include <QWidget>
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QRect>

namespace Ui {
class VLidarWindow;
}

class VLidarWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VLidarWindow(QWidget *parent = 0);
    ~VLidarWindow();

    template <typename T>
    static bool drawGraphic(T *data, int dataSize,  QGraphicsScene *scene, QRectF rectToDraw, int gridLinesNumber=10,
                             qreal x0=0, qreal xscale = 1);

protected slots:
    void connectToLidar();
    void disconnectFromLidar();
    void updateLidarGraphics();

    void enableWriteToFile();

protected:
    bool updateLidar();
    void drawSignal();
    void drawSignal2D();

    bool isConnectedToLidar();
    enum{ MIN_WINDOW_HEIGHT = 200,
          MIN_WINDOW_WITH = 400};
    static const int MEASURMENTS_NUMBER;
    static const qreal START_PHI;
    static const qreal FINAL_PHI;
    static const qreal PHI_SCALE;

private:
    class DPointer;
    DPointer  *d;
};

template <class T>
bool VLidarWindow::drawGraphic(T *data, int dataSize,  QGraphicsScene *scene, QRectF rectToDraw, int gridLinesNumber,
                               qreal x0, qreal xscale)
{
    qreal mediumValue=0;
    qreal bigestValue=-10e6;
    qreal lowestValue=10e6;

    //Borders for writing numbers.
    qreal xBorder = 40;
    qreal yBorder = 40;
    if(rectToDraw.width()<4*xBorder || rectToDraw.height() < 4*yBorder){
        // "rectangle is too small to draw in it");
        return false;
    }

    //calc medium, biggest and smallest value for scale calculation
    for(int i = 0; i<dataSize-1; i++)
    {
        mediumValue+=data[i];
        bigestValue = data[i] > bigestValue ? data[i] : bigestValue;
        lowestValue = data[i] < lowestValue ? data[i] : lowestValue;
    }
    mediumValue /= dataSize;
    qreal graphXscale=(rectToDraw.width()-2*xBorder)/dataSize/xscale;
    qreal graphYscale = (rectToDraw.height()-2*yBorder)/(bigestValue-lowestValue);

    qreal yLower = rectToDraw.y() +rectToDraw.height();
    for(int i = 0; i<dataSize-1; i++)
    {
        //draw graphic. It low border is line y=0. And left border is x=0
        qreal x=i*xscale*graphXscale + xBorder + rectToDraw.x();
        scene->addLine(x, yLower - (data[i]-lowestValue)*graphYscale - yBorder,
                       x+xscale*graphXscale, yLower - (data[i+1]-lowestValue)*graphYscale - yBorder, QPen(QColor(255,0,0)));
    }

    qreal gridNumber=gridLinesNumber;
    for(int i=0; i<gridNumber+1; i++)
    {
        //draw grid
        qreal y=yLower - i/gridNumber*(bigestValue-lowestValue)*graphYscale - yBorder;
        scene->addLine(xBorder + rectToDraw.x(), y, rectToDraw.x() + rectToDraw.width() - xBorder,
                       y, QPen(QColor(0,255,0,100)));
        qreal xGrid = xBorder + i/gridNumber*xscale*dataSize*graphXscale + rectToDraw.x();
        scene->addLine(xGrid, rectToDraw.y() + yBorder, xGrid,
                       yLower - yBorder, QPen(QColor(0,255,0,100)));

        //draw text marks
        QGraphicsTextItem *hText = scene->addText(QString::number(x0+i/gridNumber*xscale*dataSize));
        hText->setPos(xGrid, yLower);

        QGraphicsTextItem *vText = scene->addText(QString::number(i*(bigestValue-lowestValue)/gridNumber));
        vText->setPos(rectToDraw.x(), y);
    }
    return true;
}

#endif // V_LIDAR_WINDOW_H
