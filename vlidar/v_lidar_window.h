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
    friend class VTestGui;

public:
    explicit VLidarWindow(QWidget *parent = 0);
    ~VLidarWindow();

protected slots:
    void connectToLidar();
    void disconnectFromLidar();
    void updateLidarGraphics();

    void enableWriteToFile();

protected:
    void updateButtons();
    bool updateLidar();
    void drawSignal();
    void drawSignal2D();
    void writeLogToFile();

    bool isConnectedToLidar();
    enum{ MIN_WINDOW_HEIGHT = 200,
          MIN_WINDOW_WITH = 400};
    static const int MEASURMENTS_NUMBER;
    static const qreal START_PHI;
    static const qreal FINAL_PHI;
    static const qreal PHI_SCALE;
    static const qreal LIDAR_DISTANCE_SCALE;

private:
    class DPointer;
    DPointer  *d;
};

#endif // V_LIDAR_WINDOW_H
