#ifndef MARKERMATCHER_H
#define MARKERMATCHER_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "imagemanager.h"
#include "betterblobdetector.h"
#include "imagewarper.h"

using namespace cv;


class HeekDescriptor :  virtual public DescriptorExtractor {
public:
    virtual ~HeekDescriptor(){}
    /*
     * Compute the descriptors for a set of keypoints in an image.
     * image        The image.
     * keypoints    The input keypoints. Keypoints for which a descriptor cannot be computed are removed.
     * descriptors  Copmputed descriptors. Row i is the descriptor for keypoint i.
     */
    CV_WRAP void compute( const Mat& image, CV_OUT CV_IN_OUT vector<KeyPoint>& keypoints, CV_OUT Mat& descriptors ) const {
        computeImpl(image, keypoints, descriptors);
    }

    void compute( const vector<Mat>& images, vector<vector<KeyPoint> >& keypoints, vector<Mat>& descriptors ) const {

    }

    int descriptorSize() const { return 8; }
    int descriptorType() const { return 0; }

    bool empty(){ return false; }

    static Ptr<DescriptorExtractor> create( const string& descriptorExtractorType ){
        return 0;
    }

protected:
    virtual void computeImpl( const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors )  const {
        descriptors = Mat(keypoints.size(), descriptorSize(), CV_32F, 0.0);
        for(int i = 0; i < keypoints.size(); i++){
//            Point x = keypoints[i].pt - Point(185, 215);
            descriptors.at<float>(i, 0) = keypoints[i].pt.x;
            descriptors.at<float>(i, 1) = keypoints[i].pt.y;
        }
    }

    /*
     * Remove keypoints within borderPixels of an image edge.
     */
    static void removeBorderKeypoints( vector<KeyPoint>& keypoints,
                                      Size imageSize, int borderSize ){

    }
};


class FrameInfo {
public:
    vector<bool> visibilityMask;
    vector<Point2f> points;

    bool isConsistent(FrameInfo* other){
        cout << "Point differences between frames: " << endl;
        for(int i = 0; i < points.size(); i++){
            cout << i << ": " << norm(points[i] - other->points[i]) << endl;
        }
    }
};

FrameInfo* lastFrameInfo = NULL;

class MarkerMatcher {
public:
    ImageManager* imgManager;
    Mat skinImg;

    void getKeypoints(Mat _img, vector<KeyPoint> &keypoints, vector< vector<Point> > &contours, int csign = 1, bool isBinaryImg = true){
        Mat img = _img.clone();

        // Getting the gray version of the image
        Mat gray;
        if(img.channels() == 3) cvtColor(img, gray, CV_BGR2GRAY);
        else gray = img;

        if(csign == 1)gray = 255 - gray;

//        gray = 255 - gray;
//        Mat grayTmp;
//        gray.copyTo(grayTmp, skinImg);
//        if(csign == -1) gray = 255 - grayTmp;
//        else gray = grayTmp;
//        imshow("Gray", gray);
//        waitKey();

        // Pre-filtering
  //    medianBlur(gray, gray, 9);
//        blur(gray, gray, Size(9,9));

        // Adjust blob detector params
        SimpleBlobDetector::Params params;
    //    params.thresholdStep = 10;
        params.minRepeatability = 3;
        if(csign <= 0){
            params.maxThreshold = 255;
        }

        params.filterByArea = true;
        params.minArea = 50;
        params.maxArea = 10000;

        params.filterByCircularity = true;
        params.minCircularity = 0.5;
        params.maxCircularity = 1;

        params.filterByColor = true;
        params.blobColor = csign > 0 ? 0 : 255;
    //    if(csign <= 0) img = Scalar::all(255) - img;

        if(isBinaryImg) params.filterByColor = false;

        params.filterByConvexity = false;
        params.minConvexity = 0;
        params.maxConvexity = 1;

        params.filterByInertia = false;
        params.minInertiaRatio = 0;
        params.maxInertiaRatio = 1;

    //    // Threshold params
    //    cout << "Min Threshold: " << params.minThreshold << endl;
    //    cout << "Max Threshold: " << params.maxThreshold << endl;
    //    cout << "Threshold step: " << params.thresholdStep << endl;
    //    cout << "Min Repeatability: " << params.minRepeatability << endl;

    //    SimpleBlobDetector b(params);
        BetterBlobDetector b(params);

        if(isBinaryImg){
            b.findBlobs(gray, gray, keypoints, contours, csign);
    //        cout << keypoints.size() << endl;
    //        waitKey();
    //        findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        }
        else {
            b.detect(gray, keypoints);
            contours = b.getContours();
        }

    //    Mat contoursImg = Mat(img.rows, img.cols, CV_8UC3, Scalar(255,255,255));
    //    drawContours(contoursImg, contours, -1, Scalar(0,0,255), -1, CV_AA);
    //    imshow("Contours", contoursImg);
    //    waitKey();
    }

    void makeBaseKeypoints(vector<KeyPoint> &baseKeypoints, bool generateCenter = false){
        Size size = Size(512, 512);
        float divs = 5;

        float sw = size.width / divs;
        float sh = size.height / divs;

        float bw = (size.width - 3*sw) * 0.5;
        float bh = (size.height - 3*sh) * 0.5;

        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                baseKeypoints.push_back(KeyPoint(bw + j*sw, bh + i*sh, 1));
            }
        }

        if(!generateCenter){
            baseKeypoints[5] = KeyPoint(999999, 999999, 1);
            baseKeypoints[6] = KeyPoint(999999, 999999, 1);
            baseKeypoints[9] = KeyPoint(999999, 999999, 1);
            baseKeypoints[10] = KeyPoint(999999, 999999, 1);
        }
    }

    /**
     * @brief getMatches
     * Given a image (warped in our case), calculate the keypoints of it and fills
     * the keypoints. Then we get descriptors for each set of keypoints,
     * and do the actual matching
     * @param warpedImg The image to find keypoints
     * @param csign Whether the keypoints looked are black (1) or white (-1)
     * @param markers The central markers (necessary to remove wrongly assigned matches)
     * @param baseKeypoints Keypoints to compare to
     * @param keypoints The keypoints we are going to calculate
     * @param warpedContours The respective contours of each keypoint
     * @param matches The matching
     * @return
     */
    bool getMatches(Mat &warpedImg, int csign,
                      vector<Blob> &markers,
                      vector<KeyPoint> &baseKeypoints,
                      vector<KeyPoint> &keypoints,
                      vector< vector< Point> > &warpedContours,
                      vector<DMatch> &matches){

        // Find keypoints in our image
        getKeypoints(warpedImg, keypoints, warpedContours, csign);
        if(keypoints.size() < 4){
            cout << "There's not enough keypoints to match" << endl;
            return false;
        }

    //    // Remove the L blobs that may be wrongly detected as a circle
    //    for(int i = 0; i < keypoints.size(); i++){
    //        for(int j = 0; j < markers.size(); j++){
    //            Blob &b = markers[j];

    //            // TODO: Warp the centers

    //            if(norm(b.circleCenter - keypoints[i].pt) < b.circleRadius){
    //                keypoints.erase(keypoints.begin() + i);
    //                warpedContours.erase(warpedContours.begin() + i);
    //                i--;
    //                break;
    //            }
    //        }
    //    }

        // Sanity checks
        if(keypoints.size() == 0) return false;
        if(baseKeypoints.size() == 0) return false;

        // Get descriptor from both: BaseKeypoints and Keypoints
        //TODO: Note that the BaseDescriptor can be cached.
        HeekDescriptor descriptor;
        Mat baseDescriptors, targetDescriptors;
        descriptor.compute(Mat(), baseKeypoints, baseDescriptors);
        descriptor.compute(Mat(), keypoints, targetDescriptors);

        // We can choose between Flann e Bruteforce (Or custom)
        BFMatcher matcher(NORM_L2, true);
        //FlannBasedMatcher matcher;

        // Do the actual matching
        matcher.match(baseDescriptors, targetDescriptors, matches);
        if(matches.size() == 0) return false;

        return true;
    }

    void filterMatches(const vector<DMatch> &matches, vector<DMatch> &goodMatches, int thrDistance = INT_MAX){
        double maxDist = 0;
        double minDist = INT_MAX;

        for(int i = 0; i < matches.size(); i++){
            double dist = matches[i].distance;
            minDist = fmin(minDist, dist);
            maxDist = fmax(maxDist, dist);
        }

//        cout << "Max Dist: " << maxDist << endl;
//        cout << "Min Dist: " << minDist << endl;


        for(int i = 0; i < matches.size(); i++){
            //cout << matches[i].distance << endl;
            if(matches[i].distance < thrDistance){
//                cout << matches[i].distance << endl;
                goodMatches.push_back(matches[i]);
            }
        }

    //    cout << "Good Match Size: " << goodMatches.size() << endl;
    }

    /**
     * @brief checkValidity
     * WARNING: This have to be done in warp space!
     * @param blobs
     * @return
     */
    bool checkValidity(const vector<Point2f> &blobs, const vector<bool> &visibility){
        // For each line and column, check if the next blob is "forward"
        for(int i = 0; i < 4; i++){
            float lastX = 0;
            float lastY = 0;

            for(int j = 0; j < 4; j++){
                int ix = 4*i + j;
                int iy = 4*j + i;

                float currentX = lastX;
                float currentY = lastY;

                if(visibility[ix]) currentX = blobs[ix].x;
                if(visibility[iy]) currentY = blobs[iy].y;

                // Some crossing occurred
                if(currentX < lastX || currentY < lastY) return false;

                lastX = currentX;
                lastY = currentY;
            }
        }

        return true;
    }

    bool findCircleKeypoints(ImageManager &imgManager,
                       vector<Blob> &centralMarkers,
                       vector<Point2f> &outputPoints,
                       vector<KeyPoint> &baseKeypoints,
                       vector<bool> &visibilityMask,
                       vector< vector<Point2f> > &circleContours,
                       bool usePerspective = false
                       ){

        // Get the warp matrix and warp the image
        Mat H;
        Mat imgBinaryWarped;
        ImageWarper::warpImage(imgManager.thrImg, imgBinaryWarped, H, centralMarkers,
                               Size(512, 512), 5.0f, usePerspective, Scalar(0,0,0,0));

        // Make a Base Keypoint image for debug purposes
        static Mat baseKptImg;
        if(baseKptImg.empty()){
            baseKptImg = Mat(512, 512, CV_8UC3, Scalar::all(255));
            for(int i = 0; i < baseKeypoints.size(); i++){
                circle(baseKptImg, baseKeypoints[i].pt, 10, Scalar::all(0), CV_FILLED);
            }
        }

        // Find keypoints in our image
        vector<KeyPoint> keypoints;
        vector< vector< Point> > tmpContours;
        vector<DMatch> matches;

        bool success = getMatches(imgBinaryWarped, imgManager.markerColor, centralMarkers, baseKeypoints, keypoints, tmpContours, matches);
        if(!success) return false;

        // Filter Matches based on distance
        int filterMatchesDistance = 80; // Param
        vector<DMatch> goodMatches;
        if(filterMatchesDistance != INT_MAX) filterMatches(matches, goodMatches, filterMatchesDistance);
        else goodMatches = matches;


        // Reset the visibility mask
        fill(visibilityMask.begin(), visibilityMask.end(), false);

        // Get the keypoints base on the found matches
        vector<Point2f> warpSpacePoints(16);
        vector< vector<Point2f> > warpSpaceContours(16);

        // Initialize it with the grid, instead of let it null
        for(int i = 0; i < 16; i++){
            warpSpacePoints[i] = baseKeypoints[i].pt;
        }

        for(int i = 0; i < goodMatches.size(); i++){
            // Get the current match
            DMatch cm = goodMatches[i];

            // Save the respective keypoint and set its visibility to true
            warpSpacePoints[cm.queryIdx] = keypoints[cm.trainIdx].pt;
            visibilityMask[cm.queryIdx] = true;

            // Converting to Point2f (expected by the affineTransform and perspectiveTransform)
            warpSpaceContours[cm.queryIdx] = vector<Point2f>(tmpContours[cm.trainIdx].size());
            for(int j = 0; j < tmpContours[cm.trainIdx].size(); j++){
                warpSpaceContours[cm.queryIdx][j] = tmpContours[cm.trainIdx][j];
            }
        }

        // Convert points (keypoints) back to Image Space
        Mat invH;
        if(usePerspective){
            invH = H.inv();
            perspectiveTransform(warpSpacePoints, outputPoints, invH);
        }
        else {
            invertAffineTransform(H, invH);
            cv::transform(warpSpacePoints, outputPoints, invH);
        }

        // Convert each point of contours back to Image Space
        for(int i = 0; i < warpSpaceContours.size(); i++){
            //cout << i << " Warp Contour Size: " << warpSpaceContours[i].size() << endl;
            if(warpSpaceContours[i].size()){
                if(usePerspective) perspectiveTransform(warpSpaceContours[i], circleContours[i], invH);
                else cv::transform(warpSpaceContours[i], circleContours[i], invH);
            }
        }

        return true;
    }
};

#endif // MARKERMATCHER_H
