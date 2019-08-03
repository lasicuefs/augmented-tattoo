#ifndef THRESHOLDING_H
#define THRESHOLDING_H

/// Parameters to be used by the threshold algorithm
class ThresholdParams {
public:
    int blur;
    int thr1;
    int c;
    int morph;

    ThresholdParams(int b, int t1, int c, int morph) {
        this->blur = b;
        this->thr1 = t1;
        this->c = c;
        this->morph = morph;
    }
};

class AdaptiveThresholdAlgorithm {
public:
    Mat process(Mat img, Mat &output, const ThresholdParams &p, int csign) {
        return process(img, output, p.blur, p.thr1, p.c, csign, p.morph);
    }

    Mat process(Mat img, Mat &output, int b, int t1, int c, int csign, int morph = 0) {
        Mat blurredImg;
        blur(img, blurredImg, Size(b, b));

        adaptiveThreshold(blurredImg, output, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, t1 , csign * c);

        if(morph) {
            dilate(output, output, Mat(), Point(-1,1), morph);
            erode(output, output, Mat(), Point(-1,1), morph);
        }

        return output;
    }
};

class ThresholdTechnique {
public:
    virtual ~ThresholdTechnique() {}
    virtual int numPasses() {
        return 1;
    }

    virtual void process(const Mat& img, Mat &output, int pass = 0) = 0;
};

class ATAdaptiveTechnique : public ThresholdTechnique {
public:
    AdaptiveThresholdAlgorithm adaptiveThr;
    vector<ThresholdParams> thrParams;
    ImageManager* imgManager;


    ATAdaptiveTechnique(ImageManager* imgManager) {
        this->imgManager = imgManager;
        configureParams();
    }

    void configureParams() {
        thrParams.push_back(ThresholdParams(5, 33, 20, 1));
        thrParams.push_back(ThresholdParams(5, 33, 10, 1));
        thrParams.push_back(ThresholdParams(3, 33, 30, 0));
        thrParams.push_back(ThresholdParams(3, 43, 20, 1));
        thrParams.push_back(ThresholdParams(5, 23, 25, 0));
        thrParams.push_back(ThresholdParams(7, 13, 10, 0));
    }

    virtual int numPasses() {
        return 6;
    }

    virtual void process(const Mat& img, Mat &output, int pass = 0) {
        adaptiveThr.process(img, output, thrParams[pass], imgManager->markerColor);
    }

};

#endif // THRESHOLDING_H
