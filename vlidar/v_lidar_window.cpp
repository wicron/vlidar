#include "v_lidar_window.h"
#include "vlidar/ui_v_lidar_window.h"
#include "v_lidar_motion_detector.h"
#include <QTimer>
#include <QString>
#include <c_urg/urg_ctrl.h>
#include <opencv2/core/core.hpp>
#include <QDebug>
#include <QFileDialog>
#include <QErrorMessage>
#include <QSplitter>

#define TIMER_TIMEOUT_MS 10000

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
    QErrorMessage m_errorMessage;
    QSplitter m_imageSplitter;

    static const char START_LOGGING[];
    static const char STOP_LOGGING[];
    static const char FILE_ERROR[];

    static const int STORAGE_SIZE = 500;
};

const char VLidarWindow::DPointer::START_LOGGING[] = "Start logging.";
const char VLidarWindow::DPointer::STOP_LOGGING[] = "Stop logging.";
const char VLidarWindow::DPointer::FILE_ERROR[] = "Can't open the file.";

VLidarWindow::DPointer::DPointer(VLidarWindow *lidarWindow):
    m_lidar(0),
    ui(new Ui::VLidarWindow),
    m_timer(new QTimer(lidarWindow)),
    m_storage(new long[STORAGE_SIZE]),
    m_errorMessage(lidarWindow),
    m_imageSplitter(Qt::Vertical)
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
    QDialog(parent),
    d(new DPointer(this))
{
    d->ui->setupUi(this);
    connect(d->m_timer, SIGNAL(timeout()), this, SLOT(updateLidar()));
    connect(d->m_timer, SIGNAL(timeout()), this, SLOT(updateLidarGraphics()));
    connect(d->ui->m_saveLogButton, SIGNAL(clicked()), this, SLOT(enableWriteToFile()));
    connect(d->ui->m_connectButton, SIGNAL(clicked()), this, SLOT(connectToLidar()));
    connect(d->ui->m_disconnectButton, SIGNAL(clicked()), this, SLOT(disconnectFromLidar()));
    connect(d->ui->m_exitButton, SIGNAL(clicked()), this, SLOT(close()));

    d->m_timer->start(TIMER_TIMEOUT_MS);

// Add gui elements
    setMinimumSize(MIN_WINDOW_WITH,MIN_WINDOW_HEIGHT);
    d->m_imageSplitter.addWidget(d->ui->m_pictureViewer);
    d->m_imageSplitter.addWidget(d->ui->m_signalViewer);
    d->ui->m_verticalLayout->addWidget(&(d->m_imageSplitter));

    setLayout(d->ui->m_mainLayout);
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

bool VLidarWindow::isConnectedToLidar()
{
    return (d->m_lidar);
}

void VLidarWindow::updateLidar()
{
    if(d->m_lidar){
        if(!urg_requestData(d->m_lidar, URG_GD, URG_FIRST, URG_LAST)){
            if( urg_receiveData(d->m_lidar, d->m_storage, DPointer::STORAGE_SIZE ) >0 ){
                d->m_detector.setData(d->m_storage);
            }
        }
    }
}

void VLidarWindow::updateLidarGraphics()
{
    if(isConnectedToLidar()){
        drawSignal();
        drawSignal2D();
    }
}

void VLidarWindow::drawSignal2D()
{
    qDebug() << "Fix me: draw signal 2D";
}

void VLidarWindow::drawSignal()
{
    qDebug() << "Fix me: draw signal";
}

void VLidarWindow::enableWriteToFile()
{
    if(!d->m_detector.isFileOpened())
    {
        QString fileName = QFileDialog::getOpenFileName(this,
                tr("Log file"), "");
        d->ui->m_fileNameLine->setText(fileName);
        d->m_detector.openFile(fileName);

        if(d->m_detector.isFileOpened()){
            d->ui->m_saveLogButton->setText(tr(DPointer::STOP_LOGGING));
        }else{
            d->m_errorMessage.showMessage(tr(DPointer::FILE_ERROR));
        }

    }else{
        d->m_detector.closeFile();
        d->ui->m_saveLogButton->setText(tr(DPointer::START_LOGGING));
    }
}
