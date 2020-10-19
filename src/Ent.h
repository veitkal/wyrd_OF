
#pragma once
#include "ofMain.h"
#include "helpers.h"

class Ent
{
public:
    Ent();
    void setup(float _posX, float _posY, float _sz, float _minX, float _minY, float _maxX, float _maxY);
    void update();
    void display();

    void left();
    void right();
    void adv();
    void checkEdges();

    float posX, posY, sz, minX, minY, maxX, maxY, midX, midY;
    bool inv;
    int dir, state;



};
