#ifndef LGRIDDETECTOR_H
#define LGRIDDETECTOR_H

#include "utils.h"
#include "lshapeengine.h"

class CAngle {
public:
    double angle;
    int i;

    CAngle(double angle, int i){
        this->angle = angle;
        this->i = i;
    }

    static int compare(const CAngle &x, const CAngle &y){
        return x.angle < y.angle;
    }
};

class LGridDetector {
public:

    void findWindow(const vector<Blob> &blobs, int index, double thr, int &startIdx, int &endIdx){
        double base = blobs[index].circleRadius * thr;

        startIdx = -1;
        endIdx = -1;

        for(int i = 0; i < blobs.size(); i++){
            if(insideMargin(blobs[i].circleRadius, blobs[index].circleRadius, base)){
                if(startIdx < 0) startIdx = i;
                endIdx = i;
            }
        }
    }

    void process(const vector<Blob> &blobs, vector< vector<int> > &fourLSet, Size debugImgSize){
        // Filtra por tamanhos
        vector<Blob> sizeOrderedBlobs (blobs);
        sort(sizeOrderedBlobs.begin(), sizeOrderedBlobs.end(), Blob::compareByRadius);

        // Debug the sorting
        for(int i = 0; i < sizeOrderedBlobs.size(); i++){
            cout << sizeOrderedBlobs[i].circleRadius << " ";
        }
        cout << endl;


//        for(int i = 0; i < sizeOrderedBlobs.size(); i++){
//            // We find the window for this blob
//            int startIdx, endIdx;
//            findWindow(sizeOrderedBlobs, i, 0.25, startIdx, endIdx);
//            //cout << "Window for blob " << i << ": [" << startIdx << ", " << endIdx << "]" << endl;

//            int numBlobs = endIdx - startIdx + 1;
//            if(numBlobs >= 4){
//                cout << "Window for blob " << i << ": [" << startIdx << ", " << endIdx << "]" << endl;
//            }
//        }

        // Acha 3 com orientação diferente
        // Compara angulo dos 4
    }

    void process2(const vector<Blob> &blobs, vector< vector<Blob> > &fourLSet, Size debugImgSize){
        // For debug purposes
        vector< vector<Point> > contours;
        for(int i = 0; i < blobs.size(); i++) contours.push_back(blobs[i].points);

        const double sizeThr = 2.0;
        const double orientationThr = 90;
        const double orientationBetweenBlobs = 90;
        const double areaThr = 3.0;

        for(int i = 0; i < blobs.size(); i++){
            // Blobs that matches our criteria
            vector<Blob> matches;
//            matches.push_back(blobs[i]);

            double sizeMargin = blobs[i].circleRadius * sizeThr;

            for(int j = 0; j < blobs.size(); j++){
                if(i == j) continue;

                // If the size is acceptable
                if(blobs[j].area > blobs[i].area / areaThr && blobs[j].area < blobs[i].area * areaThr)
//                if(insideMargin(blobs[j].circleRadius, blobs[i].circleRadius, sizeMargin))
                {
//                    double orientation = fabs(blobs[j].orientation - blobs[i].orientation);
//                    if(insideMargin(orientation, 180, orientationThr))
                    {
//                        Vec2f biTobj = blobs[j].massCenter - blobs[i].massCenter;

//                        double minAngle = getMinAngle(biTobj, blobs[i].direction) * RAD2DEG;
//                        //cout << "MinAngle: " << minAngle << endl;

//                        if(minAngle < orientationBetweenBlobs){
                            matches.push_back(blobs[j]);
//                        }

                    }
                }
            }

            Mat debugImg = Mat::zeros(debugImgSize, CV_8UC3);
            find4L(blobs[i], matches, fourLSet, debugImg);

            bool debug = false;
//            debug = true;
            if(debug){
                // Drawing Debug

                drawContours(debugImg, contours, i, Scalar(255, 0, 0), -1);
                circle(debugImg, blobs[i].massCenter, 1, Scalar(0,255,0));

                for(int m = 0; m < matches.size(); m++){
                    drawContours(debugImg, contours, matches[m].index, Scalar(0, 0, 255), 2);
                    circle(debugImg, blobs[matches[m].index].massCenter, 1, Scalar(0,255,0));
                }

                namedWindow("DebugImg", WINDOW_NORMAL);
                imshow("DebugImg", debugImg);
                waitKey();
            }

        }

    }

    void find4L(const Blob &base, const vector<Blob> &blobs, vector< vector<Blob> > &fourLSet, Mat debugImg = Mat()){
        // We need at least 3 other blobs to form a quad
        if(blobs.size() < 3) return;

        // Calculate the distance to all other blobs
        CDist dists[blobs.size()];
        for(int i = 0; i < blobs.size(); i++){
            double distance = norm(base.massCenter - blobs[i].massCenter);
            dists[i] = CDist(distance, 0, i);
        }

        // Sort by increasing order
        qsort(dists, blobs.size(), sizeof(CDist), CDist::compare);

        // TODO: Make the combinations

//        // For now we just take the 3 closer blobs
//        Vec2f a = base.massCenter;
//        Vec2f b = blobs[ dists[0].to ].massCenter;
//        Vec2f c = blobs[ dists[1].to ].massCenter;
//        Vec2f d = blobs[ dists[2].to ].massCenter;

        vector<Blob> mySet;
        mySet.push_back(base);
        mySet.push_back(blobs[dists[0].to]);
        mySet.push_back(blobs[dists[1].to]);
        mySet.push_back(blobs[dists[2].to]);

//        circle(debugImg, ((Blob)mySet[1]).massCenter, 10, Scalar(128, 128, 0));
//        circle(debugImg, ((Blob)mySet[2]).massCenter, 10, Scalar(0, 128, 0));
//        circle(debugImg, ((Blob)mySet[3]).massCenter, 10, Scalar(0, 128, 128));

        vector<Blob> acceptedSet;
//        bool _isQuad = isQuad(mySet, acceptedSet, 30, 0.7);
        bool _isQuad = isQuad(mySet, acceptedSet, 35, 0.8);
//        bool _isQuad = isQuad(mySet, acceptedSet, 60, 1.5);
//        bool _isQuad = true;

        // And we verify if it will form a quad
//        bool _isQuad = isQuad3(a, b, c, d, 40);


//        cout << "The blobs form a quad? " << (_isQuad ? "true" : "false") << endl;

        if(_isQuad) fourLSet.push_back(acceptedSet);
    }

    bool sortOri(const vector<Blob> &blobs, vector<Blob> &ordered){
//        cout << "#---- SORT ORIENTATION ----#" << endl;
        vector<CAngle> angles;
        vector<Vec2f> directions;
//        cout << "Angles: ";
        for(int i = 0; i < blobs.size(); i++){
            Vec2f direction;
            bool noDefect = LShapeEngine::checkLDefect(blobs[i].points, direction, 45);
            if(!noDefect){
//                cout << "NO DEFECT " << endl;
                for(int i = 0; i < blobs.size(); i++) ordered.push_back(blobs[i]);
                return false;
            }
//            double angle = getAngle(Vec2f(0, -1), direction) * RAD2DEG;
//            angles.push_back(CAngle(angle, i));
            directions.push_back(direction);
        }

        angles.push_back(CAngle(0, 0));
        for(int i = 1; i < blobs.size(); i++) {
            double angle = getMinAngle(directions[0], directions[i]) * RAD2DEG;
            angles.push_back(CAngle(angle, i));
        }
//        for(int i = 0; i < blobs.size(); i++) cout << angles[i].angle << " ";

        sort(angles.begin(), angles.end(), CAngle::compare);
//        for(int i = 0; i < blobs.size(); i++) angles[i].angle -= angles[0].angle;

//        cout << endl;
//        for(int i = 0; i < blobs.size(); i++) cout << angles[i].angle << " ";
//        cout << endl;

        double thr = 90;
        if(angles[2].angle < thr && angles[3].angle > thr){
            ordered.push_back(blobs[angles[3].i]);
            ordered.push_back(blobs[angles[0].i]);
            ordered.push_back(blobs[angles[1].i]);
            ordered.push_back(blobs[angles[2].i]);
        }
        else if(angles[1].angle > thr){
            ordered.push_back(blobs[angles[0].i]);
            ordered.push_back(blobs[angles[1].i]);
            ordered.push_back(blobs[angles[2].i]);
            ordered.push_back(blobs[angles[3].i]);
        }
        else for(int i = 0; i < blobs.size(); i++) ordered.push_back(blobs[i]);
        return true;
    }

    bool isQuad( vector<Blob> &blobs, vector<Blob> &ordered, double thrAngle, double thrSize = 0.3){
        vector<Blob> byAngle;
        sortOri(blobs, byAngle);
        blobs = byAngle;

        ordered.push_back(blobs[0]);

        // We gonna work with the mass centers
        Vec2f a = blobs[0].massCenter;
        Vec2f b = blobs[1].massCenter;
        Vec2f c = blobs[2].massCenter;
        Vec2f d = blobs[3].massCenter;

        // Get the baricenter
        Vec2f baricenter = (a + b + c + d) * 0.25;

        // And the vector to all blobs
        Vec2f c1 = a - baricenter;
        Vec2f c2 = b - baricenter;
        Vec2f c3 = c - baricenter;
        Vec2f c4 = d - baricenter;

        double teta1 = getAngle(c1, c2) * RAD2DEG;
        double teta2 = getAngle(c1, c3) * RAD2DEG;
        double teta3 = getAngle(c1, c4) * RAD2DEG;

        double angles[3] = { teta1, teta2, teta3};
        sort(angles, angles + 3);

        for(int i = 0; i < 3; i++){
            if(angles[i] == teta1) ordered.push_back(blobs[1]);
            else if(angles[i] == teta2) ordered.push_back(blobs[2]);
            else ordered.push_back(blobs[3]);
        }

        //return true;

        return isQuad2((Vec2f) ordered[0].massCenter,
                       (Vec2f) ordered[1].massCenter,
                       (Vec2f) ordered[2].massCenter,
                       (Vec2f) ordered[3].massCenter, thrAngle, thrSize);

    }

//    bool isQuad3(Point2f a, Point2f b, Point2f c, Point2f d, double thr){
//        Point2f _b = (a + b + c + d) * 0.25;
//        Vec2f baricenter (_b.x, _b.y);

//        Vec2f c1 = Vec2f(a.x, a.y) - baricenter;
//        Vec2f c2 = Vec2f(b.x, b.y) - baricenter;
//        Vec2f c3 = Vec2f(c.x, c.y) - baricenter;
//        Vec2f c4 = Vec2f(d.x, d.y) - baricenter;

//        vector<double> angles;

//        angles.push_back(getMinAngle(c1, c2) * RAD2DEG);
//        angles.push_back(getMinAngle(c1, c3) * RAD2DEG);
//        angles.push_back(getMinAngle(c1, c4) * RAD2DEG);


//        if(insideMargin(angles[0], 180, thr)) return isQuad2(a, c, b, d, thr);
//        else return isQuad2(a, b, c, d, thr);
//    }


    bool isQuad2(Vec2f a, Vec2f b, Vec2f c, Vec2f d, double thr, double thrSize = 0.3) {
        Vec2f down = b - a;
        Vec2f right = c - b;
        Vec2f up = d - c;
        Vec2f left = a - d;

        double a1 = getMinAngle(left, down) * RAD2DEG;
        double a2 = getMinAngle(down, right) * RAD2DEG;
        double a3 = getMinAngle(right, up) * RAD2DEG;
        double a4 = getMinAngle(up, left) * RAD2DEG;

        //cout << a1 << " " << a2 << " " << a3 << " " << a4 << endl;


        // Some size contraints should be respected
        double s1 = norm(left) / norm(right);
        double s2 = norm(down) / norm(up);
        double s3 = norm(left) / norm(up);

//        cout << "We gonna test size. You should get an answer if it pass." << endl;

        if(s1 < 1.0 - thrSize || s1 > 1.0 + thrSize) return false;
        if(s2 < 1.0 - thrSize || s2 > 1.0 + thrSize) return false;
        if(s3 < 1.0 - thrSize || s3 > 1.0 + thrSize) return false;

//        cout << "Passed size test. We will test angles now." << endl;

//        cout << "A1: " << a1 << endl;
//        cout << "A2: " << a2 << endl;
//        cout << "A3: " << a3 << endl;
//        cout << "A4: " << a4 << endl;

        return (insideMargin(a1, 90, thr) && insideMargin(a2, 90, thr) &&
                insideMargin(a3, 90, thr) && insideMargin(a4, 90, thr));
    }
};

#endif // LGRIDDETECTOR_H
