#ifndef BLOB_H
#define BLOB_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>

using namespace cv;
using namespace std;

double getAngle(Vec2f a, Vec2f b);
#define RAD2DEG (180.0 / CV_PI)

class Blob {
public:
    vector<Point> points;
    int index;
    Moments moments;
    double hu[7];
    Point2f massCenter;

    vector<Point> hull;
    double hullArea;

    Point2f circleCenter;
    float circleRadius;

    double extent;
    double solidity;
    double area;

    double orientation;
    Vec2f direction;

    double _areaOverSolidity;

    Blob(){}
    Blob(vector<Point> points, int index){
        // Save the points and index
        this->points = points;
        this->index = index;

        // Get information of the moments (important)
        this->moments = cv::moments(points);
        HuMoments(moments, hu);

        // Now we get some geometric information
        massCenter = Point2f(moments.m10/moments.m00, moments.m01/moments.m00);
        area = contourArea(points);
        minEnclosingCircle(points, circleCenter, circleRadius);

        // And extract the convex hull
        convexHull(points, hull);
        hullArea = contourArea(hull);

        // And other metrics
        solidity = area / hullArea;
        extent = area / float(circleRadius * circleRadius * CV_PI);
        _areaOverSolidity = area / solidity;


        direction = circleCenter - massCenter;
        direction = normalize(direction);
        orientation = getAngle(Vec2f(0, -1), direction) * RAD2DEG;
    }

    /// Used to sort an array of blobs by solidity
    static int compareBySolidity(const Blob &x, const Blob &y){
        return x.solidity < y.solidity;
    }

    /// Used to sort an array of blobs by index
    static int compareByIndex(const Blob &x, const Blob &y){
        return x.index < y.index;
    }

    /// Used to sort an array of blobs by area
    static int compareByArea(const Blob &x, const Blob &y){
        return x.area < y.area;
    }

    /// Used to sort an array of blobs by radius of the minEnclosingCircle
    static int compareByRadius(const Blob &x, const Blob &y){
        return x.circleRadius < y.circleRadius;
    }

    /// Used to sort an array of blobs by solidity and area
    static int compareBySolidityAndArea(const Blob &x, const Blob &y){
        return x._areaOverSolidity < y._areaOverSolidity;
    }

    /// Used to sort an array of blobs by solidity
    static int compareByOrientation(const Blob &x, const Blob &y){
        return x.orientation < y.orientation;
    }

    // Don't want draw as a method now. Function is nice
    static void drawBlob(Mat &img, const Blob &b, Scalar color, int thickness = 1, int lineType = 8){
        vector< vector<Point> > tmp(1);
        tmp[0] = b.points;
        drawContours(img, tmp, -1, color, thickness, lineType);
    }

    static void drawBlobs(Mat &img, const vector<Blob> &blobs, Scalar color, int thickness = 1, int lineType = 8){
        vector< vector<Point> > tmp(blobs.size());
        for(int i = 0; i < blobs.size(); i++) tmp.push_back(blobs[i].points);
        drawContours(img, tmp, -1, color, thickness, lineType);
    }
};

#endif // BLOB_H
