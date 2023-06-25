#pragma once

#include "ofMain.h"
#include "ofApp.h"
#include "helpers.h"
#include "Draft.h"
//#include "OpticalFlow.h"
#include "ThreadedCV.h"
#include "EntSystem.h"

//addons
#include "ofxThermalPrinter.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "ofxCv.h"

/*
 * WYRD
 *
 * final project for degree show MA Computational Arts, Goldsmiths University of London
 *
 * Built by Jakob Jennerholm Hammar, 2020
 *
 * COPY LEFT - SHARE ALIKE
 *
 * What follows is a program creating generative weaving drafts/notations/constructs
 * and patterns to be printed in real time with a thermal printer.
 *
 * A computational, emergent system of "entities" based on turmites/Langtons Ant is feeding
 * the draft with their current states. Using optical flow/computer vision via ps3EyeCam
 * the system and draft is interacted with.
 *
 *
 * REFERENCE:
 * "Optical Flow Test," Adrian Sheerwood, Accessed September 1, 202y0. https://aaron-sherwood.com/blog/?p=700
 * https://mathworld.wolfram.com/LangtonsAnt.html
 * https://thecodingtrain.com/CodingChallenges/089-langtonsant.html
 * A. Gajardo, A. Moreira, E. Goles - 2002 - Complexity of Langton's ant, Discrete Applied Mathematics
 *
 * "ofxThermalPrinter", Patricio Gonzalez Vivo. https://github.com/patriciogonzalezvivo/ofxThermalPrinter "ofxCV", Kyle McDonald. https://github.com/kylemcdonald/ofxCv
 * "ofxPS3EyeGrabber", Christopher Baker. https://github.com/bakercp/ofxPS3EyeGrabber

DEPENDENCIES:
'ofxGui',
'ofxOpenCv',
'ofxThermalPrinter
'ofxCv',
'ofxPS3EyeGrabber',
'ofxKinect',
 */

#define PORT 33666

class ofApp : public ofBaseApp{

public:
  void setup();
  void update();
  void draw();

  void exit();

  void setupPrinter();
  void printString(string inputString);
  void printImg(ofImage inputImg);
  void printFullDraft();
  void printSession();
  void flowSessionKinect();
  void flowSession();
  void repeatSession();
  void fieldMovement();
  void drawUI(int _x, int _y, int _w, int _h);
  void oscListen();

  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseEntered(int x, int y);
  void mouseExited(int x, int y);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);

  //VARIABLES
  int numWarps, numShafts, numWeft, offsetX, offsetY, updateRate, flipCounter, morphCounter;
  int entStatesTotal;
  float orgX, orgY, width, height, wWidth, wHeight, tWidth, tHeight, cellSize, numBoxPad, cellPad, updateCounter, movementFieldMax;
  bool print, runDraft, displayGui, session;
  int updateMode, displayMode;
  vector<float> movementFieldArr;
  ofTrueTypeFont txt;

  bool kinectMotionDetected;
  int kinectCursor;

  //COLOURS
  ofColor bg, fg;

  ofFbo patternFbo;

  //OBJECTS
  Draft draft;
  ThreadedCV tCV;
  EntSystem entSys;

  //PRINTER
  ofxThermalPrinter printer;

  ofxOscReceiver oscRes;


};

