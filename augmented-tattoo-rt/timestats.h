#ifndef TIMESTATS_H
#define TIMESTATS_H

class PerfTimer {
public:
    double duration = 0.0;

    double _start = 0.0;
    double _end = 0.0;

    void start() {
        _start = (double)getTickCount();
    }

    void stop(){
        _end = (double)getTickCount();
        duration = (( _end - _start) / getTickFrequency()) * 1000.0;
    }

    void stopAndPrint(string const &prefix = "") {
        stop();
        cout << prefix << " Duration: " << duration << endl;
    }
};

#endif // TIMESTATS_H
