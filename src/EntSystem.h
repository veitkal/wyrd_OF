#pragma once
#include "ofMain.h"
#include "helpers.h"
#include "Cell.h"
#include "Ent.h"


class EntSystem
{
public:
    EntSystem();
    void setup(int _numCols, float _sz, int _numEnts, float _minX, float _minY, float _maxX, float _maxY);
    void update(int _flow);
    void display();
    void doChange(Ent& tempEnt, int idx);
    int getStateTotal();
    vector<int> calcFlowArr(int _flow);
    vector<int> getStateArr();
    void massRandomRules();
    void randomRules();
    void randomIndividRule(int idx);
    void idxRules();
    void totalCellFlip();
    void totalCellOff();
    void totalCellOn();
    void totalSideWipe();
    void totalDiagWipe();

    float sz, width, height;
    int numCols,numRows, numEnts, morphT;
    bool morph;
    vector<vector<Cell>> cellGrid;
    vector<Ent> entArr;
    vector<int> states;
    vector<int> flowStates;

};
