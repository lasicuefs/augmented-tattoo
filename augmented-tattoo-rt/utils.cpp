#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef _WIN32
#include <dirent.h>
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "utils.h"

using namespace cv;
using namespace std;

/**
 * @brief convertCaptureToImages Output some VideoCapture as a set of images
 * @param cap
 * @param outputFolder
 * @param extension
 */
void convertCaptureToImages(VideoCapture cap, const string& outputFolder, const string& extension){
    Mat capturedImg;
    int frame = 0;
    while(cap.read(capturedImg)){
        cout << "frame " << frame << endl;
        stringstream ss;
        ss << outputFolder << "/frame" << frame << extension;

        imwrite(ss.str(), capturedImg);
        frame++;
    }
}

/**
 * @brief findBiggestContour Find the biggest contour in "contours"
 * @param contours
 * @return
 */
int findBiggestContour(vector<vector<Point> > &contours){
    int indexOfBiggestContour = -1;
    int sizeOfBiggestContour = 0;
    for (int i = 0; i < contours.size(); i++){
        double area = contourArea(contours[i]);
        if(area > sizeOfBiggestContour){
            sizeOfBiggestContour = area;
            indexOfBiggestContour = i;
        }
    }
    return indexOfBiggestContour;
}

/**
 * @brief singleChannelHist Calculate and make a histogram image of a image channel
 * @param src The input image
 * @param hist The histogram (will be saved here)
 * @param bins Number of bins
 * @param range Range of the channel (0-255 is the most common)
 * @param mask A mask
 * @param color The color used in the image
 * @param winSize The size of the image
 * @param thickness The thickness of the histogram line
 * @return The generated image
 */
Mat singleChannelHist(Mat src, Mat &hist, int bins, float* range, Mat mask,
                      const Scalar &color, const Size &winSize, int thickness){
    const float* histRange = { range };

    bool uniform = true;
    bool accumulate = false;

    /// Compute the histograms:
    calcHist( &src, 1, 0, Mat(), hist, 1, &bins, &histRange, uniform, accumulate );


    Mat histImage( winSize.height, winSize.width, CV_8UC3, Scalar(0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
    normalize( hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

    // Draw the histograms for B, G and R
    int bin_w = cvRound( (double) winSize.width / bins );

    for( int i = 1; i < bins; i++ ){
        line( histImage, Point( bin_w*(i-1), (winSize.height-1) - cvRound(hist.at<float>(i-1)) ) ,
                         Point( bin_w*(i), (winSize.height-1) - cvRound(hist.at<float>(i)) ),
                         color, thickness, 8, 0 );
    }

    return histImage;
}

/**
 * @brief generateHistogram Generate a histogram image
 * @param src The 3-channel image in the respective color space
 * @param rangeA
 * @param rangeB
 * @param rangeC
 * @return
 */
Mat generateHistogram(Mat src, float* rangeA, float* rangeB, float* rangeC){
    // Separate the image in 3 places ( B, G and R )
    vector<Mat> channels;
    split( src, channels );

    // Get histogram for each channel
    Mat histA;
    Mat histImageA = singleChannelHist(channels[0], histA, rangeA[1], rangeA, Mat(), Scalar(255, 0, 0), Size(rangeA[1], 128));

    Mat histB;
    Mat histImageB = singleChannelHist(channels[1], histB, rangeB[1], rangeB, Mat(), Scalar(0, 255, 0), Size(rangeB[1], 128));

    Mat histC;
    Mat histImageC = singleChannelHist(channels[2], histC, rangeC[1], rangeC, Mat(), Scalar(0, 0, 255), Size(rangeC[1], 128));

    // Combine the histograms in a single image
    vector<Mat> imgs;
    imgs.push_back(histImageA);
    imgs.push_back(histImageB);
    imgs.push_back(histImageC);
    combine(imgs, 10, false);

    return combine(imgs, 10, false);
}

/**
 * @brief calcHistSample Calculate and display the histogram of
 * the given image and mask
 * @param src
 * @param mask
 */
void calcHistSample(Mat src, Mat mask, const string &windowName){
    /// Separate the image in 3 places ( B, G and R )
     vector<Mat> bgr_planes;
     split( src, bgr_planes );


     /// Establish the number of bins
     int histSize = 256;
     int hueSize = 180;

     /// Set the ranges ( for B,G,R) )
     float range180[] = { 0, 179 } ;
     float range256[] = { 0, 255 } ;

     const float* histRange180 = { range180 };
     const float* histRange256 = { range256 };

     bool uniform = true; bool accumulate = false;

     Mat b_hist, g_hist, r_hist;

     /// Compute the histograms:
     calcHist( &bgr_planes[0], 1, 0, mask, b_hist, 1, &hueSize, &histRange180, uniform, accumulate );
     calcHist( &bgr_planes[1], 1, 0, mask, g_hist, 1, &histSize, &histRange256, uniform, accumulate );
     calcHist( &bgr_planes[2], 1, 0, mask, r_hist, 1, &histSize, &histRange256, uniform, accumulate );

     // Draw the histograms for B, G and R
     int hist_w = 256; int hist_h = 128;
     int bin_w = cvRound( (double) hist_w/histSize );
     int bin_wHue = cvRound( (double) 180.0/hueSize );


     Mat hueImage( hist_h, 180, CV_8UC3, Scalar( 0,0,0) );
     Mat satImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
     Mat valImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

     /// Normalize the result to [ 0, histImage.rows ]
     normalize(b_hist, b_hist, 0, hueImage.rows, NORM_MINMAX, -1, Mat() );
     normalize(g_hist, g_hist, 0, satImage.rows, NORM_MINMAX, -1, Mat() );
     normalize(r_hist, r_hist, 0, valImage.rows, NORM_MINMAX, -1, Mat() );

     /// Draw for each channel
     for( int i = 1; i < histSize; i++ ){
         line( hueImage, Point( bin_wHue*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                          Point( bin_wHue*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                          Scalar( 255, 0, 0), 1, 8, 0  );
         line( satImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                          Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                          Scalar( 0, 255, 0), 1, 8, 0  );
         line( valImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                          Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                          Scalar( 0, 0, 255), 1, 8, 0  );
     }

     /// Display
     imshow(windowName + "-hue", hueImage );
     imshow(windowName + "-saturation", satImage );
     imshow(windowName + "-value", valImage );
}

/**
 * @brief combine Combines several images in one single image
 * @param imgs Vector of images to be combined
 * @param margin Margin to be used between each image
 * @param horizontally Whether the arrangement will be horizontal or vertical
 * @return The combined image
 */
Mat combine(vector<Mat> &imgs, int margin, bool horizontally){
    int maxWidth = 0;
    int maxHeight = 0;

    int sumWidth = margin;
    int sumHeight = margin;

    for(int i = 0; i < imgs.size(); i++){
        sumWidth += imgs[i].cols + margin;
        sumHeight += imgs[i].rows + margin;

        maxWidth = max(maxWidth, imgs[i].cols);
        maxHeight = max(maxHeight, imgs[i].rows);
    }

    Mat combined;
    if(horizontally) combined = Mat::zeros(maxHeight, sumWidth, CV_8UC3);
    else combined = Mat::zeros(sumHeight, maxWidth, CV_8UC3);

    int currentX = margin;
    for(int i = 0; i < imgs.size(); i++){
        if(horizontally){
            int center = (maxHeight - imgs[i].rows)/2;
            Mat current(combined, Rect(currentX, center, imgs[i].cols, imgs[i].rows));
            currentX += imgs[i].cols + margin;
            imgs[i].copyTo(current);
        }
        else {
            int center = (maxWidth - imgs[i].cols)/2;
            Mat current(combined, Rect(center, currentX, imgs[i].cols, imgs[i].rows));
            currentX += imgs[i].rows + margin;
            imgs[i].copyTo(current);
        }
    }

    return combined;
}


/**
 * @brief hsvInRange A patch to inRange function to fix the circular nature of hue
 * @param hsv
 * @param minHSV
 * @param maxHSV
 * @return
 */
Mat hsvInRange(Mat hsv, Scalar minHSV, Scalar maxHSV){
    Mat seg;

    // Segment the image using the values
    inRange(hsv, minHSV, maxHSV, seg);

    // The Hue channel is circular, so if we find negative values we should put on the other end
    Mat otherEnd;
    inRange(hsv, Scalar(minHSV[0] < 0 ? 180 + minHSV[0] : 0, minHSV[1], minHSV[2]), Scalar(180, maxHSV[1], maxHSV[2]), otherEnd);

    // Combine both ends
    seg = otherEnd | seg;

    return seg;
}

static double rad2deg(double rad){return rad*(180/M_PI);}//Convert radians to degrees
static double deg2rad(double deg){return deg*(M_PI/180);}//Convert degrees to radians

/* Calculate the mean and standart deviation of a circular
 * channel (hue, for instance), given a mask. */
void meanStdDevCircular(Mat img, Mat mask, double &mean, double &stdd){
    double sumSin = 0;
    double sumCos = 0;

    long total = 0;

    for (int row = 0; row < img.rows; ++row){
        for (int col = 0; col < img.cols; ++col){
            if((float)mask.at<uchar>(row, col) > 0){
                float cur = (float)img.at<uchar>(row, col);
                float angle = cur + cur;
                sumSin += sin(deg2rad(angle));
                sumCos += cos(deg2rad(angle));

                total++;
            }
        }
    }

    if(sumCos == 0 || total == 0) return;

    sumSin /= total;
    sumCos /= total;

    mean = rad2deg(atan2(sumSin, sumCos)) / 2.0;
    //if(mean < 0) mean = 180 + mean;

    stdd = rad2deg(sqrt(-log(sumSin*sumSin + sumCos*sumCos)));
}

#ifndef _WIN32
void getDirFiles(const string &path, vector<string> &paths){
    DIR *dir;
    struct dirent *lsdir;

    dir = opendir(path.c_str());

    /* Iterate over all the files and directories within directory that ends with .jpg or .png */
    while ( ( lsdir = readdir(dir) ) != NULL ) {
        string dname (lsdir->d_name);

        if(dname.length() > 4){
            string ext = dname.substr(dname.length()-4);
            if(ext == ".png" || ext == ".jpg"){
                paths.push_back(path + "/" + dname);
            }
        }
    }

    closedir(dir);
}
#endif

double getMinAngle(Vec2f a, Vec2f b){
    a = normalize(a);
    b = normalize(b);

    return acos(a.dot(b));
}

double getAngle(Vec2f a, Vec2f b){
    a = normalize(a);
    b = normalize(b);

    Vec2f c (-a[1], a[0]);

    double s = c.dot(b);
    double angle = acos(a.dot(b));

    if(s < 0) angle = -angle + 2*CV_PI ;

    return angle;
}

bool insideMargin(double value, double base, double thr){
    if(value < base - thr || value > base + thr) return false;
    return true;
}

void getMaxDefect(vector<Vec4i> &defects, int &maxDepthIndex, float &maxDepth){
    for(int i = 0; i < defects.size(); i++){
        Vec4i& v = defects[i];
        float depth = (float)v[3] / 256.0;
        if(depth > maxDepth){
            maxDepth = depth;
            maxDepthIndex = i;
        }
    }
}

bool hasAnotherBigDefect(vector<Vec4i> &defects, int maxDepthIndex, float maxDepth, float thr){
    for(int i = 0; i < defects.size(); i++){
        if(i == maxDepthIndex) continue;
        Vec4i& v = defects[i];
        float depth = (float)v[3] / 256.0;

        if(depth / maxDepth > thr) return true;
    }
    return false;
}

/// Given a point, return the direction of the biggest defect
Vec2f getMaxDefectDirection(vector<Point> &points/*, Mat blobsImg*/){
    // Get the hull filled with index to get its defects
    vector<int> hull;
    vector<Vec4i> defects;
    convexHull(points, hull);
    convexityDefects(points, hull, defects);

    if(defects.size() == 0) return Vec2f(0,0);

    // Find the biggest defect and save its index
    int maxDepthIndex = 0;
    float maxDepth = 0;
    for(int i = 0; i < defects.size(); i++){
        Vec4i& v = defects[i];
        float depth = v[3] / 256;

        if(depth > maxDepth){
            maxDepth = depth;
            maxDepthIndex = i;
        }
    }

    // Get the key points of the defect
    Vec4i& v = defects[maxDepthIndex];
    Point ptStart( points[v[0]] );
    Point ptEnd( points[v[1]] );
    Point ptFar( points[v[2]] );

    // Get the direction of the defect
    Point s = (ptStart - ptFar) + (ptEnd - ptFar);
    Vec2f direction = normalize(Vec2f(s.x, s.y));

//    // Draws in a debug image the positions of the defect
//    circle(blobsImg, ptStart, 1, 64, 1);
//    circle(blobsImg, ptEnd, 1, 64, 1);
//    circle(blobsImg, ptFar, 1, 64, 1);

    return direction;
}
