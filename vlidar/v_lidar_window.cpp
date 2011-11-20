#include "v_lidar_window.h"
#include "vlidar/ui_v_lidar_window.h"
#include "v_lidar_motion_detector.h"
#include <QTimer>
#include <QString>
#include <c_urg/urg_ctrl.h>
#include <opencv2/core/core.hpp>
#include <QDebug>

#define TIMER_TIMEOUT_MS 100

class VLidarWindow::DPointer
{
    public:
    DPointer(VLidarWindow *lidarWindow);
    ~DPointer();

public:
    urg_t *m_lidar;
    Ui::VLidarWindow *ui;
    QTimer *m_timer;
    long *m_storage;
    cv::Mat m_image2D;
    VLidarMotionDetector m_detector;

    static const int STORAGE_SIZE = 500;
};

VLidarWindow::DPointer::DPointer(VLidarWindow *lidarWindow):
    m_lidar(new urg_t),
    ui(new Ui::VLidarWindow),
    m_timer(new QTimer(lidarWindow)),
    m_storage(new long[STORAGE_SIZE])
{
};

VLidarWindow::DPointer::~DPointer()
{
    delete m_lidar;
    delete ui;
    delete m_timer;
    delete m_storage;
};


VLidarWindow::VLidarWindow(QWidget *parent) :
    d(new DPointer(this))
{
    d->ui->setupUi(this);
    connect(d->m_timer, SIGNAL(timeout()), this, SLOT(updateLidarGraphics()));
    d->m_timer->start(TIMER_TIMEOUT_MS);
}

VLidarWindow::~VLidarWindow()
{
    delete d;
}

void VLidarWindow::connectToLidar()
{
    if(!d->m_lidar)
    {
        d->m_lidar = new urg_t;
        urg_initialize(d->m_lidar);
        int result = urg_connect(d->m_lidar, d->ui->m_lidarName->text().toAscii().data(),
                                 d->ui->m_baudRate->text().toLong());
        if(result <0)
        {
            delete d->m_lidar;
            d->m_lidar = 0;
        }
    }
}

void VLidarWindow::disconnectFromLidar()
{
    if(d->m_lidar)
    {
        urg_disconnect(d->m_lidar);
        delete d->m_lidar;
        d->m_lidar =0;
    }
}

void VLidarWindow::updateLidar()
{
    if(d->m_lidar)
    {
        if(!urg_requestData(d->m_lidar, URG_GD, URG_FIRST, URG_LAST))
        {
            if( urg_receiveData(d->m_lidar, d->m_storage, DPointer::STORAGE_SIZE ) >0 ){
            }
        }
    }
    d->m_detector.setData(d->m_storage);
}

void VLidarWindow::updateLidarGraphics()
{
    updateLidar();
    drawSignal();
    drawSignal2D();
}

void VLidarWindow::drawSignal2D()
{

}

void VLidarWindow::drawSignal()
{
    const long* signal = d->m_detector.getFilteredSignal();

    //Fix me:
    //Draw signal
    // I think it's better to use QGraphicsScene::addLine there.
}

void VLidarWindow::writeToFile()
{

}
