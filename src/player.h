#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

enum direction { P_UP = 0, P_DOWN, P_LEFT, P_RIGHT };

class player : public ofxBox2dRect
{
	public:

		void init( b2World* b2world, int id, int _x, int _y );
		void update();
		void paint();

		void setStatus( int st );
		void updateSprite();

		void keyPressed(direction dir);
		void keyReleased(direction dir);

		vector<ofImage> sprite;

        int id;
		int vStatus;
		int hStatus;
		int height;
		int width;
		int leftStart, leftStop, rightStart, rightStop, currentFrame;
		float fallingVel;
		bool flying;
		bool falling;

		ofPoint pos;

        float xAccel, yAccel;

        b2World* b2world;
};

