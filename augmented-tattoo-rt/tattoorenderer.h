#ifndef TATTOORENDERER_H
#define TATTOORENDERER_H

#include <vector>
#include <string>

#include "opencv2/core/core.hpp"
#include <opencv2/core/opengl_interop.hpp>

using namespace std;
using namespace cv;

class TattooRenderer {
public:
    struct DrawData {
        ogl::Arrays arr;
        ogl::Texture2D tex;
        ogl::Buffer indices;
    };

    TattooRenderer(string winName, string tattooPath, int width, int height, Size patternSize);
    ~TattooRenderer();
    void openWindow();

    vector<int> createIndicesForGrid(float cols, float rows);
    vector<Vec2f> createTexCoordForGrid(float cols, float rows);
    void createFullscreenQuad(/*Mat img, */int w, int h, DrawData &data);
    void createTattoo(Mat img, DrawData &data, Size patternSize);

    Mat backgroundImg;
    Mat skinImg;
    Mat showMe;

    bool glewFlag = false;

    void setSkinMask(Mat mask);
    void update(Mat img, const vector<Point2f> &centers, bool drawTattoo = true);
    void draw();

    static void draw(void* data);

    Mat matFromOpenGL();

    void initializeOpenGL();

    Mat output;

protected:
    Mat tattoo;
    int width, height;
    Size patternSize;

    float alphaScale;

    string winName;

    DrawData backgroundData;
    DrawData tattooData;
    DrawData skinMaskData;
};

#endif // TATTOORENDERER_H
