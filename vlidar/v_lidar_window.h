#ifndef V_LIDAR_WINDOW_H
#define V_LIDAR_WINDOW_H

#include <QWidget>

namespace Ui {
    class VLidarWindow;
}

class VLidarWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VLidarWindow(QWidget *parent = 0);
    ~VLidarWindow();

protected slots:
    void connectToLidar();
    void disconnectFromLidar();
    void updateLidarGraphics();
    void writeToFile();

protected:
    void drawSignal();
    void drawSignal2D();
    void updateLidar();

private:
    class DPointer;
    DPointer  *d;
};

#endif // V_LIDAR_WINDOW_H
