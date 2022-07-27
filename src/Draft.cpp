/*
 * CLASS GENERATING A WEAVING DRAFT
 *
 * a weaving draft is a codified tool to inform the setup and operation of a weaving loom
 * to help weaving a suggested and visualized pattern
 * * the different parts of the draft are
 *
 * THREADING - how the warp threads is threaded into the different shafts of the loom
 * TIE-UP - how the foot peddles or treadles are connected to the shafts
 * TREADLING - operation of the treadles, which order which foot peaddle is used
 * DRAWDOWN - the pattern the operation will yield
 *
 */

#include "ofMain.h"
#include "Draft.h"
#include "ofApp.h"


Draft::Draft()
{

}

void Draft::setup(int _numShafts, int _numWarps, float _orgX,
                  float _orgY, float _width, float _height, float _numBoxPad, float _cellSize, ofColor _bg, ofColor _fg){

  //number of shafts
  numShafts = _numShafts;
  //number of warp threads
  numWarps = _numWarps;
  //original X
  orgX = _orgX;
  //original Y
  orgY = _orgY;
  width = _width;
  height = _height;
  //size of cells
  cellSize = _cellSize;

  boxPad = _numBoxPad * cellSize; //box padding
  wWidth = numWarps * cellSize; //width of warp and pattern box
  tWidth = numShafts * cellSize; // width of tieUp box
  tHeight = numShafts* cellSize; //height of tieUpBox
  //  wHeight = height - (tHeight+boxPad); //height of draw down and treadling box
  numWeft = floor((height - (tHeight+boxPad))/cellSize);
  wHeight = numWeft*cellSize;

  printWidth = 380; //width of thremal printing area
  printSize = printWidth/numWarps; //cell size for thermal printer

  //Corners of boxes from top right
  threadingX = orgX;
  threadingY = orgY;
  tieUpX = orgX+wWidth+boxPad;
  tieUpY = orgY;
  treadlingX = orgX+wWidth+boxPad;
  treadlingY = orgY + boxPad + tHeight;
  drawDownX = orgX;
  drawDownY = orgY + boxPad+ tHeight;

  //noise + waveforms
  noiseSeed1 = ofRandom(7777); //treadling noise1
  noiseSeed2 = ofRandom(7777); //treadling noise1
  treadlingSin1 = 3.1;
  treadlingSin2 = 4.1;
  treadlingNoise1 = 2.1;
  threadingSin1 = 6.1;
  threadingSin2 = 3.1;
  threadingNoise1 = 1.1;

  //time counter
  t = 0;

  updateWarp = true;
  updateWeft = true;

  //background / forground
  bg = _bg;
  fg = _fg;

  //resizing vectors
  threading.resize(numShafts, vector<int>(numWarps));
  threadingClr.resize(numWarps);
  tieUp.resize(numShafts, vector<int>(numShafts));
  treadling.resize(numWeft);
  treadlingClr.resize(numWeft);
  drawDown.resize(numWeft, vector<int>(numWarps));
  threadingSimple.resize(numWarps); //used to draw waveforms
  shed.resize(numWarps);

  setupThreading();
  setupTieUp();
  setupTreadling();
  //setupDrawDown();
  setupThreadingClr();
  setupTreadlingClr();

  //set up fbo used to send image to thermal printer
  currentRowFbo.allocate(printWidth, printSize, GL_RGBA);
  currentRowFbo.begin();
  ofClear(bg);
  currentRowFbo.end();
}

//--------------------------------------------------------------


void Draft::update(){
  if (updateWarp && !updateWeft) {
    updateThreading();
    updateTieUp();
    updateDrawDown();
  } else if (updateWeft && !updateWarp) {
    updateTreadling();
    updateTieUp();
    updateDrawDownSimple();
  } else if (updateWeft && updateWarp) {
    updateThreading();
    updateTieUp();
    updateTreadling();
    updateDrawDown();

  } else {
    updateDrawDownSimple();
  }

  t+=0.1;
}

//--------------------------------------------------------------

void Draft::draw(){

  //draw background fields
  ofFill();
  ofSetColor(110,20,110);
  ofDrawRectangle(threadingX, threadingY, wWidth, tHeight);

  ofSetColor(10,20,110);
  ofDrawRectangle(tieUpX, tieUpY, tWidth, tHeight);

  ofSetColor(10,110,110);
  ofDrawRectangle(drawDownX, drawDownY,  wWidth, wHeight);

  ofSetColor(75,90,50);
  ofDrawRectangle(treadlingX, treadlingY,  tWidth, wHeight);

  //draw all the parts
  drawThreadingSimple();
  drawTieUp();
  drawTreadling();
  drawDrawDown();

  //DRAWING TO FBO FOR PRINTER IMAGE
  currentRowFbo.begin();
  ofClear(bg);
  drawCurrentRow();
  //  ofDrawLine(0,0,380,0);
  //  ofDrawLine(380, 0, 370,10);
  currentRowFbo.end();

  //  ofColor(255);
  //  currentRowFbo.draw(0,0);
}

//--------------------------------------------------------------

void Draft::setupThreading() {
  //setup threading with random values
  for(int i = 0; i < threading.size(); i++) {
    for(int j = 0; j < threading[0].size(); j++) {
      int randVal = (int)ofRandom(2);
      threading[i][j] = randVal;
      threadingClr[j] = ofColor(255, 0, 0);
    }
  }

  for(int i = 0; i < threading.size(); i++) {
  }
}

//--------------------------------------------------------------

//SETUP OF TIE-UP WITH DIFFERENT CONFIGURATIONS
void Draft::setupTieUp() {
  //set up one each
  for(int i = 0; i < tieUp.size(); i++) {
    for(int j = 0; j < tieUp[0].size(); j++) {
      int tempState = ((j+i) % (numShafts -numShafts/2))!=0?false:true;
      tieUp[i][j] = tempState;
    }
  }
}

void Draft::setupTieUpRandom() {
  for(int i = 0; i < tieUp.size(); i++) {
    for(int j = 0; j < tieUp[0].size(); j++) {
      int randVal = (int)ofRandom(2);
      tieUp[i][j] = randVal;
    }
  }
}
void Draft::setupTieUpSimple() {
  //random one single selected treadle
  for(int i = 0; i < tieUp.size(); i++) {
    for(int j = 0; j < tieUp[0].size(); j++) {
      //RANDOM
      int tempState = ((j+i+1) % (numShafts))!=0?false:true;
      tieUp[i][j] = tempState;
    }
  }
}
void Draft::setupTieUpTwill() {
  for(int i = 0; i < tieUp.size(); i++) {
    for(int j = 0; j < tieUp[0].size(); j++) {
      //RANDOM
      int tempState = ((j+i+1) % (numShafts))!=0?false:true;
      int tempState2 = ((j+i) % (numShafts))!=0?false:true;
      int tempState3 = tempState+tempState2;
      tieUp[i][j] = tempState3;
    }
  }
}
void Draft::setupTieUpPlex() {
  for(int i = 0; i < tieUp.size(); i++) {
    for(int j = 0; j < tieUp[0].size(); j++) {
      //RANDOM
      int tempState = ((j+i) % (2))!=0?false:true;
      tieUp[i][j] = tempState;
    }
  }
}


//--------------------------------------------------------------

void Draft::setupTreadling() {
  for(int i = 0; i < treadling.size(); i++) {
    int randVal = (int)ofRandom(numShafts);
    treadling[i] = randVal;
    treadlingClr[i] = ofColor(0);
  }

}

//--------------------------------------------------------------

void Draft::setupDrawDown() {
  //looping over all the treadles and sets up a full pattern
  int tempTreadle = 0;
  for(int i = 0; i < drawDown.size(); i++) {
    tempTreadle = treadling[i];
    vector<int> testShed = calcShed(tempTreadle);
    drawDown[i] = testShed;
  }
}

// SETUP COLOR
//--------------------------------------------------------------

void Draft::setupTreadlingClr() {
  for(int i = 0; i < treadling.size(); i++) {
    ofColor c = i % 2 == 0 ? 0 : 255;
    treadlingClr[i] = c;
  }

}

void Draft::setupThreadingClr() {
  //setup threading with random values
    for(int i = 0; i < threading[0].size(); i++) {
      ofColor c = i % 2 == 1 ? 0 : 255;
      threadingClr[i] = c;
    }

}

//--------------------------------------------------------------


//UPDATE THREADING WITH HARMONICS, WAVEFORMS + NOISE
void Draft::updateThreading() {
  float n1 = ofMap(ofNoise(t * threadingNoise1+noiseSeed2),0,1,-1,1);
  float s1 = sin(t*threadingSin1);
  float s2 = sin(t*threadingSin2);
  float s = s1+s2+n1;
  float maxVal = (float)numShafts;
  int tempVal = ofClamp(ofMap(s, -1.0, 1.0, 0.0, maxVal), 0, numShafts-1);
  threadingSimple.push_back(tempVal);
  threadingSimple.pop_front();

  for(int i = 0; i < threading.size(); i++) {
    for(int j = 0; j < threading[0].size(); j++) {
      if(threadingSimple[j] == i) {
        threading[i][j] = 1;
      } else {
        threading[i][j] = 0;
      }

    }
  }
}

//--------------------------------------------------------------
//Updates with a mirrored repeat pattern from an input array.
void Draft::updateThreadingMirror(vector<int> _repeatArr) {
  int repNum = _repeatArr.size();
  int tempVal = 0;

  vector<int> revIdx(repNum);
  vector<int> arrIdx(repNum);
  for (int x = 0; x < repNum; x++) {
    revIdx[x] = x;
    arrIdx[x] = x;
  }
  reverse(revIdx.begin(), revIdx.end());
  vector<int> revArr = _repeatArr;
  reverse(revArr.begin(), revArr.end());
  bool tempFlip = false;

  for (int i = 0; i < threadingSimple.size(); i++)  {
    if(i % repNum == 0) {
      tempFlip = !tempFlip;
    }
    if (tempFlip) {
      tempVal = _repeatArr[i%repNum] * (arrIdx[i%repNum]);
    } else {
      tempVal = revArr[i%repNum] * (revIdx[i%repNum]);
      //            tempVal = 4;
    }
    threadingSimple[i] = tempVal;
  }
}

//--------------------------------------------------------------
//Updates with a recurring, unfolding pattern from an input array.
void Draft::updateThreadingRecur(vector<int> _repeatArr) {
  int repNum = _repeatArr.size();
  int tempVal = 0;
  for (int i = 0; i < threadingSimple.size(); i++)  {
    if (i>repNum) {
      if(ofRandom(1) > 0.8) {
        tempVal = _repeatArr[i%repNum] * (int)ofRandom(i)%repNum;
      } else {
        tempVal = _repeatArr[i%repNum] * i%repNum;

      }

    } else {
      tempVal = _repeatArr[i%repNum] * i%repNum;
    }
    threadingSimple[i] = tempVal;
  }
}

//--------------------------------------------------------------
//Updates with a repeated pattern from an input array.
void Draft::updateThreadingRepeat(vector<int> _repeatArr) {
  int repNum = _repeatArr.size();
  for (int i = 0; i < threadingSimple.size(); i++)  {
    int tempVal = _repeatArr[i%repNum] * i%repNum;
    threadingSimple[i] = tempVal;
  }
}

//--------------------------------------------------------------
void Draft::updateTieUp() {
}

//--------------------------------------------------------------
void Draft::updateTieUpRand(int idx) {
  int tempVal;
  for (int i = 0; i < tieUp[idx].size(); i++) {

    tempVal = ofRandom(0.5) > 0.2?1:0;
    tieUp[idx][i] = tempVal;
  }
}

//--------------------------------------------------------------

void Draft::updateTreadling() {
  float n1 = ofMap(ofNoise(t * treadlingNoise1+noiseSeed1),0,1,-1,1);
  float s1 = sin(t*treadlingSin1);
  float s2 = sin(t*treadlingSin2);
  float s = s1+s2+n1;
  float maxVal = (float)numShafts;
  int tempTreadle = ofClamp(ofMap(s, -1.0, 1.0, 0.0, maxVal), 0, numShafts-1);
  //cout << tempTreadle << endl;
  treadling.push_front(tempTreadle);
  treadling.pop_back();
}

//----------------------

//PUSH TREADLING AT INT POSITION
void Draft::pushTreadling(int _tempTreadle) {
  treadling.push_front(_tempTreadle);
  treadling.pop_back();
}
//----------------------------------------

//PUSH TREADLING AT INT POSITION
void Draft::pushThreading(int _tempThread) {
  threadingSimple.push_back(_tempThread);
  threadingSimple.pop_front();
}

//----------------------------------------
void Draft::updateDrawDown() {
  //calculating current shed and updating drawDown
  int tempVal = treadling[0];
  vector<int> testShed = calcShed(tempVal);
  shed = testShed;
  drawDown.push_front(shed);
  drawDown.pop_back();
}
//--------------------------------------------------------------
void Draft::updateDrawDownSimple() {
  //calculating current shed and updating drawDown
  int tempVal = treadling[0];
  vector<int> testShed = calcShedSimple(tempVal);
  shed = testShed;
  drawDown.push_front(shed);
  drawDown.pop_back();
}
//--------------------------------------------------------------

void Draft::drawThreading() {
  //draw background of box
  ofFill();
  ofSetLineWidth(8);
  ofSetColor(bg);
  ofDrawRectangle(threadingX-2, threadingY-2, wWidth+4, tHeight+4);
  //draw edge
  ofNoFill();
  ofSetLineWidth(2);
  ofSetColor(fg);
  ofDrawRectangle(threadingX, threadingY, wWidth, tHeight);

  //draw cells
  for(int i = 0; i  < threading.size(); i++) {
    for(int j = 0; j < threading[i].size(); j++) {
      float x = orgX + (cellSize * j);//uncomment to draw from bottom right, the draft way
      float y = orgY + (cellSize * i);//uncomment to draw from bottom right

      //if current index is 1 colour is fg, else bg
      ofColor c = threading[i][j]>0?fg:bg;
      ofSetColor(c);
      ofFill();

      ofDrawRectangle(x, y, cellSize, cellSize);
    }
  }

  //draw grid
  for(int i = 0; i < threading.size(); i++) {
    for(int j = 0; j < threading[i].size(); j++) {
      ofSetColor(fg);
      float x1 = orgX + (j * cellSize);
      float y1 = orgY + (i * cellSize);

      ofDrawLine(x1, orgY, x1, orgY+tWidth);
      ofDrawLine(orgX, y1, orgX + wWidth, y1);
    }
  }

}

//--------------------------------------------------------------


void Draft::drawThreadingSimple() {
  //draw background of box
  ofFill();
  ofSetLineWidth(8);
  ofSetColor(bg);
  ofDrawRectangle(threadingX-2, threadingY-2, wWidth+4, tHeight+4);
  //draw edge
  ofNoFill();
  ofSetLineWidth(2);
  ofSetColor(fg);
  ofDrawRectangle(threadingX, threadingY, wWidth, tHeight);

  //draw cells
  for(int i = 0; i  < threading.size(); i++) {
    for(int j = 0; j < threading[i].size(); j++) {
      //        float x = orgX + wWidth - cellSize - (cellSize * j); //uncomment to draw from top left
      //        float y = orgY + tHeight - cellSize - (cellSize * i);//uncomment to draw from top left
      float x = orgX + (cellSize * j);//uncomment to draw from bottom right, the draft way
      float y = orgY + (cellSize * i);//uncomment to draw from bottom right

      //if current index is 1 colour is fg, else bg
      ofColor c = threadingSimple[j] == i?fg:bg;
      ofFill();
      ofSetColor(c);

      ofDrawRectangle(x, y, cellSize, cellSize);
    }
  }

  //draw grid
  for(int i = 0; i < threading.size(); i++) {
    for(int j = 0; j < threading[i].size(); j++) {
      ofSetColor(fg);
      float x1 = orgX + (j * cellSize);
      float y1 = orgY + (i * cellSize);

      ofDrawLine(x1, orgY, x1, orgY+tWidth);
      ofDrawLine(orgX, y1, orgX + wWidth, y1);
    }
  }

}

//--------------------------------------------------------------

void Draft::drawTieUp() {
  //draw background of box
  ofFill();
  ofSetLineWidth(8);
  ofSetColor(bg);
  ofDrawRectangle(tieUpX-2, tieUpY-2, tWidth+4, tHeight+4);
  //draw edge
  ofNoFill();
  ofSetLineWidth(2);
  ofSetColor(fg);
  ofDrawRectangle(tieUpX, tieUpY, tWidth, tHeight);

  //draw cells, loop through tieUp arrayy
  for(int i = 0; i < tieUp.size(); i++) {
    for(int j = 0; j < tieUp[0].size(); j++) {
      float x = tieUpX + (i * cellSize); //draw from top left, better for code
      float y = tieUpY + (j * cellSize); //draw from top left
      //         float x = tieUpX + (i * cellSize); //draw form bottom left, the draft way
      //         float y = tieUpY + tWidth - cellSize - (j * cellSize); //draw form bottom left, the draft way

      ofFill();
      //if current index is 1 colour is fg, else bg
      ofColor c = tieUp[i][j]>0?fg:bg;
      ofSetColor(c);

      ofDrawRectangle(x, y, cellSize, cellSize);
    }
  }

  //draw grid
  for(int i = 0; i < tieUp.size(); i++) {
    for(int j = 0; j < tieUp[i].size(); j++) {
      ofSetColor(fg);
      float x1 = tieUpX + (j * cellSize);
      float y1 = tieUpY + (i * cellSize);

      ofDrawLine(x1, tieUpY, x1, tieUpY+tWidth);
      ofDrawLine(tieUpX, y1, tieUpX + tWidth, y1);
    }
  }

}

//--------------------------------------------------------------

void Draft::drawTreadling() {
  //draw background of box
  ofFill();
  ofSetLineWidth(8);
  ofSetColor(bg);
  ofDrawRectangle(treadlingX-2, treadlingY-2, tWidth+4, wHeight+4);
  //draw edge
  ofNoFill();
  ofSetLineWidth(2);
  ofSetColor(fg);
  ofDrawRectangle(treadlingX, treadlingY, tWidth, wHeight);

  //draw treadling, looping over the deque + number of shafts as to calculate cells in grid
  for(int i = 0; i < treadling.size(); i++) {
    for(int j = 0; j < numShafts; j++) {
      float x = treadlingX + (j * cellSize);
      float y = treadlingY + (i * cellSize);

      ofFill();
      //if check if val at index (ie 0-numShafts-1) is the same as j, ie x index
      //set colour to fg if so
      ofColor c = treadling[i] == j?fg:bg;
      ofSetColor(c);

      ofDrawRectangle(x, y, cellSize, cellSize);
    }

  }

  //draw grid
  for(int i = 0; i < numShafts; i++) {
    for(int j = 0; j < treadling.size(); j++) {
      ofSetColor(fg);
      float x1 = treadlingX + (i * cellSize);
      float y1 =treadlingY + (j * cellSize);

      ofDrawLine(x1,treadlingY, x1, treadlingY+wHeight);
      ofDrawLine(treadlingX, y1, treadlingX + tWidth, y1);
    }
  }
}

//--------------------------------------------------------------

void Draft::drawDrawDown() {
  //draw background of box
  ofFill();
  ofSetLineWidth(8);
  ofSetColor(bg);
  ofDrawRectangle(drawDownX-2, drawDownY-2, wWidth+4, wHeight+4);
  //draw edge
  ofNoFill();
  ofSetLineWidth(2);
  ofSetColor(fg);
  ofDrawRectangle(drawDownX, drawDownY, wWidth, wHeight);

  //draw treadling, looping over the deque + number of shafts as to calculate cells in grid
  for(int i = 0; i < drawDown.size(); i++) {
    for(int j = 0; j < drawDown[0].size(); j++) {
      float x = drawDownX + (j * cellSize);
      float y = drawDownY + (i * cellSize);

      ofFill();
      //if check if val at index (ie 0-numShafts-1) is the same as j, ie x index
      //set colour to fg if so 
      //ofColor c = drawDown[i][j] == 1?fg:bg; //old
      //
      //ofColor c = drawDown[i][j] == 1?treadlingClr[i]:threadingClr[j]; //rising shaft
      ofColor c = drawDown[i][j] == 1?treadlingClr[i]:threadingClr[j]; // sinking shaft
      ofSetColor(c);

      ofDrawRectangle(x, y, cellSize, cellSize);
    }

  }

  //draw grid
  /* for(int i = 0; i < drawDown.size(); i++) { */
  /*   for(int j = 0; j < drawDown[0].size(); j++) { */
  /*     ofSetColor(fg); */
  /*     float x1 = drawDownX + (j * cellSize); */
  /*     float y1 = drawDownY + (i * cellSize); */

  /*     ofDrawLine(x1,drawDownY, x1, drawDownY+wHeight); */
  /*     ofDrawLine(drawDownX, y1, drawDownX + wWidth, y1); */
  /*   } */
  /* } */
}

//--------------------------------------------------------------
//calculates the current shed, ie pattern row at selected treadle
vector<int> Draft::calcShed(int _treadle) {
  vector<int> tempVec;
  for(int i = 0; i < threading[0].size(); i++) {
    int tempVal = 0;
    for(int j = 0; j < tieUp[0].size(); j++) {
      int tieUpVal = tieUp[_treadle][j];
      int threadingVal = threading[j][i];
      tempVal += tieUpVal * threadingVal;
      //        converting to binary 0/1
      tempVal = tempVal>0?1:0;
    }
    tempVec.push_back(tempVal);
  }
  return tempVec;
}

//--------------------------------------------------------------
//calculates the current shed if threading is simple, ie pattern row at selected treadle
vector<int> Draft::calcShedSimple(int _treadle) {
  for(int i = 0; i  < threading.size(); i++) {
    for(int j = 0; j < threading[i].size(); j++) {
      int tempInt = threadingSimple[j] == i?1:0;
      threading[i][j] = tempInt;
    }
  }

  vector<int> tempVec;
  for(int i = 0; i < threading[0].size(); i++) {
    int tempVal = 0;
    for(int j = 0; j < tieUp[0].size(); j++) {
      int tieUpVal = tieUp[_treadle][j];
      int threadingVal = threading[j][i];
      tempVal += tieUpVal * threadingVal;
      //        converting to binary 0/1
      tempVal = tempVal>0?1:0;
    }
    tempVec.push_back(tempVal);
  }
  return tempVec;
}
//--------------------------------------------------------------
void Draft::drawCurrentRow() {
  int crX = 0; //current row x pos
  int crY = 0; //curent row y pos
  int crW = printWidth; //current row width
  int crH = printSize; //current row height
  ofSetColor(bg);
  ofDrawRectangle(crX,crY,crW,crH);

  for (int i = 0; i < numWarps; i++) {
    ofSetColor(0);
    float x = crX+(i*printSize);
    float y = crY+printSize;
    //       ofDrawLine(x, crY, x, y);

    //setting current colour of each cell
    /* ofColor c = shed[i]>0?0:255; */ //old
    ofColor c = shed[i]>0?treadlingClr[0]:threadingClr[i];
    ofSetColor(c);

    ofDrawRectangle(x,crY, printSize, printSize);
  }
}
//--------------------------------------------------------------
void Draft::drawPattern(float _px, float _py, float _pw, float _ph) {
  ofSetColor(bg);

  float psz = _pw/numWarps;

  for(int i = 0; i < drawDown.size(); i++) {
    for(int j = 0; j < drawDown[0].size(); j++) {
      float x = _px + (j * psz);
      float y = _py + (i * psz);

      ofFill();
      //if check if val at index (ie 0-numShafts-1) is the same as j, ie x index
      //set colour to fg if so
      //ofColor c = drawDown[i][j] == 1?fg:bg; //old
      ofColor c = drawDown[i][j] == 1?treadlingClr[i]:threadingClr[j]; // sinking shaft
      ofSetColor(c);

      ofDrawRectangle(x, y, psz, psz);
    }
  }

}

//--------------------------------------------------------------

//RETURNS OF IMAGE OF DRAFT FOR PRINTING
ofImage Draft::draftToImg() {
  ofFbo tempFbo;
  tempFbo.allocate(printWidth,height*0.50);
  tempFbo.begin();
  ofClear(255);
  ofPushMatrix();
  ofScale(0.48);
  drawThreading();
  drawTieUp();
  drawTreadling();
  drawDrawDown();
  ofPopMatrix();
  tempFbo.end();

  ofPixels pixels;
  pixels.allocate(ofGetWidth(), ofGetHeight(), 4);

  tempFbo.readToPixels(pixels);
  ofImage tempImg;
  tempImg.setFromPixels(pixels);
  return tempImg;
}

//--------------------------------------------------------------
//returns current shed, or calculated pattern row
string Draft::getCurrentString() {
  return vectorToString(shed);
}

//--------------------------------------------------------------
//RETURNS OFIMAGE OF CURRENT PATTERN ROW FOR PRINTING
ofImage Draft::getCurrentImg() {
  ofPixels pixels;
  /* pixels.allocate(ofGetWidth(), ofGetHeight(), 4); */
  pixels.allocate(800, 400, 4);

  currentRowFbo.readToPixels(pixels);
  ofImage currentImage;
  currentImage.setFromPixels(pixels);

  return currentImage;
}
