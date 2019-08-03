#ifndef OCCLUSIONHANDLER_H
#define OCCLUSIONHANDLER_H

const int lui[][3] = { {15,10,5}, {13,9,5}, {14,10,6}, {12,9,6}, {7,6,5}, {5,5,5}, {6,6,6}, {4,5,6},
                       {11,10,9}, {9,9,9}, {10,10,10}, {8,9,10}, {3,6,9}, {1,5,9}, {2,6,10}, {0,5,10}};

class OcclusionHandlerSimple {
public:
    static Point2f extrapolate(Point2f a, Point2f b) {
        return b + (b - a);
    }

    static Point2f extrapolate(Point2f a, Point2f b, Point2f c) {
        return c + ((c-b) + (b-a))*0.5f;
    }

    static void handleOcclusions(vector<Point2f> &markers, vector<bool> &visibilityMask) {
        for(int i = 0; i < markers.size(); i++){
            if(!visibilityMask[i]){
                if(visibilityMask[lui[i][0]])
                    markers[i] = extrapolate(markers[lui[i][0]], markers[lui[i][1]], markers[lui[i][2]]);
                else
                    markers[i] = extrapolate(markers[lui[i][0]], markers[lui[i][1]]);
            }
        }
    }

};

#define EXT_A (3.0/2.0)
#define EXT_B (2.0/3.0)

const double rowfactors[] = {EXT_A, EXT_B, EXT_B, EXT_A, EXT_A, 1, 1, EXT_A, EXT_A, 1, 1, EXT_A, EXT_A, EXT_B, EXT_B, EXT_A};
const int lui3r[][3] = { {3,2,1}, {3,2,0}, {0,1,3}, {0,1,2}, {7,6,5}, {5,5,5}, {6,6,6}, {4,5,6},
                       {11,10,9}, {9,9,9}, {10,10,10}, {8,9,10}, {15,14,13}, {15,14,12}, {12,13,15}, {12,13,14}};

const double colfactors[] = {EXT_A, EXT_A, EXT_A, EXT_A, EXT_B, 1, 1, EXT_B, EXT_B, 1, 1, EXT_B, EXT_A, EXT_A, EXT_A, EXT_A};
const int lui3c[][3] = { {12,8,4}, {13,9,5}, {14,10,6}, {15,11,7}, {12,8,0}, {5,5,5}, {6,6,6}, {15,11,3},
                       {0,4,12}, {9,9,9}, {10,10,10}, {3,7,15}, {0,4,8}, {1,5,9}, {2,6,10}, {3,7,11}};


class OcclusionHandler {
public:
    static Point2f extrapolateSpecial(Point2f a, Point2f b, Point2f c, double factor = (3.0/2.0)){
        return a + (((b-a)+(c-b))*factor);
    }

    static int getIndex(int row, int col) {
        return row * 4 + col;
    }

    static bool isBorder(int index) {
        return (index == 0 || index == 3 || index == 12 || index == 15);
    }

    static int checkRow(vector<bool> &visibilityMask, int row, int col) {
        int howManyPointsUsed = 0;
        for(int i = 0; i < 4; i++) if(i != col && visibilityMask[getIndex(row, i)]) howManyPointsUsed++;
        return howManyPointsUsed;
    }

    static int checkCol(vector<bool> &visibilityMask, int row, int col) {
        int howManyPointsUsed = 0;
        for(int i = 0; i < 4; i++) if(i != row && visibilityMask[getIndex(i, col)]) howManyPointsUsed++;
        return howManyPointsUsed;
    }

    static void handleIndividualOclusion(vector<Point2f> &markers, vector<bool> &visibilityMask, int row, int col) {
        int index = getIndex(row, col);
        int crows = checkRow(visibilityMask, row, col);
        int ccols = checkCol(visibilityMask, row, col);

        Point2f finalPoint;
        if(crows == 3 && ccols == 3) {
            // If both are 3, we avg them
            Point2f a = extrapolateSpecial(markers[lui3r[index][0]], markers[lui3r[index][1]], markers[lui3r[index][2]], rowfactors[index]);
            Point2f b = extrapolateSpecial(markers[lui3c[index][0]], markers[lui3c[index][1]], markers[lui3c[index][2]], colfactors[index]);
            finalPoint = (a + b) * 0.5;
        }
        else if(crows == 3) {
            // If at least we have 3 points in the same row we use it
            finalPoint = extrapolateSpecial(markers[lui3r[index][0]], markers[lui3r[index][1]], markers[lui3r[index][2]], rowfactors[index]);
        }
        else if(ccols == 3) {
            // Ok. Now we check if we have 3 points in the same col
            finalPoint = extrapolateSpecial(markers[lui3c[index][0]], markers[lui3c[index][1]], markers[lui3c[index][2]], colfactors[index]);
        }
    //    else if(crows == 2 && ccols == 2){
    //        // If both are 2, we can avg them too
    //    }
        else {
            //finalPoint = OcclusionHandlerSimple::extrapolate(markers[lui[index][0]], markers[lui[index][1]], markers[lui[index][2]]);
            finalPoint = OcclusionHandlerSimple::extrapolate(markers[lui[index][1]], markers[lui[index][2]]);
        }
        markers[index] = finalPoint;
    }

    static void handleOcclusions(vector<Point2f> &markers, vector<bool> &visibilityMask) {
        for(int i = 0; i < markers.size(); i++){
            int row = i / 4;
            int col = i % 4;

            if(!visibilityMask[i]) {
                handleIndividualOclusion(markers, visibilityMask, row, col);
            }
        }
    }

    static void printVisibilityMask(const vector<bool> &mask) {
        for(int i = 0; i < mask.size(); i++){
            cout << (mask[i] ? "O ":"X ");
            if((i+1)%4==0) cout << endl;
        }
    }

};

#endif // OCCLUSIONHANDLER_H
