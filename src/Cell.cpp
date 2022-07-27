/*
 * CLASS OF CELLS USED AS ENVIRONMENT FOR THE ENTITIES
 *
 *
 *
 */

#include "Cell.h"

Cell::Cell()

{
}

void Cell::setup(float _posX, float _posY, float _sz, bool _state) {
  posX = _posX;
  posY = _posY;
  sz = _sz;
  state = _state;
  newState = false;

  bg = ofColor(255);
  fg = ofColor(0);
}

void Cell::update() {

}

void Cell::display() {
  //SET COLOUR FROM STATE
  ofColor c = state==true?fg:bg;
  ofSetColor(c);

  ofDrawRectangle(posX, posY, sz, sz);
}

void Cell::setColor(ofColor newColor) {
  fg = newColor;
}

//CHECK WITHIN BOUNDS OF CELL
bool Cell::withinBounds(float _x, float _y) {
  return(_x >= posX && _x <= posX + sz && _y >= posY && _y <= posY + sz);

}

//FLIP THE STATE OF CELL
void Cell::stateFlip() {
  state = !state;
}

void Cell::stateOn() {
  state = true;
}

void Cell::stateOff() {
  state = false;
}
