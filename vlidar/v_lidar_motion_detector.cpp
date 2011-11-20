#include <v_lidar_motion_detector.h>
#include <opencv2/core/core.hpp>
#include <QDebug>
#include <QString>

VLidarMotionDetector::VLidarMotionDetector()
{
};

void VLidarMotionDetector::setData(const long* data)
{

}

bool VLidarMotionDetector::findMovingObjects(std::list<VLidarObject> &movingObjects)
{

}

bool VLidarMotionDetector::findMovingObjects(std::list<VLidarSegment> &movingSegments)
{

}

const long* VLidarMotionDetector::getFilteredSignal()
{
    return 0;
}

void VLidarMotionDetector::segmentate(const cv::Mat &data, std::list<VLidarSegment> &segments)
{

}

bool VLidarMotionDetector::findSegmentMotion(const cv::Mat &oldSignal, const cv::Mat &newSignal,
                       VLidarSegment &segment)
{

}

void VLidarMotionDetector::closeFile()
{
    if(m_log.isOpen()){
        qDebug()<< "stop write to file";
        m_log.close();
    }
}

void VLidarMotionDetector::writeToFile()
{
    if(m_log.isOpen()){
        qDebug()<< "writing to file ...";
    }
}

void VLidarMotionDetector::openFile(const QString &newFleName)
{
    if(newFleName.length())
    {
        qDebug()<< "start write to file";
        m_log.setFileName(m_fileName);
        m_log.open(QIODevice::ReadWrite);
    }
}

bool VLidarMotionDetector::isFileOpened()
{
    return m_log.isOpen();
}
