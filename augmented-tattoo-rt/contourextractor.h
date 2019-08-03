#ifndef CONTOUREXTRACTOR_H
#define CONTOUREXTRACTOR_H

class ContourExtractor {
public:

    float minArea = 50.0f;
    float maxArea = 2000.0f;

    /// Returns true if passed the filter
    typedef bool (*FilterContourCallback)(vector<Point> &contour, void* userdata);

    /// Filter contours by size
    static bool SizeFilterCB(vector<Point> &contour, void* userdata = NULL){
        ContourExtractor* c = (ContourExtractor*) userdata;
        double area = contourArea(contour);
        if(area > c->minArea && area < c->maxArea) return true;
        return false;
    }

    /// Extract contours running the filters
    void process(Mat img, vector< vector<Point> >& candidates,
                 vector< FilterContourCallback > &filters){
        // Get the contours
        vector <Vec4i> hierarchy;
        vector < vector<Point> > contours;
        findContours(img.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

        // For each blob we run each filter. If the blob pass all tests
        // we include it in the selection list.
        for(int i = 0; i < contours.size(); i++){
            // Little injection. Only consider leaf contours.
            if(hierarchy[i][2] != -1) continue;

            bool filtered = false;
            for(int j = 0; j < filters.size(); j++){
                if(!filters[j](contours[i], this)){
                    filtered = true;
                    break;
                }
            }
            if(!filtered) candidates.push_back(contours[i]);
        }
    }

};

#endif // CONTOUREXTRACTOR_H
