#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxBox2d.h"
#include "player.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define RACE_SECONDS 180

class testApp : public ofBaseApp
{
	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void resetProjectorAlignment();
        bool saveProjectorAlignment();
        bool loadProjectorAlignment();
        void computeProjectorAlignment();

		void contactStart(ofxBox2dContactArgs &e);
        void contactEnd(ofxBox2dContactArgs &e);

        ofVideoGrabber 		    vidGrabber;
        ofxCvColorImage		    colorImg;
        ofxCvGrayscaleImage     grayImage;
        ofxCvContourFinder 	    contourFinder;

		int 				    threshold;
		int                     minSize;
		int                     maxSize;
		float                   elapsedSec, frameElapsed;

		bool                    reloadBlobs;
		bool                    drawBlobs;
		bool                    showInfo;
		bool                    showCam;
		bool                    rotatingStart;

		ofPoint                 cameraPoint;

		player                  p1;
		player                  p2;

		ofxBox2d			    box2d;
		vector<ofxBox2dPolygon> triangles;

		ofImage                 tarmac;

        //--- SETUP VARIABLES ---//
        vector<cv::Point2f> projectorPoints;
        cv::Mat toProjectorMatrix;
        bool alignmentComplete;
};

