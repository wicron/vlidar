#ifndef _VLIDAR_MOTION_DETECTOR_
#define _VLIDAR_MOTION_DETECTOR_

#include <vector>
#include <list>

namespace cv{
class Mat;
}
class VLidarSegment;

struct VLidarObject
{
    VLidarObject(VLidarSegment &segment);

    double r;
    double angle;
    double minAngle;
    double maxAngle;

    double dr;
    double dAngle;
};

class VLidarMotionDetector
{
public:
    enum ImageType{
        RAW_IMAGE,
        SEGMENTS_SELECTED
    };

//    void setData(const cv::Mat &data);
//    void setData(const std::vector<char> &data);
    void setData(const long* data);
    bool findMovingObjects(std::list<VLidarObject> &movingObjects);
    bool findMovingObjects(std::list<VLidarSegment> &movingSegments);
    const long* getFilteredSignal();

protected:
    void segmentate(const cv::Mat &data, std::list<VLidarSegment> &segments);
    bool findSegmentMotion(const cv::Mat &oldSignal, const cv::Mat &newSignal,
                           VLidarSegment &segment);
};

#endif /*_VLIDAR_MOTION_DETECTOR_*/
