#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

enum MarkerColor {
    Darker = 1,
    Brighter = -1,
};

class ImageManager {
public:
    int targetHeight = 360;
    float sizeFactor = 1.0f;

    MarkerColor markerColor = MarkerColor::Darker;

    Mat originalImg;
    Mat img;
    Mat grayImg;
    Mat thrImg;

    // Drawing phase
    Mat skinImg;
    Mat inpaintMask;
    Mat inpaintedImg;
    Mat finalImg;
};

#endif // IMAGEMANAGER_H
