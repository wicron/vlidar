#include <v_lidar_motion_detector.h>
#include <opencv2/core/core.hpp>

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
