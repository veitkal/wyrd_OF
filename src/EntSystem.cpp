/*
 * CLASS OF SYSTEM OF ENTITIES
 *
 * the ents are loosely based on turmites/Langtons Ant
 *
 *
 */


#include "EntSystem.h"

EntSystem::EntSystem()
{

}

void EntSystem::setup(int _numCols, float _sz, int _numEnts, float _minX, float _minY, float _maxX, float _maxY) {
  sz = _sz; //size
  numEnts = _numEnts; //number of enteties
  morph = true; //morphing of environment
  morphT = 0; //morph time
  width = _maxX - _minX;
  height = _maxY - _minY;
  numCols = _numCols; //number of columns
  numRows = round(height/sz); //number of rows

  //CELL GRID
  cellGrid.resize(numCols, vector<Cell>(numRows));

  //flowstates that is how much influence the entities are getting from camera interaction
  flowStates.resize(numEnts);
  for (int i = 0; i < flowStates.size(); i++){
    flowStates[i] = i;
  }

  //setting up grid of cells
  for (int i = 0; i < numCols; i++) {
    for (int j = 0; j < numRows; j++) {
      float x = sz * i;
      float y = sz * j;

      Cell tempCell;
      bool rState = ofRandom(1) > 0.5?true:false;
      tempCell.setup(x,y,sz,false);
      cellGrid[i][j] = tempCell;
    }
  }


  //SETUP ENTS
  for (int i = 0; i < numEnts; i++) {
    int rx =  (int)ofRandom(cellGrid.size());
    int ry =  (int)ofRandom(cellGrid[0].size());
    float x = cellGrid[rx][ry].posX;
    float y = cellGrid[rx][ry].posY;
    Ent tempEnt;
    tempEnt.setup(x, y, sz, _minX, _minY, _maxX, _maxY);
    states.resize(numEnts);

    entArr.push_back(tempEnt);
  }

  //clear entArray
  entArr.resize(numEnts);
}

void EntSystem::update(int _flow) {
  //calculate flow array
  calcFlowArr(_flow);

  //update and change the ents directions
  for(int i = 0; i < entArr.size(); i++) {
    doChange(entArr[i], i);
    entArr[i].update();
  }

  if (morph == true) {
    totalSideWipe();
  }
}

//THE CHANGING ALGORITHM/RULE APPLICATION
void EntSystem::doChange(Ent& tempEnt, int idx) {
  float x = tempEnt.midX;
  float y = tempEnt.midY;

  for(int i = 0; i < cellGrid.size(); i++) {
    for(int j = 0; j < cellGrid[i].size(); j++) {

      //if within bounds
      if (cellGrid[i][j].withinBounds(x, y)) {
        if (cellGrid[i][j].state == true) {
          tempEnt.right();
          cellGrid[i][j].stateFlip();
          states[idx] = cellGrid[i][j].state==true?1:0;
        } else {
          tempEnt.left();
          cellGrid[i][j].stateFlip();
          states[idx] = cellGrid[i][j].state==true?1:0;
        }
      }
    }
  }
}

//RULE CHANGING FUNCTIONS
//random rule for single entity
void EntSystem::randomIndividRule(int idx) {
  int dirState = (int)ofRandom(4);
  entArr[idx].state = dirState;
}

//single random rule applied for all ents/objects
void EntSystem::massRandomRules() {
  int dirState = (int)ofRandom(4);
  for(int i = 0; i < entArr.size(); i++) {
    entArr[i].state = dirState;
  }
}

//random rules for all ents
void EntSystem::randomRules() {
  int dirState = (int)ofRandom(4);
  for(int i = 0; i < entArr.size(); i++) {
    int dirState = (int)ofRandom(4);
    entArr[i].state = dirState;
  }
}

//index-based rules
void EntSystem::idxRules() {
  for(int i = 0; i < entArr.size(); i++) {
    int dirState = i;
    entArr[i].state = dirState;
  }
}

//flowstates
vector<int> EntSystem::calcFlowArr(int _flow) {
  return flowStates;
}

//DRAW/DISPLAY
void EntSystem::display() {
  for(int i = 0; i < cellGrid.size(); i++) {
    for(int j = 0; j < cellGrid[i].size(); j++) {
      cellGrid[i][j].display();
    }
  }

  for(int k = 0; k < entArr.size(); k++) {
    entArr[k].display();
  }
}

//GET THE TOTAL STATES ADDED TOGETHER INTO ONE INT
int EntSystem::getStateTotal() {
  int totStates = 0;
  for(auto a : states) {
    totStates+=a;
  }
  totStates = ofClamp(totStates, 0, 4);
  return totStates;
}

//GET THE STATE ARRAY
vector<int> EntSystem::getStateArr() {
  return states;
}

//MORPH FUNCTIONS - CHANNGES TO ENVIRONMENT
//--------------------------------------------------------------
void EntSystem::totalCellFlip() {
  for(int i = 0; i < cellGrid.size(); i++){
    for(int j = 0; j < cellGrid[0].size(); j++){
      cellGrid[i][j].stateFlip();
    }
  }
}
void EntSystem::totalSideWipe() {
  if(morph == true && morphT < cellGrid.size()) {
    for(int i = 0; i < cellGrid[0].size(); i++) {
      cellGrid[morphT][i].stateOff();
    }
    morphT++;
  } else {
    morph = false;
    morphT = 0;
  }
}
void EntSystem::totalDiagWipe() {
  if(morph == true && morphT < cellGrid.size()) {
    int i = ofClamp(morphT, 0, cellGrid[0].size()-1);
    cellGrid[i][i].stateOn();
    morphT++;
  } else {
    morph = false;
    morphT = 0;
  }
}
