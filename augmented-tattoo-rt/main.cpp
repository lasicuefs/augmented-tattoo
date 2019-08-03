#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <qapplication.h>
#include "ui/mainwindow.h"

QApplication* app;

/** @function main */
int main( int argc, char** argv ) {
//    putenv("PATH=lib\\opencv;lib\\qt;");

    app = new QApplication(argc, argv);
    MainWindow mainWin(0);
    mainWin.resize(800, 600);
    mainWin.show();

    app->exec();

    cout << "Exiting..." << endl;

    fflush(stdout);

    return 0;
}
