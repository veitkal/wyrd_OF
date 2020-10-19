
/*
 *
 *
 * OPTICAL FLOW CLASS USING 'ofxCv'
 *
 * multithreaded computervision system reading movemnts as direction and velocity in x/y axis
 * used to control a virtual cursor as well as triggers when movement is detected in certain areas
 *
 * Ref:
 * "Optical Flow Test," Adrian Sheerwood, Accessed April 15, 2020. https://aaron-sherwood.com/blog/?p=700
 * "ofxCV", Kyle McDonald. https://github.com/kylemcdonald/ofxCv
 *
 *
*/

#include "ofMain.h"
#include "ThreadedCV.h"


ThreadedCV::ThreadedCV()
{

  startThread();
}

ThreadedCV::~ThreadedCV()
{
  waitForThread(true);
}

void ThreadedCV::setup(int _numShafts, int _numWarps) {

  //    //CAMERA
  camera.setGrabber(std::make_shared<ofxPS3EyeGrabber>()); //COMMENT TO USE INTERNAL CAMERA
  //    vidGrabber.setDeviceID(1);                                 //UNCOMMENT TO USE INTERNAL CAMERA
  //    vidGrabber.setPixelFormat(OF_PIXELS_NATIVE);
  camera.setDesiredFrameRate(60);
  camera.initGrabber(640, 480);
  camera.getGrabber<ofxPS3EyeGrabber>()->setAutogain(false);
  camera.getGrabber<ofxPS3EyeGrabber>()->setAutoWhiteBalance(false);

  cursorX = 0; //used for controling the treadling ie where the treadling is, influenced by movement in x axis
  counterY = 0; //counter used as lag or small delay to smoothen flip/trigger of bool
  counterX = 0; //counter used as lag or small delay to smoothen flip/trigger of bool
  yMotionNeg = true; //toggle decided by negative movement in Y axis
  yMotionPos = true; //toggle decided by positiv movement in Y axis
  motionDetected = false; //checks for movement
  yReset = false; //trigger checking negative Y motion
  prev=glm::vec2(ofGetWidth()/2,ofGetHeight()/2); //previous

  multi = 150; //64
  damp = 0.05;
  yThresh = -10;
  traction = 2.0;
  currentColor.allocate(640, 480);
  float decimate = 0.25;
  decimatedImage.allocate( currentColor.width * decimate, currentColor.height * decimate );


  curFlow = &fb;
  numShafts = _numShafts;
  warpMovements.resize(29);
  avrgMove.resize(100);
}


void ThreadedCV::threadedFunction() {
  camera.update();
  ofPixels pixels;
  while(isThreadRunning()) {
    if(camera.isFrameNew()) {


      //      //Reading pixles and convert to ofxCVImage
      pixels = camera.getPixels();
      currentColor.setFromPixels( pixels );

      //Decimate images to 25%, a lot less expensive and lets you keep higher resolution camera input. Ref: Theo Papatheodorou see readme
      decimatedImage.scaleIntoMe( currentColor, CV_INTER_AREA );
      //FLIPPING THE IMAGE
      decimatedImage.mirror(false,true);


      curFlow->calcOpticalFlow(decimatedImage);
      flow=fb.getAverageFlow() * multi; ///*~30?
      flow=glm::vec2(flow.x,flow.y) ;
      dampenedflow+=(flow-dampenedflow)*damp; //~.05
      prev+=dampenedflow;

      glm::vec2 midZone=glm::vec2(ofGetWidth()/2,ofGetHeight()/2);
      glm::vec2 returnToMid = midZone-prev;
      glm::vec2 norm = glm::normalize(returnToMid);
      float dist = glm::distance(prev, midZone);

      prev.x = ofClamp(prev.x, -100, ofGetWidth()+100);

      //slight controllable traction/current
      if(dist > 3) {
        prev+=norm*(traction);
      }

      //GUARDING STRATEGIES, when clamp doesn't work. Limit + augmented traction
      //if to far beyond the limit, additional traction to smoothly take the "cursor" back
      if(prev.x < -10 ) {
        prev+=norm*traction*3; //~10
      }
      if(prev.x > ofGetWidth()+10) {
        prev+=norm*traction*3; //~10
      }
      if (dampenedflow.x > 1000.) {
        dampenedflow.x=0.;
      }
      if (dampenedflow.x < -1000.) {
        dampenedflow.x=0.;
      }

      int x = ofClamp(prev.x, 0, ofGetWidth());
      float cursorArea = ofGetWidth()/numShafts;

      //positioning a virtual cursor within the treadling
      for (int i = 0; i < numShafts; i++) {
        float cLoc = i * cursorArea;
        if (x < cursorArea) {
          cursorX = 0;
        } else if (x > cLoc && x < cLoc + cursorArea) {
          cursorX = i;
        }
      }

      //non mirrored
      for (int i = 0; i < numShafts; i++) {
        float cLoc = i * cursorArea;
        if (x < cursorArea) {
          cursorX = 0;
        } else if (x > cLoc && x < cLoc + cursorArea) {
          cursorX = i;
        }
      }



      //DETECT Y MOTION WITH SMALL LAG, ie counterY > lagNumber
      if (dampenedflow.y < yThresh && counterY > 30) {   //~-20
        yMotionNeg = !yMotionNeg;
        yReset = !yReset;
        counterY = 0; //RESETTING COUNTER ADDING LAG
      }

      //RESETS yReset TO USE AS TRIGGER
      if(counterY > 31) {
        yReset = 0;
      }

      //DETECT MOTION
      if (dampenedflow.x < -5. || dampenedflow.x > 5. || dampenedflow.y < -5. || dampenedflow.y > 5.) {
        motionDetected = true;
        counterX = 0;
      } else if (counterX > 200){ //500
        motionDetected = false;
      }
    }

    //save average movement
    avrgMove.push_front(prev.x);
    avrgMove.pop_back();
    avrgMove.resize(100);

    counterX++;
    counterY++;
  }
}

void ThreadedCV::draw() {

  ofSetColor(255);
  ofPushMatrix();
  //    ofTranslate(250, 100);
  ofScale(0.5, 0.5);
  decCamera.draw(0,220,640,480);
  curFlow->draw(0,220,640,480);
  //  ofDrawBitmapStringHighlight(ofToString((int) ofGetFrameRate()) + "fps", 10, 20);

  ofPopMatrix();

  // DRAW DEBUG UI
  ofPushMatrix();
  ofSetColor(255,0,255);
  ofDrawBitmapStringHighlight("prevX: " + ofToString(prev.x), 0, 0);
  ofDrawBitmapStringHighlight("prevY: " + ofToString(prev.y), 150, 0);
  ofDrawBitmapStringHighlight("flowx: " + ofToString(dampenedflow.x), 0, 25);
  ofDrawBitmapStringHighlight("flowY: " + ofToString(dampenedflow.y), 150, 25);
  ofDrawBitmapStringHighlight("Y-: " + ofToString(yMotionNeg), 0, 75);
  ofDrawBitmapStringHighlight("Motion: " + ofToString(motionDetected), 150, 75);
  ofDrawBitmapStringHighlight("cursorX: " + ofToString(cursorX), 0, 100);

  //draw cursor position indicator
  ofPushMatrix();
  ofTranslate(90, 90);
  ofSetColor(255);
  ofDrawRectangle(0,0, 40, 10);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 5; j++) {
      ofSetColor(0);
      ofDrawLine(0, i * 10, 40, i * 10);
      ofDrawLine(j*10, 0, j*10, 10);
    }
  }
  ofSetColor(0);
  ofDrawRectangle(cursorX*10, 0, 10, 10);

  //draw cursor flow line
  ofSetColor(0);
  float testX = ofMap(prev.x, 0, ofGetWidth(), 0, 200);
  ofSetLineWidth(3);
  ofDrawLine(50,0, 250, 0);
  ofDrawRectangle(testX+50-5, -5, 10, 10);
  ofPopMatrix();

  ofPopMatrix();
}

//----------------------------
void ThreadedCV::calcAvrg() {

}

///GETTER FUNCTIONS///
/////////////////////j
int ThreadedCV::getCursor() {
  return cursorX;
}

glm::vec2 ThreadedCV::getDampenedFlow() {
  return dampenedflow;
}

bool ThreadedCV::getYmotionNeg() {
  return yMotionNeg;
}

bool ThreadedCV::getYreset() {
  return yReset;
}

bool ThreadedCV::getMotionDetected() {
  return motionDetected;
}

float ThreadedCV::getAvgMovement() {
  float avg = 0;
  for (int i = 0; i < avrgMove.size(); i++) {
    float tempF = avrgMove[i];
    avg+=tempF;
  }

  avg = avg/avrgMove.size();

  return avg;
}
