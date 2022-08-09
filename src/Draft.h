/*
 * CLASS GENERATING A WEAVING DRAFT
 *
 * a weaving draft is a codified tool to inform the setup and operation of a weaving loom
 * to help weaving a suggested and visualized pattern
 *
 * the different parts of the draft are
 *
 * THREADING - how the warp threads is threaded into the different shafts of the loom
 * TIE-UP - how the foot peddles or treadles are connected to the shafts
 * TREADLING - operation of the treadles, which order which foot peaddle is used
 * DRAWDOWN - the pattern the operation will yield
 *
 */


#pragma once
#include "ofMain.h"
#include "helpers.h"

class Draft {

public:
  Draft();

  void setup(int _numShafts, int _numWarps, float _orgX,
             float _orgY, float _width, float _height, float _numBoxPad, float _cellSize, ofColor _bg, ofColor _fg);
  void update();
  void draw();

  //SETUP
  void setupThreading();
  void setupTreadling();
  void setupDrawDown();
  void setupTieUp();
  void setupTieUpRandom();
  void setupTieUpSimple();
  void setupTieUpTwill();
  void setupTieUpPlex();
  void setupTieUpTwill2();
  void setupThreadingClr();
  void setupTreadlingClr();

  //CALCULATIONS
  void calculateFullPattern();
  vector<int> calcShed(int _treadle);
  vector<int> calcShedSimple(int _treadle);

  //UPDATE
  void updateThreading();
  void updateThreadingRepeat(vector<int> _repeatArr);
  void updateThreadingRecur(vector<int> _repeatArr);
  void updateThreadingRecurMirror(vector<int> _repeatArr);
  void updateThreadingMirror(vector<int> _repeatArr);
  void updateTieUp();
  void updateTieUpRand(int idx);
  void updateTreadling();
void pushTreadling(int _tempTreadle);
void pushThreading(int _tempTreadle);
  void updateDrawDown();
  void updateDrawDownSimple();

  //DRAW
  void drawThreading();
  void drawThreadingSimple();
  void drawTieUp();
  void drawTreadling();
  void drawDrawDown();
  void drawCurrentRow();
  void drawPattern(float _px, float _py, float _pw, float _ph);

  //PRINT
  ofImage draftToImg();
  string getCurrentString();
  ofImage getCurrentImg();
  ofPixels getCurrentPixels();

  int numWarps, numShafts, numWeft;
  float orgX, orgY, width, height, wWidth, wHeight, tWidth, tHeight, cellSize, boxPad, cellPad, printWidth, printSize;

  //Corners of boxes from top right
  float threadingX, threadingY, treadlingX, treadlingY, tieUpX, tieUpY, drawDownX, drawDownY, t, noiseSeed1, noiseSeed2;

  //update params
  bool updateWarp, updateWeft;
  //wave params
  float treadlingSin1, treadlingSin2, treadlingNoise1, threadingSin1, threadingSin2, threadingNoise1 ;

  ofColor bg, fg;

  deque<vector<int>> threading;
  deque<ofColor> threadingClr;
  vector<vector<int>> tieUp;
  deque<int> treadling;
  deque<ofColor> treadlingClr;
  deque<vector<int>> drawDown;
  vector<int> shed;
  deque<int> threadingSimple; // a single deque used to draw waveforms in the threading

  bool colorOffset;

  //fbo for thermal printing
  ofFbo currentRowFbo;

};
