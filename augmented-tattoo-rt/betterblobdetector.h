#ifndef BETTERBLOBDETECTOR_H
#define BETTERBLOBDETECTOR_H

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
//http://stackoverflow.com/questions/13534723/how-to-get-extra-information-of-blobs-with-simpleblobdetector

class BetterBlobDetector : public cv::SimpleBlobDetector
{
public:

    BetterBlobDetector(const cv::SimpleBlobDetector::Params &parameters = cv::SimpleBlobDetector::Params());

    const std::vector < std::vector<cv::Point> > getContours();

    virtual void findBlobs(const cv::Mat &image, const cv::Mat &binaryImage, std::vector<cv::KeyPoint>& keypoints, std::vector < std::vector<cv::Point> >&contours, int csign = 1) const;
protected:
    virtual void detectImpl( const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, const cv::Mat& mask=cv::Mat()) const;
    virtual void findBlobs(const cv::Mat &image, const cv::Mat &binaryImage,
                           std::vector<Center> &centers, std::vector < std::vector<cv::Point> >&contours) const;

};

#endif // BETTERBLOBDETECTOR_H
