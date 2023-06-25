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

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  //GENERAL
  ofSetFrameRate(30);
  ofHideCursor();


  //VARIABLES
  bg = ofColor(255); //background colour draft
  fg = ofColor(0); //foreground colour draft

  print = false; //check if to print with thermal printer
  runDraft = true; //run/pause update

  numShafts = 8; //number of shafts
  numWarps = 16; //number of warps
  offsetX = 10; //offset where to begin drawing draft
  offsetY = 10;
  orgX = offsetX; //origin of draft ie translated 0
  orgY = offsetY;

  // Set size - old = 800x480
  width = 640 - (offsetX * 2); //temporary size
  height = 480 - (offsetY * 2);
  numBoxPad = 1; //padding between drawnBoxes, calculated as a number of cells ie n*cellSize
  updateRate = 5;
  flipCounter = 0; //counter used in print-mode
  updateCounter = 0;
  //updateModes: 0=sequence, 1=repeat, 2=mirror, 3=spiral
  updateMode = 1;
  //displayModes: 0 = draftOnly, 1=patternOnly, 2=entSystem only, 3=uiOnly, 4=all of them
  displayMode = 0;
  session = false; //false = flow/interactive, true = print
  movementFieldMax = 20; //max of the slow interaction/influence of the entity system

  cellSize = width / (numWarps+numShafts + numBoxPad); //size of cells in draft

  //TYPOGRAPHY
  txt.load("verdana.ttf", 8, true, true);
  txt.setLineHeight(18.0f);
  txt.setLetterSpacing(1.037);

  //SETUP PRINTER
  setupPrinter();
  draft.setup(numShafts, numWarps, orgX, orgY, width, height, numBoxPad, cellSize, bg, fg);
  tCV.setup(numShafts, numWarps);


  //SETUP ENTSYSTEM
  entSys.setup(100, 800/100, numShafts, 0, 0, 640, 480);

  //MOVEMENT FIELD ARRAY, number of counters
  movementFieldArr.resize(numShafts);

  //SETUP PATTERN FBO
  patternFbo.allocate(640, 480);
  patternFbo.begin();
  ofClear(0);
  patternFbo.end();

  //SETUP OSC
  oscRes.setup(PORT);
  cout << "OSC listening on port " << PORT << endl;

}

// EXIT FUNCTION TO CLOSE DOWN PRINTER AND OPTIONALLY PRINT EMPTY LINE
void ofApp::exit(){
  //    printer.println("\n"); //UNCOMMENT TO ADD EXTRA EMPTY SPACE WHEN EXIT
  printer.close();
}

//--------------------------------------------------------------
void ofApp::update(){
  //UPDATE RATE CAP TO AVOID CRASHES
  if(updateRate < 1) {
    updateRate = 1;
  }

  if (session == false) {
    //update with optical flow and camera interaction
    flowSession();
    //influence entSystem
    fieldMovement();
  } else {
    printSession();
    fieldMovement();
  }

  //listen for OSC messages
  oscListen();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofSetColor(0);
  ofFill();
  ofDrawRectangle(0,0,ofGetWindowWidth(), ofGetWindowHeight());

  //DISPLAY
  if(displayMode == 0) {
    draft.drawPattern(0,0, ofGetWindowWidth(), ofGetWindowHeight());
  } else if(displayMode == 1) {
    draft.draw();
  } else if(displayMode == 2){
    entSys.display();
  } else if(displayMode == 3) {
    //draft.draw();
    //draft.drawPattern(0,500, 800, 480);
    //ofPushMatrix();
    //ofTranslate(800, 0);
    //entSys.display();
    //ofPopMatrix();
    //drawUI(800,500, 800, 435);
    drawUI(0,0, ofGetWindowWidth(), ofGetWindowHeight());
  }

  //quick FPS UI
  if (displayGui) {
    ofSetColor(0);
    ofDrawBitmapStringHighlight(ofToString((int) ofGetFrameRate()) + "fps"    , 20, 20);
    ofDrawBitmapStringHighlight("Rate" + ofToString(updateRate), 20, 40);
  }
}

//--------------------------------------------------------------


//--------------------------------------------------------------
//set up thermal printer
void ofApp::setupPrinter(){
  //PRINTER SEETTINGS
  printer.open("/dev/ttyUSB0");
  printer.setReverse(false);
  printer.setBold(true);
  printer.setAlign(MIDDLE);
  printer.setControlParameter(7,160,0); //suggesten in git issues
  /* printer.setControlParameter(20,160,250); //arduino standard */

}
//--------------------------------------------------------------
//print line with thermal printer
void ofApp::printString(string inputString){
  printer.println(inputString);

}
//--------------------------------------------------------------
//print image with thermalPrinter
void ofApp::printImg(ofImage inputImg){
	ofPixels pxls = inputImg.getPixels();
  //printer.print(inputImg, 100);
  printer.print(inputImg, 200);
  /* printer.println("nnnnnn"); */

}
//--------------------------------------------------------------
//print fullDraft with thermalPrinter
void ofApp::printFullDraft(){
  runDraft = false;  //pausing the update
  draft.setupDrawDown(); //calculating the full pattern (ie with the same threading)
  printImg(draft.draftToImg()); //printing full draft
  print = false;  //stopping the printing

  runDraft = true; //resuming the draft
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == '1') { draft.setupTieUpSimple();}
  if (key == '2') { draft.setupTieUpPlex();}
  if (key == '3') { draft.setupTieUpTwill();}
  if (key == '4') { draft.setupTieUpTwill2();}
  if (key == '5') { draft.setupTieUpRandom();}

  //runDrafts
  if (key == 'r'){
    runDraft = !runDraft;
  }
  if (key == 'd'){
    displayGui = !displayGui;
  }

  if (key == 'w'){
    draft.updateWarp = false;
  }

  if (key == 'o'){
    cout << tCV.getAvgMovement() << endl;
  }
  if (key == '.'){
    updateRate-=1;
  }
  if (key == ','){
    updateRate+=1;
  }
  if (key == 'b'){
    entSys.setup(100, 800/100, numShafts, 0, 0, 800, 480);
  }
  if (key == 'm'){
    entSys.morph = true;
  }
  if (key == 'z'){
    if(updateMode < 4) {
      updateMode++;
    } else {
      updateMode = 0;
    }
  }
  if (key == 'x'){
    if(displayMode < 3) {
      displayMode++;
    } else {
      displayMode = 0;
    }
  }
  if (key == 's'){
    session = !session;
  }
  if (key == 'u'){
    entSys.randomRules();
  }
  if (key == 'y'){
    entSys.massRandomRules();
  }
  if (key == 'i'){
    entSys.idxRules();
  }

  //THERMAL PRINTER /////
  if (key == 'p'){
    print = !print;
    cout << print << endl;
  }
}


//--------------------------------------------------------------
//session to use camera interaction, the one used in the installation
void ofApp::flowSession() {
  //setting update rate if movement is detected
  if(tCV.getMotionDetected()) {
    updateRate = 2;
    updateCounter = 180;
  } else {
    updateRate = 48;
  }

  //changing mode if movement detected in y-axis
  /*
  if (updateMode > 3) updateMode = 0;
  if(tCV.getYreset()) {
    updateMode = (int)ofRandom(3);
  }
  */

  //updating the drafts
  if (runDraft && ofGetFrameNum() % updateRate == 0 ) {
    //ENT SYSTEM
    entSys.update(tCV.getCursor());

    //update threading if movement not detected by pushing states from entSystem
    if (tCV.getMotionDetected() == false) {
      draft.updateWarp = false;
      draft.updateWeft = false;

      //Either as sequential one by one
      if(updateMode == 0) {
        draft.pushThreading(entSys.getStateTotal());
      } else if(updateMode == 1) {
        //OR as repeats
        //draft.updateThreadingRepeat(entSys.getStateArr());
        draft.updateThreadingRecurMirror(entSys.getStateArr());
      } else if(updateMode == 2) {
        //                or as mirrored repeats
        draft.updateThreadingMirror(entSys.getStateArr());

      } else if(updateMode == 3) {
        //                or as recursion
        draft.updateThreadingRecur(entSys.getStateArr());

      } else {
        //                else as mode 0
        draft.pushThreading(entSys.getStateTotal());

      }
    }
    //IF motion is detected, push treadling according to optical flow movements
    if (tCV.getMotionDetected() == true) {
      draft.updateWarp = false;
      draft.updateWeft = false;
      draft.pushTreadling(tCV.getCursor());

      //            AND PRINT
      if(print && ofGetFrameNum() % updateRate == 0) {
        ofColor(255);

        //////DEBUG COMMENT
        ofImage tempImg = draft.getCurrentImg();
//printer.printImage(tempImg);
	
        printImg(tempImg.getPixels());
        //
        /* draft.getCurrentImg().draw(0,0); */
      }
    }
    //          update all of the draft
    draft.update();

    //changing updateMode when movement in y-axis is detected
    /*
    if (tCV.yReset) {
      if(updateMode < 4) {
        updateMode++;
      } else {
        updateMode = 0;
      }
    }
    */

    //morph the entSystem/change the cellgrid
    if (morphCounter > 9000 * 30) {
      morphCounter = 0;
      entSys.morph;
    }
    morphCounter++;
  }
}

//--------------------------------------------------------------
//Session for print experiments. Very modular.
void ofApp::printSession() {
  if (runDraft && ofGetFrameNum() % updateRate == 0) {
    //ENT SYSTEM
    entSys.update(tCV.getCursor());
    if (ofRandom(1)>0.98){
      draft.updateWarp = false;
      draft.updateWeft = false;
      if(updateMode == 0) {
        draft.pushThreading(entSys.getStateTotal());
      } else if(updateMode == 1) {
        draft.updateThreadingRepeat(entSys.getStateArr());

      } else if(updateMode == 2) {
        draft.updateThreadingMirror(entSys.getStateArr());

      } else if(updateMode == 3) {
        draft.updateThreadingRecur(entSys.getStateArr());

      } else {
        draft.pushThreading(entSys.getStateTotal());

      }
      draft.pushTreadling(entSys.getStateTotal());
      draft.pushTreadling(tCV.getCursor());
      draft.update();
    } else {
      draft.updateWarp = false;
      draft.updateWeft = false;
      draft.pushTreadling(entSys.getStateTotal());
    }
    if(print) {
      ofColor(255);
      printImg(draft.getCurrentImg());
      //      printFullDraft();
    }


    if (flipCounter > 200) {
      flipCounter = 0;
    }
    draft.update();
    flipCounter++;
  }
}

//session for repeat patterns
void ofApp::repeatSession() {
  if (runDraft && ofGetFrameNum() % updateRate == 0) {
    if (flipCounter < 50) {
      draft.updateWarp = false;
      draft.updateWeft = false;
      //ENT SYSTEM
      entSys.update(tCV.getCursor());
      draft.updateThreadingMirror(entSys.getStateArr());
      //        cout << entSys.getStateTotal() << endl;
      draft.update();
    } else {
      draft.updateWarp = false;
      draft.updateWeft = false;
      //        draft.updateThreadingRepeat(entSys.getStateArr());
      draft.pushTreadling(entSys.getStateTotal());
      entSys.update(tCV.getCursor());
    }

    if(print) {
      ofColor(255);
      printImg(draft.getCurrentImg());
    }

    if (flipCounter > 200) {
      flipCounter = 0;
    }
    draft.update();
    flipCounter++;
  }
}

//--------------------------------------------------------------
//calculating accumulated movements in the room, changing the rules for the enteties
void ofApp::fieldMovement() {
  int idx = tCV.cursorX;
  if(tCV.motionDetected && idx < movementFieldArr.size()) {
    if( movementFieldArr[tCV.cursorX] > movementFieldMax) {
      movementFieldArr[tCV.cursorX] = 0.0;
      entSys.randomIndividRule(tCV.cursorX);
      //draft.updateTieUpRand(tCV.cursorX);
    } else {
      movementFieldArr[tCV.cursorX]+=0.1;
    }
  }
}

//--------------------------------------------------------------
void ofApp::drawUI(int _x, int _y, int _w, int _h) {
  int xR = _x;
  int yR = _y;
  int wR = _w;
  int hR = _h;
  int off = 15;
  ofSetColor(0);
  ofFill();
  ofDrawRectangle(xR,yR, wR,hR);

  //    TYPO
  ofSetColor(255);
  txt.drawString("::Draft Settings::", xR+off, yR+(1*off));
  txt.drawString(ofToString((int)ofGetFrameRate()) + "fps", xR+off, yR+(2*off));
  txt.drawString("Update Rate: " + ofToString(updateRate), xR+off, yR+(3*off));
  txt.drawString("Run Draft [r]: " + ofToString(runDraft), xR+off, yR+(4*off));
  txt.drawString("Mode [z]: " + ofToString(updateMode), xR+off, yR+(5*off));
  txt.drawString("Display [x]: " + ofToString(displayMode), xR+off, yR+(6*off));
  txt.drawString("Session [s]: " + ofToString(session), xR+off, yR+(7*off));


  txt.drawString("::Optical Flow::", xR+off, yR+(15*off));
  txt.drawString("cursorX: " + ofToString(tCV.cursorX), xR+off, yR+(17*off));
  txt.drawString("prevX: " + ofToString(tCV.prev.x), xR+off, yR+(18*off));
  txt.drawString("prevY: " + ofToString(tCV.prev.y), xR+off, yR+(19*off));
  txt.drawString("flowx: " + ofToString(tCV.dampenedflow.x), xR+off, yR+(20*off));
  txt.drawString("flowY: " + ofToString(tCV.dampenedflow.y), xR+off, yR+(21*off));
  txt.drawString("Y-: " + ofToString(tCV.yMotionNeg), xR+off, yR+(22*off));
  txt.drawString("Motion detected: " + ofToString(tCV.motionDetected), xR+off, yR+(23*off));

  // FLOW/CURSOR visualizer
  ofPushMatrix();
  ofTranslate(xR+off, yR+(15.5*off));
  ofSetColor(255);
  ofDrawRectangle(0,0, 40, 10);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < numShafts; j++) {
      ofSetColor(0);
      ofDrawLine(0, i * 10, 40, i * 10);
      ofDrawLine(j*10, 0, j*10, 10);
    }
  }

  ofSetColor(0);
  ofDrawRectangle(tCV.cursorX*10, 0, 10, 10);


  //draw cursor flow line
  ofSetColor(255);
  float testX = ofMap(tCV.prev.x, 0, ofGetWidth(), 0, 160);
  ofSetLineWidth(3);
  ofDrawLine(50,0, 250, 0);
  ofDrawRectangle(testX+70, -5, 10, 10);
  ofPopMatrix();

  // draw camera+ curflow
  ofPushMatrix();
  ofTranslate(xR+(27*off), yR+(15*off));
  ofScale(0.4, 0.4);

  ////////// DEBUG COMMENTED
  /* tCV.decCamera.draw(0,0,640,480); */
  /* tCV.curFlow->draw(0,0,640,480); */
  ofPopMatrix();

  //ENTYSTEM
  ofSetColor(255);
  txt.drawString("::Ent System::", xR+(23 *off), yR+(1*off));
  txt.drawString("Num ents: " + ofToString(entSys.numEnts), xR+(23 *off), yR+(2*off));
  vector<int> currStates;
  for (int i = 0; i < entSys.numEnts; i++) {
    currStates.push_back(entSys.entArr[i].state);
  }
  txt.drawString("Current Rules: " + ofToString(currStates), xR+(23 *off), yR+(3*off));
  txt.drawString("Morph : " + ofToString(entSys.morph), xR+(23 *off), yR+(4*off));

  //ENVIRONMENTAL INFLUENCE
  txt.drawString("::Environmental Influence::", xR+(23 *off), yR+(6*off));
  txt.drawString("Movement Field Max: " + ofToString(movementFieldMax), xR+(23 *off), yR+(7*off));
  txt.drawString("Movement Field Array :" + ofToString(movementFieldArr), xR+(23 *off), yR+(8*off));

  float mvX = xR+(25*off); //org 29
  float mvY = yR+(14*off);
  float mvW = 20;
  float mvH = 75;
  ofSetColor(255);
  ofSetLineWidth(1);
  for(int i = 0; i < movementFieldArr.size(); i++) {
    ofNoFill();
    float offVal = i*mvW+50;
    ofDrawRectangle(mvX+offVal, mvY, mvW, -mvH);

    ofFill();
    float fieldVal = movementFieldArr[i];
    float fieldHeight = ofMap(fieldVal, 0, movementFieldMax, 0, 75);
    ofDrawRectangle(mvX+offVal, mvY, mvW, -fieldHeight);
  }

}

//--------------------------------------------------------------
void ofApp::oscListen(){

	// listen for waiting messages
	while(oscRes.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		oscRes.getNextMessage(m);
		string msgString = m.getAddress();
		cout << "OSC msg: " << msgString << endl;

		// check for mouse moved message
		if(m.getAddress() == "/displaymode") {

			int d = (int)m.getArgAsFloat(0);

			switch(d) {
				case 1: 
					displayMode = 0;
					break;
				case 2: 
					displayMode = 1;
					break;
				case 3: 
					displayMode = 2;
					break;
				case 4: 
					displayMode = 3;
					break;
				default:
					break;
			}
			cout << " Diplay mode is: " << displayMode << endl;
		} else if(m.getAddress() == "/updatemode") {

			int d = (int)m.getArgAsFloat(0);

			switch(d) {
				case 1: 
					updateMode = 0;
					break;
				case 2: 
					updateMode = 1;
					break;
				case 3: 
					updateMode = 2;
					break;
				case 4: 
					updateMode = 3;
					break;
				default:
					break;
			}
			cout << " Update mode is: " << updateMode << endl;
		} else if(m.getAddress() == "/tieup") {

			int d = (int)m.getArgAsFloat(0);

			switch(d) {
				case 1: 
					draft.setupTieUpSimple();
					break;
				case 2: 
					draft.setupTieUpPlex();
					break;
				case 3: 
					draft.setupTieUpTwill();
					break;
				case 4: 
					draft.setupTieUpTwill2();
					break;
				case 5: 
					draft.setupTieUpRandom();
					break;
				default:
					break;
			}
			cout << " Tie-up  is: " << d - 1 << endl;
		} else if(m.getAddress() == "/kinect/id_count") {


			if(ofRandom(1) > 0.75 ) {
				int threadRand = (int) ofRandom(4);
				updateMode = threadRand % 3;
			cout << " ThreadRand: " << threadRand << endl;
			}

			if(ofRandom(1) > 0.6 ) {
				int tieupRand = (int) ofRandom(5);
				switch(tieupRand) {
					case 0: 
						draft.setupTieUpSimple();
						break;
					case 1: 
						draft.setupTieUpPlex();
						break;
					case 2: 
						draft.setupTieUpTwill();
						break;
					case 3: 
						draft.setupTieUpTwill2();
						break;
					case 4: 
						draft.setupTieUpRandom();
						break;
					default:
						break;
				}

				cout << " tieupRand: " << tieupRand << endl;
			}
		}


	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
