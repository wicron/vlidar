#include "v_lidar_window.h"
#include "vlidar/ui_v_lidar_window.h"
//#include "v_lidar_motion_detector.h"
#include <opencv2/core/core.hpp>

#include <QTimer>
#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QErrorMessage>
#include <QSplitter>
#include <QFile>

#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include "v_lidar.h"

#define TIMER_TIMEOUT_MS 200

class VLidarWindow::DPointer
{
public:
    DPointer(VLidarWindow *lidarWindow);
    ~DPointer();

public:
    Ui::VLidarWindow *ui;
    QTimer *m_timer;

    VLidar m_lidar;

    QErrorMessage m_errorMessage;
    QSplitter m_imageSplitter;

    QwtPlot *m_polarPlot;
    QwtPlot *m_signalPlot;
    QwtPlotCurve *m_signalCurve;
    QwtPlotCurve *m_polarCurve;

    QFile m_log;
    QTextStream m_logStream;
};

VLidarWindow::DPointer::DPointer(VLidarWindow *lidarWindow):
    ui(new Ui::VLidarWindow),
    m_timer(new QTimer(lidarWindow)),

    m_errorMessage(lidarWindow),
    m_imageSplitter(Qt::Vertical),
    m_polarPlot(new QwtPlot),
    m_signalPlot(new QwtPlot),
    m_signalCurve(new QwtPlotCurve),
    m_polarCurve(new QwtPlotCurve)
{
};

VLidarWindow::DPointer::~DPointer()
{
    delete ui;
    delete m_timer;

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
    d->m_lidar.connect(d->ui->m_lidarName->text(), d->ui->m_baudRate->text().toLong());
    updateButtons();
}

void VLidarWindow::updateButtons() {
    bool connected = d->m_lidar.connected();

    d->ui->m_connectButton->setEnabled(!connected);
    d->ui->m_disconnectButton->setEnabled(connected);
}

void VLidarWindow::disconnectFromLidar()
{
    d->m_lidar.disconnect();
    updateButtons();
}

bool VLidarWindow::isConnectedToLidar()
{
    return d->m_lidar.connected();
}

bool VLidarWindow::updateLidar()
{
    return d->m_lidar.update();
}

void VLidarWindow::updateLidarGraphics()
{
    writeLogToFile();
    if(isConnectedToLidar() && updateLidar()){
        drawSignal();
        drawSignal2D();
    }
}

void VLidarWindow::drawSignal2D()
{
    d->m_polarCurve->setSamples(d->m_lidar.cartesian());
    d->m_polarPlot->replot();
}

void VLidarWindow::drawSignal()
{
    d->m_signalCurve->setSamples(d->m_lidar.angles(), d->m_lidar.distances());
    d->m_signalPlot->replot();
}


void VLidarWindow::enableWriteToFile()
{
    if(!d->m_log.isOpen())
    {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Log file"), "");
        d->ui->m_fileNameLine->setText(fileName);
        d->m_log.setFileName(fileName);
        d->m_log.open(QIODevice::ReadWrite);

        if(d->m_log.isOpen()){
            d->ui->m_saveLogButton->setText(tr("Stop logging"));
            d->m_logStream.setDevice(&d->m_log);
        }else{
            d->m_errorMessage.showMessage(tr("Can't open the file"));
        }

    }else{
        d->m_log.close();
        d->ui->m_saveLogButton->setText(tr("Start logging"));
    }
}

void VLidarWindow::writeLogToFile()
{
    if(d->m_log.isOpen()){
        d->m_logStream << "<scan>" << '\n';
        const QVector<double> &angles = d->m_lidar.angles();
        const QVector<double> &distances = d->m_lidar.distances();

        for(int i = 0; i < angles.size(); i++) {
            d->m_logStream << angles[i] << " " << distances[i] << '\n';
        }
        d->m_logStream << "</scan>" << '\n';
    }
}
