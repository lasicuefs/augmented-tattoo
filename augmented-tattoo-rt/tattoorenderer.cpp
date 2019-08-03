#include "tattoorenderer.h"

#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "shader.h"
#include "timestats.h"

using namespace std;
using namespace cv;

TattooRenderer::TattooRenderer(string winName, string tattooPath, int width, int height, Size patternSize) {
    this->winName = winName;
    this->width = width;
    this->height = height;
    this->patternSize = patternSize;
    this->alphaScale = 0.6f;

    int blurSize = 3;

    tattoo = imread(tattooPath, CV_LOAD_IMAGE_UNCHANGED);
    if(tattoo.empty()){
        cout << "Ocorreu um erro ao carregar a imagem da tatuagem" << endl;
    }
    blur(tattoo, tattoo, Size(blurSize, blurSize));
}

TattooRenderer::~TattooRenderer() {
    cout << "Destroying Tattoo Renderer..." << endl;
    setOpenGlDrawCallback("OpenGL", 0, 0);
    destroyWindow("OpenGL");
}

vector<int> TattooRenderer::createIndicesForGrid(float cols, float rows) {
    vector<int> indices;
    for(int i = 0; i < rows-1; i++) {
        for(int j = 0; j < cols-1; j++) {
            int a = (i*cols) + j;
            int b = (i+1)*cols + j;

            indices.push_back( b );
            indices.push_back( a );
            indices.push_back( a + 1 );

            indices.push_back( a + 1 );
            indices.push_back( b + 1 );
            indices.push_back( b );
        }
    }

    return indices;
}

vector<Vec2f> TattooRenderer::createTexCoordForGrid(float cols, float rows) {
    vector<Vec2f> texCoords;

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            texCoords.push_back( Vec2f(float(j)/float(cols-1), float(i)/float(rows-1)) );
            //cout << texCoords.back() << endl;
        }
    }
    return texCoords;
}

void TattooRenderer::createTattoo(Mat img, DrawData &data, Size patternSize) {
    vector<Vec2f> texCoords = TattooRenderer::createTexCoordForGrid(patternSize.width, patternSize.height);
    vector<int> indices = TattooRenderer::createIndicesForGrid(patternSize.width, patternSize.height);

    vector<Vec4f> colors;
    for(int i = 0; i  < patternSize.width * patternSize.height; i++) {
        colors.push_back(Vec4f(1, 1, 1, alphaScale));
    }

//    data.arr.setVertexArray(vertex);
    data.arr.setTexCoordArray(texCoords);
    data.arr.setColorArray(colors);

    data.indices.copyFrom(indices);

    if(img.channels() == 4)
        data.tex = ogl::Texture2D(img.rows, img.cols, ogl::Texture2D::RGBA);
    else
        data.tex = ogl::Texture2D(img.rows, img.cols, ogl::Texture2D::RGB);

    data.tex.copyFrom(img);
}

void TattooRenderer::createFullscreenQuad(/*Mat img, */int w, int h, DrawData &data) {
    Mat_<Vec2f> vertex(1, 4);
    vertex << Vec2f(0, h), Vec2f(0, 0), Vec2f(w, 0), Vec2f(w, h);

    Mat_<Vec2f> texCoords(1, 4);
    texCoords << Vec2f(0, 1), Vec2f(0, 0), Vec2f(1, 0), Vec2f(1, 1);

    Mat_<int> indices(1, 6);
    indices << 0, 1, 2, 2, 3, 0;

    data.arr.setVertexArray(vertex);
    data.arr.setTexCoordArray(texCoords);
    data.indices.copyFrom(indices);
//    data.tex.copyFrom(img);
}

GLuint regProg;
GLuint cameraImageTextureID;

void TattooRenderer::initializeOpenGL() {
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, height, 0.0f, 0.0f, 1.0f);

    glDisable(GL_CULL_FACE);

    //static bool flag = false;
    if(!glewFlag) {
        GLenum err = glewInit();
        if(err != GLEW_OK)
            exit(0);
        regProg = Shader::setShaders((char *) "../shaders/reg.vert", (char *) "../shaders/reg.frag");
        glewFlag = true;
    }
//    glGenTextures(3, &cameraImageTextureID);
//    glActiveTexture(GL_TEXTURE0 + 0);
}

void TattooRenderer::draw(void* data) {
    TattooRenderer* me = static_cast<TattooRenderer*>(data);
    me->draw();
}

void TattooRenderer::draw() {
    glClearColor(1, 1, 1, 1);

    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    if(showMe.empty())
        return;

    backgroundData.tex = ogl::Texture2D(showMe);
    backgroundData.tex.bind();
    ogl::render(backgroundData.arr, backgroundData.indices, ogl::TRIANGLES);

    glActiveTexture( GL_TEXTURE0 );
    tattooData.tex.bind();

    glActiveTexture( GL_TEXTURE1 );
    skinMaskData.tex = ogl::Texture2D(skinImg);
    skinMaskData.tex.bind();

    glUseProgram(regProg);
    GLint texLoc = glGetUniformLocation(regProg, "backgroundTex");
    glUniform1i(texLoc, 0);

    GLint maskParam = glGetUniformLocation(regProg, "maskTex");
    glUniform1i(maskParam, 1);

    glActiveTexture( GL_TEXTURE0 );
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ogl::render(tattooData.arr, tattooData.indices, ogl::TRIANGLES);
    glDisable(GL_BLEND);

    backgroundData.tex.release();
    skinMaskData.tex.release();
}

Mat TattooRenderer::matFromOpenGL() {
    output.create(height, width, CV_8UC3);
    //use fast 4-byte alignment (default anyway) if possible
    glPixelStorei(GL_PACK_ALIGNMENT, (output.step & 3) ? 1 : 4);
    //set length of one complete row in destination data (doesn't need to equal output.cols)
    glPixelStorei(GL_PACK_ROW_LENGTH, output.step/output.elemSize());
    glReadPixels(0, 0, output.cols, output.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, output.data);
    flip(output, output, 0);

    return output;
}

void TattooRenderer::setSkinMask(Mat skin) {
    skinImg = skin;
}

void TattooRenderer::update(Mat img, const vector<Point2f> &centers, bool drawTattoo) {
    showMe = img;

    if(img.cols == 0 || img.rows == 0) return;

    // Adjust the window to the size of background image
    if(img.cols != width || img.rows !=  height) {
        cout << "Adjusting Window... " << endl;
        width = img.cols;
        height = img.rows;

        if(skinImg.empty())
            skinImg = Mat::zeros(width, height, CV_8U);

        resizeWindow(winName, width, height);
        initializeOpenGL();
        createFullscreenQuad(width, height, backgroundData);
    }

    if(!drawTattoo) {
        vector<Vec2f> vertex;
        for(int i = 0; i < 16; i++)
            vertex.push_back(Point2f(0,0));
        tattooData.arr.setVertexArray(vertex);
    }
    else if(centers.size() > 0) {
        // We expect in inverted order and as a Vec2f
        static vector<Vec2f> vertex (16);
        for(int i = 0; i < centers.size(); i++) {
            vertex[i] = (centers[(centers.size()-1)-i]);
            //vertex.push_back(centers[(centers.size()-1)-i]);
//            vertex.push_back(centers[i]);
        }
        tattooData.arr.setVertexArray(vertex);
    }

    updateWindow(winName);
}

void TattooRenderer::openWindow() {
    namedWindow(winName, CV_WINDOW_OPENGL);
    resizeWindow(winName, width, height);

    // Falta texturizar (no update)
    createFullscreenQuad(width, height, backgroundData);
    createTattoo(tattoo, tattooData, patternSize);

    initializeOpenGL();

    setOpenGlDrawCallback(winName, TattooRenderer::draw, (void*) this);
}
