#ifndef CIRCULARMARKERDETECTOR_H
#define CIRCULARMARKERDETECTOR_H

#include "occlusionhandler.h"
#include "markermatcher.h"

class CircularMarkerDetector {
public:

    bool findCircularMarkers(ImageManager &imgManager,
                             vector<Blob> &markers,
                             vector<Blob> &outBlobs,
                             vector<Point2f> &outMarkers,
                             vector<bool> &visibilityMask
                             ) {
        vector< vector<Point2f> > outContours(16);

        MarkerMatcher matcher;
        matcher.imgManager = &imgManager;

        // Generate the base keypoints to check against
        static vector<KeyPoint> baseKeypoints;
        if(baseKeypoints.empty()) matcher.makeBaseKeypoints(baseKeypoints, true);


        // Let's test the keypoints
        bool valid = matcher.findCircleKeypoints(imgManager, markers, outMarkers, baseKeypoints, visibilityMask, outContours, false);
        if(!valid) return false;

        // Insert the central markers to the set
        outMarkers[5] = markers[3].massCenter;
        visibilityMask[5] = true;

        outMarkers[6] = markers[0].massCenter;
        visibilityMask[6] = true;

        outMarkers[9] = markers[2].massCenter;
        visibilityMask[9] = true;

        outMarkers[10] = markers[1].massCenter;
        visibilityMask[10] = true;

        outBlobs[5] = markers[3];
        outBlobs[6] = markers[0];
        outBlobs[9] = markers[2];
        outBlobs[10] = markers[1];

        // Print the visibility mask (debug)
        //OcclusionHandler::printVisibilityMask(visibilityMask);

        // Discard criterion
        if(std::count(visibilityMask.begin(), visibilityMask.end(), false) > 4){
            cout << "Too many nodes occluded" << endl;
            return false;
        }

        // Let's Handle occlusion
        OcclusionHandler::handleOcclusions(outMarkers, visibilityMask);

        for(int i = 0; i < outMarkers.size(); i++){
            if(i == 5 || i == 6 || i == 9 || i == 10) continue;
            if(visibilityMask[i]){
                vector<Point> blobPoints(outContours[i].size());
                std::copy(outContours[i].begin(), outContours[i].end(), blobPoints.begin());

                outBlobs[i].points = blobPoints;
                outBlobs[i].massCenter = outMarkers[i];
    //            outBlobs[i].circleCenter = outMarkers[i];
                minEnclosingCircle(outBlobs[i].points, outBlobs[i].circleCenter, outBlobs[i].circleRadius);


    //            outBlobs[i] = Blob(blobPoints, i);
    ////            outBlobs[i].massCenter = outMarkers[i];
            }
            else {
                outBlobs[i].massCenter = outMarkers[i];
                outBlobs[i].circleCenter = outMarkers[i];
                //minEnclosingCircle(outBlobs[i].points, outBlobs[i].circleCenter, outBlobs[i].circleRadius);
            }
        }

        return true;
    }
};

#endif // CIRCULARMARKERDETECTOR_H
