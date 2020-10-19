/* THREADED COMPUTER VISION CLASS USING OPTICAL FLOW */

#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxPS3EyeGrabber.h"
#include "ofxKinect.h"

//Namespaces for cleaner code
using namespace ofxCv;
using namespace cv;
//using namespace glm;

class ThreadedCV: public ofThread  {

public:
    ThreadedCV();
    ~ThreadedCV();

    void setup(int _numShafts, int _numWarps);
    void threadedFunction();
    void update(float _multi, float _damp, float _yThresh, float _traction);
    void draw();
    void calcAvrg();

    int getCursor();
    glm::vec2 getDampenedFlow();
    bool getYmotionNeg();
    bool getYreset();
    bool getMotionDetected();
    float getAvgMovement();


    ofVideoGrabber camera;
    ofxCvColorImage decCamera;

    ofxCv::FlowFarneback fb;
    ofxCv::FlowPyrLK lk;
    ofxCv::Flow* curFlow;

    glm::vec2 flow;
    glm::vec2 dampenedflow;
    glm::vec2 prev;
    ofxCvColorImage currentColor;
    ofxCvColorImage decimatedImage;

    int numShafts, numWarps;
    float cursorX, counterY, counterX;
    bool yMotionPos, yMotionNeg, motionDetected, yReset;
    float multi, damp, yThresh, traction;
    vector<float> warpMovements;
    deque<float> avrgMove;
};
