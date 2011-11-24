#include "v_lidar_window.h"
#include "vlidar/ui_v_lidar_window.h"
#include "v_lidar_motion_detector.h"
#include <QTimer>
#include <QString>
#include <opencv2/core/core.hpp>
#include <QDebug>
#include <QFileDialog>
#include <QErrorMessage>
#include <QSplitter>
#include <QGraphicsTextItem>
#include <QPainter>

#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#ifdef __cplusplus
extern "C"{
#include <c_urg/urg_ctrl.h>
}
#endif

#define TIMER_TIMEOUT_MS 100

const int VLidarWindow::MEASURMENTS_NUMBER = 800;
const qreal VLidarWindow::START_PHI = -120;
const qreal VLidarWindow::FINAL_PHI = 120;
const qreal VLidarWindow::PHI_SCALE=0.36*3.14/180;
const qreal VLidarWindow::LIDAR_DISTANCE_SCALE=1000;

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
    double *m_phi;
    double *m_radius;
    double *m_x;
    double *m_y;

    cv::Mat m_image2D;
    VLidarMotionDetector m_detector;
    QErrorMessage m_errorMessage;
    QSplitter m_imageSplitter;

    QwtPlot *m_polarPlot;
    QwtPlot *m_signalPlot;
    QwtPlotCurve *m_signalCurve;
    QwtPlotCurve *m_polarCurve;

    static const char START_LOGGING[];
    static const char STOP_LOGGING[];
    static const char FILE_ERROR[];

};

const char VLidarWindow::DPointer::START_LOGGING[] = "Start logging.";
const char VLidarWindow::DPointer::STOP_LOGGING[] = "Stop logging.";
const char VLidarWindow::DPointer::FILE_ERROR[] = "Can't open the file.";

VLidarWindow::DPointer::DPointer(VLidarWindow *lidarWindow):
    m_lidar(0),
    ui(new Ui::VLidarWindow),
    m_timer(new QTimer(lidarWindow)),
    m_storage(new long[VLidarWindow::MEASURMENTS_NUMBER]),

    m_errorMessage(lidarWindow),
    m_imageSplitter(Qt::Vertical),

    m_polarPlot(new QwtPlot),
    m_signalPlot(new QwtPlot),
    m_signalCurve(new QwtPlotCurve),
    m_polarCurve(new QwtPlotCurve),

    m_phi(new double[VLidarWindow::MEASURMENTS_NUMBER]),
    m_radius(new double[VLidarWindow::MEASURMENTS_NUMBER]),
    m_x(new double[VLidarWindow::MEASURMENTS_NUMBER]),
    m_y(new double[VLidarWindow::MEASURMENTS_NUMBER])
{
    for (int i=0; i<VLidarWindow::MEASURMENTS_NUMBER; i++){
        m_phi[i] = i*PHI_SCALE;
    }
};

VLidarWindow::DPointer::~DPointer()
{
    delete m_lidar;
    delete ui;
    delete m_timer;
    delete[] m_storage;

    delete[] m_phi;
    delete[] m_radius;
    delete[] m_x;
    delete[] m_y;

//By default all attached items are deleted in destructor of plot
//    delete m_signalCurve;
//    delete m_polarCurve;
    delete m_polarPlot;
    delete m_signalPlot;
};


VLidarWindow::VLidarWindow(QWidget *parent) :
    QDialog(parent),
    d(new DPointer(this))
{
    d->ui->setupUi(this);
    connect(d->m_timer, SIGNAL(timeout()), this, SLOT(updateLidarGraphics()));
    connect(d->ui->m_saveLogButton, SIGNAL(clicked()), this, SLOT(enableWriteToFile()));
    connect(d->ui->m_connectButton, SIGNAL(clicked()), this, SLOT(connectToLidar()));
    connect(d->ui->m_disconnectButton, SIGNAL(clicked()), this, SLOT(disconnectFromLidar()));
    connect(d->ui->m_exitButton, SIGNAL(clicked()), this, SLOT(close()));

    d->m_timer->start(TIMER_TIMEOUT_MS);

    // Add gui elements
    setMinimumSize(MIN_WINDOW_WITH,MIN_WINDOW_HEIGHT);
    d->m_polarPlot->setTitle(tr("Lidar signal in polar coordinates"));
    d->m_polarPlot->setTitle(tr("Lidar signal"));

    d->m_imageSplitter.addWidget(d->m_polarPlot);
    d->m_imageSplitter.addWidget(d->m_signalPlot);
    d->m_signalCurve->attach(d->m_signalPlot);

    d->m_polarCurve->setStyle(QwtPlotCurve::NoCurve);
    d->m_polarCurve->setSymbol(new QwtSymbol ( QwtSymbol::Rect, Qt::NoBrush,
                                              QPen( Qt::darkMagenta ), QSize( 5, 5 ) ) );
    d->m_polarCurve->attach(d->m_polarPlot);

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
        if(result >=0){
            d->ui->m_connectButton->setEnabled(false);
            d->ui->m_disconnectButton->setEnabled(true);
        }else{
            d->ui->m_connectButton->setEnabled(true);
            d->ui->m_disconnectButton->setEnabled(false);
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
        d->ui->m_connectButton->setEnabled(true);
        d->ui->m_disconnectButton->setEnabled(false);
    }
}

bool VLidarWindow::isConnectedToLidar()
{
    return (d->m_lidar);
}

bool VLidarWindow::updateLidar()
{
    if(d->m_lidar){
        urg_requestData(d->m_lidar, URG_GD, URG_FIRST, URG_LAST);
        if( urg_receiveData(d->m_lidar, d->m_storage, MEASURMENTS_NUMBER ) >0 ){
            d->m_detector.setData(d->m_storage);

            for(int i=0; i<VLidarWindow::MEASURMENTS_NUMBER; i++)
            {
                d->m_radius[i]=d->m_storage[i]/LIDAR_DISTANCE_SCALE;
                d->m_x[i]=d->m_radius[i]*cos(d->m_phi[i]);
                d->m_y[i]=d->m_radius[i]*sin(d->m_phi[i]);
            }
            return true;
        }
    }
    return false;
}

void VLidarWindow::updateLidarGraphics()
{
    if(isConnectedToLidar() && updateLidar()){
        drawSignal();
        drawSignal2D();
    }
}

void VLidarWindow::drawSignal2D()
{
    d->m_polarCurve->setRawSamples(d->m_x, d->m_y, VLidarWindow::MEASURMENTS_NUMBER);
}

void VLidarWindow::drawSignal()
{
    d->m_signalCurve->setRawSamples(d->m_phi, d->m_radius, VLidarWindow::MEASURMENTS_NUMBER);
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
