/*
 * CLASS OF ENTITIES
 *
 * similar to langtons ant / turmites
 *
 * walk a step, turn towards direction
 *
 * REFERENCE:
 * https://mathworld.wolfram.com/LangtonsAnt.html
 * https://thecodingtrain.com/CodingChallenges/089-langtonsant.html
 *
 * A. Gajardo, A. Moreira, E. Goles - 2002 - Complexity of Langton's ant, Discrete Applied Mathematics
 */


#include "Ent.h"

Ent::Ent()

{

}

void Ent::setup(float _posX, float _posY, float _sz, float _minX, float _minY, float _maxX, float _maxY) {
  //VARS
  posX = _posX;
  posY = _posY;
  sz = _sz;
  minX = _minX;
  minY = _minY;
  maxX = _maxX;
  maxY = _maxY;
  //random inversion rule
  inv = ofRandom(1)>0.5?true:false;
  dir = 0; //0 = north, 1 = east, 2 = south = 3 = west;

  //random initial state
  state = (int)ofRandom(4);

}

void Ent::update() {
  midX = posX + sz/2;
  midY = posY + sz/2;
  checkEdges();
}

void Ent::display() {
  //if inverted else
  if(inv) {
    ofSetColor(99,224,139);
  } else {
    ofSetColor(224, 99, 139);
  }

  ofDrawRectangle(posX, posY, sz, sz);
}



//ADVANCE if direction =North East South West go there
void Ent::adv() {

  switch (dir) {
  case 0:
    posY-=sz;
    break;
  case 1:
    posX+=sz;
    break;
  case 2:
    posY+=sz;
    break;
  case 3:
    posX-=sz;
    break;
  default:
    break;
  }

}

void Ent::checkEdges() {
  if (posX > maxX - sz) {
    posX = minX;
  } else if (posX < minX) {
    posX = maxX - sz;
  } else if (posY > maxY - sz) {
    posY = minY;
  } else if (posY < minY) {
    posY = maxY - sz;
  }
}


//THE DIFFERENT RULES OF NAVIGATION
//MOVE "RIGHT"
void Ent::right() {

  switch (state) {
  case 0:
    if(inv) {
      dir = (dir+1) % 4;
    } else {
      dir = ((dir-1)+4) % 4;
    }
    adv();
    break;
  case 1:
    if(inv) {
      dir = (dir+1) % 4;
    } else {
      dir = ((dir-1)+4) % 4;
    }
    adv();
    if(inv) {
      dir = ((dir-1)+4) % 4;
    } else {
      dir = (dir+1) % 4;
    }
    adv();
    break;
  case 2:
    if(inv) {
      dir = (dir+1) % 4;
    } else {
      dir = ((dir-1)+4) % 4;
    }
    adv();
    break;
  case 3:
    adv();
    break;
  default:
    if(inv) {
      dir = (dir+1) % 4;
    } else {
      dir = ((dir-1)+4) % 4;
    }
    adv();
    break;
  }
}

//MOVE "LEFT"
void Ent::left() {
  switch (state) {
  case 0:
    if(inv) {
      dir = ((dir-1)+4) % 4;
    } else {
      dir = (dir+1) % 4;
    }
    adv();
    break;
  case 1:
    if(inv) {
      dir = ((dir-1)+4) % 4;
    } else {
      dir = (dir+1) % 4;
    }
    adv();
    if(inv) {
      dir = ((dir-1)+4) % 2;
    } else {
      dir = (dir+1) % 2;
    }
    adv();
    break;
  case 2:
    adv();
    adv();
    break;
  case 3:
    adv();
    break;
  default:
    if(inv) {
      dir = ((dir-1)+4) % 4;
    } else {
      dir = (dir+1) % 4;
    }
    adv();
    break;
  }
}
