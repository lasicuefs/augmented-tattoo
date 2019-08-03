#ifndef CENTRALMARKERDETECTOR_H
#define CENTRALMARKERDETECTOR_H

#include "contourextractor.h"
#include "lgriddetector.h"

class CentralMarkerDetector {
public:
    ContourExtractor contourExtractor;

    float matchThr;
    const int maxQtdLs = 30;

    MatchShapeEngine defaultMSE;

    //vector< ContourExtractor::FilterContourCallback > noFilters;
    //vector< vector<Point> > rawContours;

    vector< vector<Point> > contours;
    vector< ContourExtractor::FilterContourCallback > filters;

    vector< vector<Point> > lselection;

    vector<Blob> blobs;

    CentralMarkerDetector(float matchThr = 0.5f) {
        this->matchThr = matchThr;
        configureFilters();
    }

    void configureFilters() {
        filters.push_back(ContourExtractor::SizeFilterCB);
    }

    bool process(Mat &img, vector<Blob> &markers) {
        // Process first without the size filter
        //contourExtractor.process(imgManager.thrImg, rawContours, noFilters);

        contours.clear();
        contourExtractor.process(img, contours, filters);

        // Now we try to identify the Ls between the contours (using matchShapes)
        lselection.clear();
        LShapeFilter::process(contours, lselection, defaultMSE, matchThr); // Thr bem aberto


        bool passedFilterTooNoisy = lselection.size() <= maxQtdLs;

        // In this case we can just return now
        if(!passedFilterTooNoisy) {
            cout << "Image too noisy, next thresholding configuration will be tested." << endl;
            return false;
        }

        // Now we convert contours to blobs.
        blobs.clear();
        for(int i = 0; i < lselection.size(); i++) {
            blobs.push_back(Blob(lselection[i], i));
        }

        // We want to find groups of 4 Ls that may form our marker
        vector< vector<Blob> > fourLSet;

        // The grid detector will evaluate size, angles and quad constraints
        LGridDetector gridDetector;
        gridDetector.process2(blobs, fourLSet, img.size());

        // If we have valid L-sets, we choose the best
        if(fourLSet.size()) {
            vector< vector<Blob> > ordered;
            sortFourLSet(fourLSet, ordered);
            markers = ordered[0];
            return true;
        }

        // If didn't find a valid L-set return false
        return false;
    }

    class CFitness {
    public:
        int i;
        double fitness;

        CFitness(double fitness, int i){
            this->fitness = fitness;
            this->i = i;
        }

        static int compare(const CFitness &x, const CFitness &y){
            return x.fitness < y.fitness;
        }
    };

    double getOrientationDeviation(const vector<Blob> &fourL) {
        double avg = (fourL[1].orientation + fourL[2].orientation + fourL[3].orientation) / 3.0;
        double accum = ( pow(fourL[1].orientation - avg, 2) +
                        pow(fourL[2].orientation - avg, 2) +
                        pow(fourL[3].orientation - avg, 2) ) / 3.0;
        return sqrt(accum) / avg;
    }

    double getSidesDeviation(const vector<Blob> &fourL){
        Vec2f v1 = fourL[1].massCenter - fourL[0].massCenter;
        Vec2f v2 = fourL[2].massCenter - fourL[1].massCenter;
        Vec2f v3 = fourL[3].massCenter - fourL[2].massCenter;
        Vec2f v4 = fourL[0].massCenter - fourL[3].massCenter;

        double s1 = norm(v1);
        double s2 = norm(v2);
        double s3 = norm(v3);
        double s4 = norm(v4);

        double avg = ( s1 + s2 + s3 + s4 ) / 4.0;
        double accum = ( pow(s1 - avg, 2) + pow(s2 - avg, 2) + pow(s3 - avg, 2) + pow(s4 - avg, 2) ) / 4.0;

        return sqrt(accum) / avg;
    }

    double getFitness(const vector<Blob> &fourL) {
        return getOrientationDeviation(fourL) + getSidesDeviation(fourL);// + getFitness3(fourL)/0.5;
    }

    void sortFourLSet(const vector< vector<Blob> > &fourLSet, vector< vector<Blob> > &ordered) {
        // Fill the fitness for ordering
        vector<CFitness> fitnesses;
        for(int i = 0; i < fourLSet.size(); i++){
            fitnesses.push_back(CFitness(getFitness(fourLSet[i]), i));
        }

        // Sort by fitness
        sort(fitnesses.begin(), fitnesses.end(), CFitness::compare);

        // Save in a ordered vector
        for(int i = 0; i < fourLSet.size(); i++){
            ordered.push_back(fourLSet[fitnesses[i].i]);
        }
    }
};

#endif // CENTRALMARKERDETECTOR_H
