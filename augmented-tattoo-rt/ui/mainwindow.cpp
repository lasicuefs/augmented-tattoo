#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "blob.h"
#include "matchshapeengine.h"

#include "imagemanager.h"

#include "thresholding.h"
#include "centralmarkerdetector.h"
#include "circularmarkerdetector.h"
#include "tattoodrawer.h"
#include "timestats.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"

#ifdef _WIN32
#include "windows.h"
#endif

#include "ui/ui_about.h"

using namespace std;
using namespace cv;

class AugmentedTattoo {
public:
    VideoCapture cap;
    ImageManager imgManager;
    ThresholdTechnique* thresholdTech;
    CentralMarkerDetector centralMarkerDetector;
    CircularMarkerDetector circularMarkerDetector;
    TattooDrawer* drawer;
    TattooRenderer* renderer;

    vector<Blob> centralMarkers = vector<Blob>(4);
    vector<Blob> blobs = vector<Blob>(16);
    vector<Point2f> centers = vector<Point2f>(16);
    vector<bool> visibilityMask = vector<bool>(16);

    int currentFrame = 0;
    bool exitFlag = false;
    int waitTime = 10;

    int targetSize = 720;
    bool redim = false;

    Mat tattoo;

    AugmentedTattoo(string &tattooPath/* = "data/tattoos/sibgrapi.png"*/) {
        //configureStream();
        thresholdTech = new ATAdaptiveTechnique(&imgManager);
        drawer = new TattooDrawer();

        //renderer = new TattooRenderer("OpenGL", tattooPath, 512, 512, Size(4,4));
        //renderer->openWindow();

        // rotate image in 180 degrees
        tattoo = imread(tattooPath, CV_LOAD_IMAGE_UNCHANGED);
        Point2f src_center(tattoo.cols/2.0F, tattoo.rows/2.0F);
        Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.F);
        warpAffine(tattoo, tattoo, rot_mat, Size());

        int blurSize = 3;
        blur(tattoo, tattoo, Size(blurSize, blurSize));
    }

    virtual ~AugmentedTattoo() {
        delete thresholdTech;
        delete drawer;
        delete renderer;
    }

    void configure(int targetHeight, MarkerColor markerColor) {
        imgManager.targetHeight = targetHeight;
        imgManager.markerColor = markerColor;
    }

    void configureWebcam(int id) {
        cap = VideoCapture(id);
        waitTime = 30;
    }

    void configureStream(const string &path) {
        cap = VideoCapture(path);
        waitTime = 1;
    }

    void showImg(const string &path) {
        imgManager.originalImg = imread(path);
        process();
    }

    double buffer[1000];

    void run() {
        // Check if the stream is open first
        if(!cap.isOpened()) {
            cout << "VideoCapture not open" << endl;
            return;
        }

        // Read to img and process
        char escape = -1;
        PerfTimer processTimer;
        processTimer.start();

//        HWND hwnd = FindWindow(L"Qt5QWindowIcon", L"OpenGL");
//        cout << "Find Window Handle" << hwnd << endl;

        while(cap.read(imgManager.originalImg) /*&& escape != 27 && IsWindowVisible(hwnd)*/){
            if (!imgManager.originalImg.empty()) process();
            currentFrame++;

            escape = waitKey(waitTime);

            processTimer.stop();
            buffer[currentFrame % 1000] = processTimer.duration;
            //processTimer.stopAndPrint("Process Loop");

            processTimer.start();
        }

        processTimer.stopAndPrint("Process Loop");
        cout << "Stop Processing" << endl;
        exitFlag = true;
    }

    Mat rgbMask;

    void process() {
        if(redim) {
            float factor = float(targetSize) / float(imgManager.originalImg.rows);
            resize(imgManager.originalImg, imgManager.img, Size(), factor, factor/*, INTER_NEAREST*/);
        }
        else {
            // For real time we don't rescale
            imgManager.img = imgManager.originalImg;
        }

        // Convert the image to grayscale
        cvtColor(imgManager.img, imgManager.grayImg, COLOR_BGR2GRAY);

        // We loop several passes but break it as soon as it finds the markers
        int numPasses = thresholdTech->numPasses();
        bool foundCentral = false;
        for(int i = 0; i < numPasses; i++) {
            thresholdTech->process(imgManager.grayImg, imgManager.thrImg, i);

            centralMarkers.clear();
            foundCentral = centralMarkerDetector.process(imgManager.thrImg, centralMarkers);
            //cout << (foundCentral ? "Yes, we found." : "No, we didn't find.") << endl;

            if(foundCentral)
                break;
        }

        if(!foundCentral) {
            render(false);
            return;
        }

        //blobs.clear();
        bool foundCircular = circularMarkerDetector.findCircularMarkers(
                    imgManager, centralMarkers, blobs, centers, visibilityMask);

        render(foundCircular);
    }

    void render(bool drawTattoo = false) {
        if(drawTattoo) {
            drawer->draw(imgManager, blobs, centers);
            // Why converting to rgb is more performant overall???
            //cvtColor(imgManager.skinImg, rgbMask, COLOR_GRAY2BGR);
            //renderer->setSkinMask(rgbMask);
            //renderer->update(imgManager.inpaintedImg, centers, drawTattoo);

            Size size = tattoo.size();
            float width = (size.width - 1)/3;
            float height = (size.height - 1)/3;

            vector<Point2f> pts_src{Point2f(0,0), Point2f(width,0), Point2f(2*width,0), Point2f(3*width,0),
                                   Point2f(0,height), Point2f(width,height),
                                   Point2f(2*width,height), Point2f(3*width,height),
                                   Point2f(0,2*height), Point2f(width,2*height),
                                   Point2f(2*width,2*height), Point2f(3*width,2*height),
                                   Point2f(0,3*height), Point2f(width,3*height),
                                   Point2f(2*width,3*height), Point2f(3*width,3*height)};


            vector<Point2f> pts_dst{centers[0], centers[1], centers[2], centers[3],
                                   centers[4], centers[5], centers[6], centers[7], centers[8],
                                   centers[9], centers[10], centers[11],
                                   centers[12], centers[13], centers[14], centers[15]};

            //vector<Point2f> pts_dst_four{centers[0], centers[3], centers[12], centers[15]};
            Mat foreground;

            Mat h = findHomography(pts_src, pts_dst);
            warpPerspective(tattoo, foreground, h, imgManager.inpaintedImg.size());
            //imshow("foreground", foreground);

            Mat channels[4];
            Mat alpha;
            Mat background = imgManager.inpaintedImg.clone();

            split(foreground, channels);
            alpha = channels[3];
            alpha = alpha & imgManager.skinImg;

            cvtColor(foreground, foreground, CV_BGRA2BGR );

            // Convert Mat to float data type
            foreground.convertTo(foreground, CV_32FC3);
            background.convertTo(background, CV_32FC3);

            // Normalize the alpha mask to keep intensity between 0 and alpha for blending
            alpha.convertTo(alpha, CV_32FC1, 1.0/450);
            Mat t[] = {alpha, alpha, alpha};
            merge(t, 3, alpha);


            // Storage for output image
            Mat ouImage = Mat::zeros(foreground.size(), foreground.type());

            // Multiply the foreground with the alpha matte
            multiply(alpha, foreground, foreground);

            // Multiply the background with ( 1 - alpha )
            multiply(Scalar::all(1.0)-alpha, background, background);

            // Add the masked foreground and background.
            add(foreground, background, ouImage);

            ouImage = ouImage/255;


            imshow("tattoo", ouImage);

        }
        else {
            imshow("tattoo", imgManager.img);
            //renderer->update(imgManager.img, centers, drawTattoo);
        }
    }
};

AugmentedTattoo* augmentedTattoo;

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_chooseTattoo_clicked() {
    QString tattoo = QFileDialog::getOpenFileName(this,
                                                         "Choose Tattoo File",
                                                         "",
                                                         tr("Png Files (*.png)"));
    //getOpenFileNames(this, tr("Open File"),"/path/to/file/",tr("Mp3 Files (*.mp3)"));
    //ui->listWidget->addItems(fileNames);
    ui->tattooPath->setText(tattoo);
}

void MainWindow::on_playButton_clicked() {
    config.tattooPath = ui->tattooPath->text().toUtf8().constData();

    if(ui->webcamRadio->isChecked()) {
        config.inputType = AugmentedTattooConfig::Webcam;
        config.webcamID = 0; // TODO: allow multiple
    }
    else if(ui->imageRadio->isChecked()) {
        config.inputType = AugmentedTattooConfig::Image;
        config.videoPath = ui->inputPath->text().toUtf8().constData();
    }
    else if(ui->videoRadio->isChecked()) {
        config.inputType = AugmentedTattooConfig::Video;
        config.videoPath = ui->inputPath->text().toUtf8().constData();
    }

    if(ui->whiteRadio->isChecked()) {
        config.color = -1;
    }
    else if(ui->blackRadio->isChecked()) {
        config.color = 1;
    }

    config.redimSize = ui->sizeCheck->isChecked();
    config.targetSize = ui->targetSize->text().toInt();

    augmentedTattoo = new AugmentedTattoo(config.tattooPath);

    augmentedTattoo->imgManager.markerColor =
            config.color == 1 ? MarkerColor::Darker : MarkerColor::Brighter;
    augmentedTattoo->redim = config.redimSize;
    augmentedTattoo->targetSize = config.targetSize;

    if(config.inputType == AugmentedTattooConfig::Image) {
        augmentedTattoo->showImg(config.videoPath);
    }
    else if(config.inputType == AugmentedTattooConfig::Webcam) {
        augmentedTattoo->configureWebcam(0);
        augmentedTattoo->run();
    }
    else if(config.inputType == AugmentedTattooConfig::Video) {
        augmentedTattoo->configureStream(config.videoPath);
        augmentedTattoo->run();
    }
}

void MainWindow::on_chooseInput_clicked() {
    QString input = QFileDialog::getOpenFileName(this, "Choose Input File",
           "", tr("Image and Video (*.*)"));
    ui->inputPath->setText(input);
}

void MainWindow::activatePathCapture(bool checked) {
    ui->inputPath->setEnabled(checked);
    ui->chooseInput->setEnabled(checked);
}

void MainWindow::on_videoRadio_toggled(bool checked) {
    activatePathCapture(checked);
}

void MainWindow::on_actionAbout_triggered() {
    QDialog* about = new QDialog(0,0);

    Ui_Dialog aboutUi;
    aboutUi.setupUi(about);

    about->exec();
}

void MainWindow::on_imageRadio_toggled(bool checked) {
    activatePathCapture(checked);
}

void MainWindow::on_sizeCheck_toggled(bool checked) {
    ui->targetSize->setEnabled(checked);
}

void MainWindow::show(){
    QMainWindow::show();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QMessageBox::StandardButton resBtn =
            QMessageBox::question( this, "Augmented Tattoo",
                                   tr("Deseja encerrar o programa?\n"),
                                   QMessageBox::No |
                                   QMessageBox::Yes, QMessageBox::Yes);

    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    }
    else {
        //Verifica se o cap foi criado, evita que dê erro se for fechado antes de apertar o play!
        if (augmentedTattoo!=NULL) {
            augmentedTattoo->cap.release();
        }
        event->accept();
    }

}
