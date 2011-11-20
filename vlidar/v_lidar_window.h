#ifndef V_LIDAR_WINDOW_H
#define V_LIDAR_WINDOW_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class VLidarWindow;
}

class VLidarWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VLidarWindow(QWidget *parent = 0);
    ~VLidarWindow();

protected slots:
    void connectToLidar();
    void disconnectFromLidar();
    void updateLidarGraphics();
    void updateLidar();

    void enableWriteToFile();

protected:
    void drawSignal();
    void drawSignal2D();

    bool isConnectedToLidar();
    enum{ MIN_WINDOW_HEIGHT = 200,
          MIN_WINDOW_WITH = 400};

private:
    class DPointer;
    DPointer  *d;
};

#endif // V_LIDAR_WINDOW_H
