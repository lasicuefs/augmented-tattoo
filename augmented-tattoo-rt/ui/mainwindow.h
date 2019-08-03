#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

using namespace std;

class AugmentedTattooConfig {
public:
    enum InputType {
        Image,
        Webcam,
        Video
    };

    string tattooPath;

    InputType inputType;

    // Ugly! but ok for now
    string imagePath;
    int webcamID=0;
    string videoPath;

    int color = 1;

    bool redimSize = false;
    int targetSize = 720;

};

#include <QMessageBox>
#include <QCloseEvent>
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    AugmentedTattooConfig config;
    bool closed = false;

    void show();

    void activatePathCapture(bool checked);

    void closeEvent(QCloseEvent* event);

private slots:

    void on_chooseTattoo_clicked();

    void on_playButton_clicked();

    void on_chooseInput_clicked();

    void on_videoRadio_toggled(bool checked);

    void on_actionAbout_triggered();

    void on_imageRadio_toggled(bool checked);

    void on_sizeCheck_toggled(bool checked);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
