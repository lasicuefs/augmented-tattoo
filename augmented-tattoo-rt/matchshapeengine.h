#ifndef MATCHSHAPEENGINE_H
#define MATCHSHAPEENGINE_H

#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "blob.h"

using namespace std;
using namespace cv;

class MatchShapeEngine {
public:

    MatchShapeEngine() {
        vector<string> baseImgs;

        // TODO: Remove this hardcoded paths
        //baseImgs.push_back("data/match/L1.png");
        baseImgs.push_back("match/L2.png"); baseThrs.push_back(0.35);
        baseImgs.push_back("match/L3.png"); baseThrs.push_back(0.35);
        baseImgs.push_back("match/L9.png"); baseThrs.push_back(0.35);
        baseImgs.push_back("match/L10.png"); baseThrs.push_back(0.35);
//        baseImgs.push_back("data/match/L11.png"); baseThrs.push_back(0.15);
//        baseImgs.push_back("data/match/L12.png"); baseThrs.push_back(0.15);
//        baseImgs.push_back("data/match/L8.png");

        calcBaseContours(baseImgs);
    }

    MatchShapeEngine(const vector<string> &imgs){
        calcBaseContours(imgs);
    }

    /// Extract base contours from the images
    void calcBaseContours(const vector<string> &imgs) {
        for(int i = 0; i < imgs.size(); i++){
            // This is the shape we will check against
            vector< vector<Point> > tmpContour;
            Mat base = imread(imgs[i]);
            getShape(base, tmpContour);
            baseContours.push_back( tmpContour[1] );
        }

    }

    /// Get the contours in the image
    void getShape(Mat img, vector< vector<Point> > &contours) {
        Mat tmpBinaryImage = img.clone();
        if(tmpBinaryImage.channels() == 3) cvtColor(img, tmpBinaryImage, COLOR_BGR2GRAY);

        // Get the contours
        vector <Vec4i> hierarchy;
        findContours(tmpBinaryImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS);
    }

    /// Use the I3 algorithm
    float getMinI3(const vector<Point> &inputContour) {
        float minI = (float) INT_MAX;

        for(int i = 0; i < baseContours.size(); i++) {
            minI = fmin( minI, matchShapes(baseContours[i], inputContour, CV_CONTOURS_MATCH_I3, 0) );
        }

        return minI;
    }

    vector< vector<Point> > baseContours;
    vector<double> baseThrs;
};

class LShapeFilter {
public:
    static void process(const vector< vector<Point> > &candidates,
                 vector< vector<Point> > &selection,
                 MatchShapeEngine &matchShapeEngine, double thr = 0.6){
        // Now we can find L shape candidates
        for(int i = 0; i < candidates.size(); i++){
            Blob b (candidates[i], i);
            bool passed = true;

            // First we try some lightweight filters
            if(b.solidity > 0.92) passed = false;
            if(b.extent > 0.95) passed = false;

            // Now we try to filter by matching shapes
            double i3 = matchShapeEngine.getMinI3(candidates[i]);
            if(i3 > thr) passed = false;


            if(passed) selection.push_back(candidates[i]);
        }
    }
};

#endif // MATCHSHAPEENGINE_H
