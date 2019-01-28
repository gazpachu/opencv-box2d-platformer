#include "testApp.h"
#include "ofxXmlSettings.h"

using namespace cv;

//--------------------------------------------------------------
void testApp::setup()
{
    // App Setup
    //ofSetVerticalSync(true);
    //ofSetFrameRate(120);
    reloadBlobs = true;
    showCam = false;
    drawBlobs = false;
    showInfo = false;

	threshold = 85 ;
	minSize = 50;
	maxSize = 15000;
	cameraPoint = ofPoint(0,0);

    // Load graphics
	tarmac.loadImage("tarmac_bg.png");

    // OpenCV Setup (allocate memory)
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(SCREEN_WIDTH, SCREEN_HEIGHT);
    colorImg.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);
    grayImage.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Box2D World Setup
	box2d.init();
	box2d.setGravity(0,10);
	box2d.createBounds();
	//box2d.createGround();
	box2d.setFPS(30.0);
	//box2d.registerGrabbing();

    // Collision listeners
	ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &testApp::contactEnd);

    // Players setup
	p1.init( box2d.getWorld(), 1, 200, 200 );
	p2.init( box2d.getWorld(), 2, 200, 200 );

	// Start the app in setup mode
    /*if (!loadProjectorAlignment()) {
        ofLog(OF_LOG_NOTICE, "No saved alignment found, starting from scratch.");
        resetProjectorAlignment();
    }*/
}

//--------------------------------------------------------------
void testApp::update()
{
    bool bNewFrame = false;

    vidGrabber.grabFrame();
    bNewFrame = vidGrabber.isFrameNew();

    // Update geometry
	if( bNewFrame )
	{
        colorImg.setFromPixels(vidGrabber.getPixels(), SCREEN_WIDTH, SCREEN_HEIGHT);

        if( reloadBlobs )
        {
            reloadBlobs = false;

            grayImage = colorImg;
            grayImage.threshold(threshold);

            //////////////////////////////////////////////////////////////////////
            // Find track contours based on simple image difference
            //////////////////////////////////////////////////////////////////////
            contourFinder.findContours(grayImage, minSize, maxSize, 200, true);	// find holes

            // Destroy the polygons from the world
            for( int i = 0; i < triangles.size(); i++ )
            {
                triangles[i].destroy();
            }
            triangles.clear();

            // Transform the blobs into polygons
            for (int i = 0; i < contourFinder.nBlobs; i++)
            {
                vector<ofPoint> pts = contourFinder.blobs[i].pts;
                ofxBox2dPolygon poly;

                for( int j = 0; j < pts.size(); j++ )
                {
                    poly.addVertex( pts[j] );
                }

                //poly.setAsEdge(true);
                poly.triangulate(15);
                poly.setPhysics(0,0,0.5f);
                poly.setFixedRotation(true);
                poly.create(box2d.getWorld());
                poly.bodyDef.type = b2_staticBody;
                triangles.push_back(poly);
            }
        }
	}

	float current = ofGetElapsedTimef();

    // Update players' sprite
    if( current > frameElapsed + 0.08f )
    {
        frameElapsed = current;
        p1.updateSprite();
        p2.updateSprite();
    }

    // Reload Blobs
	if( current > elapsedSec + 1 )
	{
	    elapsedSec = current;
        reloadBlobs = true;
	}

    // Update players and world
    p1.update();
    p2.update();

    box2d.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackground(0);
    ofEnableAlphaBlending();
    ofSetColor( ofColor(255) );

    tarmac.draw(0,0);

    if( showCam )
    {
        colorImg.draw(cameraPoint.x, cameraPoint.y);
    }

    // Draw OpenCV blobs
    if( drawBlobs )
    {
        for( int i = 0; i < contourFinder.nBlobs; i++ )
        {
            contourFinder.blobs[i].draw(0,0);
        }
    }

    // Draw Shapes
    for( int i = 0; i < triangles.size(); i++ )
    {
        triangles[i].draw();
    }

    // Draw strings
    ofSetHexColor(0xffffff);
    char reportStr[1024];

    // Draw calibration points
    /*for (size_t i = 0; i < projectorPoints.size(); i++)
    {
        Point2f &pt = projectorPoints[i];
        ofCircle(pt.x, pt.y, 5);
    }*/

    if( showInfo )
    {
        sprintf(reportStr, "threshold %i (press: +/-)\nmin size: %i\nmax size: %i\nnum blobs found %i, fps: %f\nTriangles amount: %i", threshold, minSize, maxSize, contourFinder.nBlobs, ofGetFrameRate(), triangles.size());
        ofDrawBitmapString(reportStr, 20, 20);
    }

    // Draw players
	p1.paint();
	p2.paint();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
		case ' ':
			reloadBlobs = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;

        case 'f':
			ofToggleFullscreen();
			break;

        case 'b':
			if( drawBlobs ) drawBlobs = false;
			else drawBlobs = true;
			break;

        case 'm':
			if( showCam ) showCam = false;
			else showCam = true;
			break;

        case 'n':
			if( showInfo ) showInfo = false;
			else showInfo = true;
			break;

        case 'o':
			maxSize += 100;
			break;
		case 'l':
			maxSize -= 100  ;
			break;

        case 'i':
			minSize++;
			break;
		case 'k':
			minSize --;
			break;

        // Player 1
        case OF_KEY_UP: p1.keyPressed(P_UP); break;
        case OF_KEY_DOWN: p1.keyPressed(P_DOWN); break;
        case OF_KEY_LEFT: p1.keyPressed(P_LEFT); break;
        case OF_KEY_RIGHT: p1.keyPressed(P_RIGHT); break;

        // Player 2
        case 'w': p2.keyPressed(P_UP); break;
        case 's': p2.keyPressed(P_DOWN); break;
        case 'a': p2.keyPressed(P_LEFT); break;
        case 'd': p2.keyPressed(P_RIGHT); break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
    switch( key )
    {
        // Player 1
        case OF_KEY_UP: p1.keyReleased(P_UP); break;
        case OF_KEY_DOWN: p1.keyReleased(P_DOWN); break;
        case OF_KEY_LEFT: p1.keyReleased(P_LEFT); break;
        case OF_KEY_RIGHT: p1.keyReleased(P_RIGHT); break;

        // Player 2
        case 'w': p2.keyReleased(P_UP); break;
        case 's': p2.keyReleased(P_DOWN); break;
        case 'a': p2.keyReleased(P_LEFT); break;
        case 'd': p2.keyReleased(P_RIGHT); break;
    }
}

//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e)
{
	if(e.a != NULL && e.b != NULL)
    {
		// if we collide with the ground we do not
		// want to play a sound. this is how you do that
		/*if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle)
        {
            e.a->GetBody()->GetUserData()

		}*/
	}
}

//--------------------------------------------------------------
void testApp::contactEnd(ofxBox2dContactArgs &e)
{
	if(e.a != NULL && e.b != NULL)
    {
        player* pa = static_cast<player*>( e.a->GetBody()->GetUserData() );
        player* pb = static_cast<player*>( e.b->GetBody()->GetUserData() );

        if( pa != NULL )
            switch(pa->id)
            {
                case 1: p1.flying = false; break;
                case 2: p2.flying = false; break;
            }

        if( pb != NULL )
            switch(pb->id)
            {
                case 1: p1.flying = false; break;
                case 2: p2.flying = false; break;
            }

    }
}

//---------------------------------------------------------
void testApp::resetProjectorAlignment()
{
    projectorPoints.clear();
    toProjectorMatrix = Mat::eye(3, 3, CV_32F);
    alignmentComplete = false;
}

//---------------------------------------------------------
bool testApp::saveProjectorAlignment()
{
    if (projectorPoints.size() != 4) {
        return false;
    }

    ofxXmlSettings alignment;
    alignment.addTag("alignment");
    alignment.pushTag("alignment");
    for (size_t i = 0; i < projectorPoints.size(); i++) {
        alignment.addTag("position");
        alignment.pushTag("position", i);
        alignment.addValue("x", projectorPoints[i].x);
        alignment.addValue("y", projectorPoints[i].y);
        alignment.popTag();
    }
    alignment.popTag();

    // TODO If ever upgraded beyond oF 0.700, this returns a boolean
    alignment.saveFile("alignment.xml");
    return true;
}

//---------------------------------------------------------
bool testApp::loadProjectorAlignment()
{
    ofxXmlSettings alignment;
    if (!alignment.loadFile("alignment.xml")) {
        return false;
    }

    alignment.pushTag("alignment");
    if (alignment.getNumTags("position") != 4) {
        return false;
    }

    projectorPoints.clear();
    for (size_t i = 0; i < 4; i++) {
        alignment.pushTag("position", i);

        Point2f p;
        p.x = alignment.getValue("x", 0);
        p.y = alignment.getValue("y", 0);
        projectorPoints.push_back(p);

        alignment.popTag();
    }
    alignment.popTag();

    computeProjectorAlignment();
    return true;
}

//---------------------------------------------------------
void testApp::computeProjectorAlignment()
{
    vector<Point2f> dstPoints(4);
    dstPoints[0] = Point2f(0, 0);
    dstPoints[1] = Point2f(SCREEN_WIDTH, 0);
    dstPoints[2] = Point2f(SCREEN_WIDTH, SCREEN_HEIGHT);
    dstPoints[3] = Point2f(0, SCREEN_HEIGHT);

    // This matrix transforms from point in camera space to points in
    // projector space, i.e. if point a is at (x, y) as seen by the camera,
    // transforming point b at (2x, 2y) will make it appear correct when
    // projected back into the scene.
    toProjectorMatrix = getPerspectiveTransform(&projectorPoints[0], &dstPoints[0]);
    alignmentComplete = true;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    if( projectorPoints.size() < 4)
    {
        projectorPoints.push_back(Point2f(x, y));
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
