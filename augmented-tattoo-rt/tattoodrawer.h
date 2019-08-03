#ifndef TATTOODRAWER_H
#define TATTOODRAWER_H

#include "opencv2/photo/photo.hpp"

#include "imagemanager.h"
#include "tattoorenderer.h"
#include "timestats.h"

class TattooDrawer {
public:

    TattooDrawer() {

    }

    Mat skin;
    Mat getSkinImage(ImageManager &imgManager){
        cvtColor(imgManager.img, skin, CV_BGR2HSV);

        inRange(skin,  Scalar(0, 30, 0), Scalar(20, 255, 255), imgManager.skinImg);
        Mat otherSide; inRange(skin, Scalar(170, 30, 0), Scalar(180, 255, 255), otherSide);
        imgManager.skinImg = otherSide | imgManager.skinImg;

        //imshow("Skin", imgManager.skinImg);
        return imgManager.skinImg;
    }

    void generateInpaintMask(ImageManager &imgManager,
                            const vector<Blob> &outBlobs) {
        // Inpaint circles
        imgManager.inpaintMask = Mat(imgManager.img.size(), CV_8U, 0.0);
        vector< vector<Point> > tmpContours;
        for(int i = 0; i < outBlobs.size(); i++){
            if(i == 5 || i == 6 || i == 9 || i == 10)  tmpContours.push_back(outBlobs[i].hull);
            else tmpContours.push_back(outBlobs[i].points);
        }
        drawContours(imgManager.inpaintMask, tmpContours, -1, Scalar::all(255), CV_FILLED);

        // Do some preprocessing
        dilate(imgManager.inpaintMask, imgManager.inpaintMask, Mat(), Point(-1,-1), 4);
    }

    void draw(ImageManager &imgManager,
             const vector<Blob> &outBlobs,
             const vector<Point2f> &outMarkers) {
        getSkinImage(imgManager);

        // By now the blobs might be black spots.
        // We will get the inpaint mask (and add to the skin mask)
        generateInpaintMask(imgManager, outBlobs);
        //imshow("Inpaint Mask", imgManager.inpaintMask);

        // We should put the blobs in the skin mask
        imgManager.skinImg += imgManager.inpaintMask;
        //imshow("Skin Image + Inpaint Mask", imgManager.skinImg);

        // Do the actual inpaint
        inpaint(imgManager.img, imgManager.inpaintMask, imgManager.inpaintedImg, 1, INPAINT_TELEA);
        //imshow("Inpainted Image", imgManager.inpaintedImg);

//        // This is a white-background black tattoo image
//        Mat rgbMask;
//        cvtColor(imgManager.skinImg, rgbMask, COLOR_GRAY2BGR);

//        renderer->setSkinMask(rgbMask);
//        renderer->update(imgManager.inpaintedImg, outMarkers);

//        imgManager.finalImg = renderer->output;
//        return imgManager.finalImg;
    }

};

#endif // TATTOODRAWER_H
