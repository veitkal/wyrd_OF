/*
 * CLASS OF CELLS USED AS ENVIRONMENT FOR THE ENTITIES
 *
 *
 *
 */

#pragma once
#include "ofMain.h"
#include "helpers.h"

class Cell
{
public:
    Cell();
    void setup(float _posX, float _posY, float _sz, bool _state);
    void update();
    void display();

    bool withinBounds(float _x, float _y);
    void setColor(ofColor newColor);
    void stateFlip();
    void stateOn();
    void stateOff();

    float posX, posY, sz;
    bool state, newState;
    ofColor bg, fg;


};
