#ifndef IMAGEWARPER_H
#define IMAGEWARPER_H

#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

class ImageWarper {
public:

    static Mat getPerspective(const vector<Blob> &centralMarkers, float width, float height,
                       float offw = 0, float offh = 0, Scalar errorCorrection = Scalar(0, 0)){

        if(centralMarkers.size() < 4) return Mat();

        vector<Point2f> src;
        src.push_back(centralMarkers[3].massCenter);
        src.push_back(centralMarkers[0].massCenter);
        src.push_back(centralMarkers[1].massCenter);
        src.push_back(centralMarkers[2].massCenter);

        vector<Point2f> dst;
        dst.push_back(Point2f(0+offw, 0+offh));
        dst.push_back(Point2f(width+offw+errorCorrection[0], 0     +offh+errorCorrection[1]));
        dst.push_back(Point2f(width+offw+errorCorrection[2], height+offh+errorCorrection[3]));
        dst.push_back(Point2f(0+offw, height+offh));

        return getPerspectiveTransform(src, dst);
    }

    static Mat getAffine(const vector<Blob> &centralMarkers, float width, float height,
                   float offw = 0, float offh = 0, Scalar errorCorrection = Scalar(0, 0)){

        Point2f src[3];
        src[0] = centralMarkers[3].massCenter;
        src[1] = centralMarkers[1].massCenter;
        src[2] = centralMarkers[2].massCenter;

        Point2f dst[3];
        dst[0] = Point2f(0+offw, 0+offh);
        dst[1] = Point2f(width+offw+errorCorrection[2], height+offh+errorCorrection[3]);
        dst[2] = Point2f(0+offw, height+offh);

        return getAffineTransform(src, dst);
    }

    static void warpImage(const Mat &img, Mat &warpedImg, Mat &M, vector<Blob> &centralMarkers,
                   Size warpImgSize = Size(512, 512), float divs = 5.0f,
                   bool usePerspective = false, Scalar errorFactor = Scalar::all(0)){
    //    // Number of divisions in the image, counting the padding/border
    //    float divs = 5.0f;

    //    // The size of the warped img. Don't have to be the same as image
    //    Size sizeWarpImg = Size(512, 512);

        // The size of each subdivision in the image
        float pw = ((float)warpImgSize.width) / divs;
        float ph = ((float)warpImgSize.height) / divs;

        // The offset to correct the positioning
        float ow = warpImgSize.width * 0.5 * (1.0 - (1.0/divs));
        float oh = warpImgSize.height * 0.5 * (1.0 - (1.0/divs));

        // Get the appropriate transform matrix and warp the image
        if(usePerspective){
            M = getPerspective(centralMarkers, pw, ph, ow, oh, errorFactor);
            if(!M.empty()) warpPerspective(img, warpedImg, M, warpImgSize);
        }
        else {
            M = getAffine(centralMarkers, pw, ph, ow, oh, errorFactor);
            if(!M.empty()) warpAffine(img, warpedImg, M, warpImgSize);
        }
    }
};

#endif // IMAGEWARPER_H
