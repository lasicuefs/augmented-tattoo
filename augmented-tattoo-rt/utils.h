#ifndef COMMON_H
#define COMMON_H

#include <iostream>

using namespace std;
using namespace cv;

static float range256[] = {0, 256};
static float range180[] = {0, 180};

void convertCaptureToImages(VideoCapture cap, const string& outputFolder, const string& extension = ".png");

int findBiggestContour(vector<vector<cv::Point> > &contours);

Mat generateHistogram(Mat src, float* rangeA = range256, float* rangeB = range256, float* rangeC = range256);

Mat singleChannelHist(Mat src, Mat &hist, int bins, float* range, Mat mask = Mat(),
                      const Scalar &color = Scalar::all(255),
                      const Size &winSize = Size(256, 128), int thickness = 1);

void calcHistSample(Mat src, Mat mask = Mat(), const string &windowName = "HistSample");

Mat combine(vector<Mat> &imgs, int margin = 10, bool horizontally = true);

Mat hsvInRange(Mat hsv, Scalar minHSV, Scalar maxHSV);

void meanStdDevCircular(Mat img, Mat mask, double &mean, double &stdd);

void getDirFiles(const string &path, vector<string> &paths);

/// Angle between two vectors, from 0 to 360 (degrees)
double getAngle(Vec2f a, Vec2f b);
double getMinAngle(Vec2f a, Vec2f b);
inline Vec2f p2v(const Point &p){ return Vec2f(p.x, p.y); }
inline Point v2p(const Vec2f &v){ return Point(v[0], v[1]); }

bool insideMargin(double value, double base, double thr);


void getMaxDefect(vector<Vec4i> &defects, int &maxDepthIndex, float &maxDepth);

bool hasAnotherBigDefect(vector<Vec4i> &defects, int maxDepthIndex, float maxDepth, float thr);

/// Given a point, return the direction of the biggest defect
Vec2f getMaxDefectDirection(vector<Point> &points);





class CDist {
public:
    double dist;
    int from;
    int to;

    CDist(){}

    CDist(double dist, int from, int to){
        this->dist = dist;
        this->from = from;
        this->to = to;
    }

    static int compare(const void *x, const void *y){
        CDist xx = *(CDist*)x, yy = *(CDist*)y;
        if (xx.dist < yy.dist) return -1;
        if (xx.dist > yy.dist) return  1;
        return 0;
    }

    static int compareR(const CDist &x, const CDist &y){
        return x.dist < y.dist;
    }

};


#endif // COMMON_H
