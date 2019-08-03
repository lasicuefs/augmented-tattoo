#ifndef LSHAPEENGINE_H
#define LSHAPEENGINE_H

class LShapeEngine {
public:
    LShapeEngine();

    static bool checkLDefect(const vector<Point> &contour, Vec2f &direction, double angleThr = 20.0){
        // Little epsilon. Just to remove imperfections
        vector<Point> approx;
        double epsilon = 0.001;//* arcLength(contour, true);
        approxPolyDP(contour, approx, epsilon, true);

        vector<int> hull;
        vector<Vec4i> defects;

    //    vector< vector<Point> > temp;
    //    temp.push_back(approx);
    //    Mat img = Mat::zeros(800, 600, CV_8UC3);
    //    drawContours(img, temp, 0, Scalar(233,233, 0));

        // Minimum to check convexHull
    //    if(approx.size() < 4) return false;
        convexHull(approx, hull);
        convexityDefects(approx, hull, defects);

    //    vector< vector<Point> > tempH;
    //    vector<Point> hull2;
    //    convexHull(approx, hull2);
    //    tempH.push_back(hull2);
    //    drawContours( img, tempH, 0, Scalar(2, 233, 233) );

        // L-Shape must have at least a defect
        //cout << "Defects size: " << defects.size() << endl;
        if(defects.size() == 0) return false;

        // Let's examine the largest
        int maxDepthIndex = 0;
        float maxDepth = 0;
        getMaxDefect(defects, maxDepthIndex, maxDepth);

    //    bool anotherDefect = hasAnotherBigDefect(defects, maxDepthIndex, maxDepth, 0.5);
    ////    cout << (anotherDefect ? "Tem outro defeito" : "Nem tem") << endl;
    //    if(anotherDefect) return false;

        // Convert to points
        Vec4i& v = defects[maxDepthIndex];
        int startidx = v[0]; Point ptStart( approx[startidx] );
        int endidx   = v[1]; Point ptEnd( approx[endidx] );
        int faridx   = v[2]; Point ptFar( approx[faridx] );

        // Get the vectors that form the defect
        Vec2f up = Vec2f(ptStart.x, ptStart.y) - Vec2f(ptFar.x, ptFar.y);
        //Vec2f diag = Vec2f(s.x, s.y) - Vec2f(ptFar.x, ptFar.y);
        Vec2f left = Vec2f(ptEnd.x, ptEnd.y) - Vec2f(ptFar.x, ptFar.y);

        // Normalize vectors
        left = normalize(left);
        up = normalize(up);

        // Get the direction of the defect
        Point pdir = (ptStart - ptFar) + (ptEnd - ptFar);
        direction = normalize(Vec2f(pdir.x, pdir.y));

        return true;
    }
};

#endif // LSHAPEENGINE_H
