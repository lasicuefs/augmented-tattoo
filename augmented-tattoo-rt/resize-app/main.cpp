#include <stdio.h>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <QDir>

using namespace std;
using namespace cv;

void configureOutput(VideoCapture &cap, VideoWriter &writer,
                     const string &filename, int targetHeight){
    // Get and print info from video
    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    int fps = cap.get(CV_CAP_PROP_FPS);

    cout << "Video Info: " << endl;
    cout << "Width: " << width << endl;
    cout << "Height: " << height << endl;
    cout << "FPS: " << fps << endl;
    cout << endl;

    float factor = (targetHeight / double(height));
    Size finalSize(width * factor, height * factor);

    // Open the video writer stream
    writer = VideoWriter(filename, CV_FOURCC('M', 'J', 'P', 'G'), fps, finalSize);
}

/** @function main */
int main( int argc, char** argv ) {
    cout << "Usage: \n  resize-app.exe input.avi output.avi 480" << endl << flush;
    fflush(stdout);

    if(argc < 4) return 0;

    string input = argv[1];
    string output = argv[2];
    int targetHeight = atoi(argv[3]);

    cout << "Input: " << input << endl;
    cout << "Output: " << output << endl;
    cout << "Target Height: " << targetHeight << endl;

    replace(output.begin(), output.end(), '\\', '/');
    QDir().mkpath(output.substr(0, output.find_last_of('/')).c_str());

    VideoCapture cap(input);

    VideoWriter writer;
    configureOutput(cap, writer, output, targetHeight);

    Mat img, resized;

    while(cap.read(img)){

        float factor = float(targetHeight) / float(img.rows);
        resize(img, resized, Size(), factor, factor/*, INTER_NEAREST*/);

        writer << resized;
        //imshow("Resized", resized);
        //if(waitKey(30) == 27) break;
    }

    cap.release();
    writer.release();

    return 0;
}

