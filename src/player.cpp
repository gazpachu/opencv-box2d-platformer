#include "player.h"

#define MAX_JUMP -3.0f
#define MAX_X_ACCEL -5.0f
#define X_ACCEL 0.15f
#define Y_ACCEL -0.1f

//--------------------------------------------------------------
void player::init( b2World* world, int _id, int _x, int _y )
{
    id = _id;
    pos.x = _x;
    pos.y = _y;

    string character = "superman/";

    if( id == 2 )
        character = "batman/";

    // Load graphics
    for( int i = 0; i < 20; i++ )
    {
        ofImage img;
        img.loadImage(character + ofToString(i) + ".png");
        sprite.push_back(img);
    }

    leftStart = 8;
    leftStop = 14;
    rightStart = 1;
    rightStop = 7;
    currentFrame = 0;

    height = sprite[0].getHeight();
    width = sprite[0].getWidth();
    xAccel = 0.0f;
    yAccel = 0.0f;
    vStatus = -1;
    hStatus = -1;
    fallingVel = 0.0f;
    flying = false;
    falling = false;

    b2world = world;

    setPhysics(2,0,2.0f);
    setup(b2world, pos.x, pos.y, width/2, height/2);
    setFixedRotation(true);
    bodyDef.fixedRotation = true;
    body->SetFixedRotation(true);
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;
    bodyDef.linearDamping = 1;
    bodyDef.angularDamping = 1;
    body->SetUserData( this );
}

//--------------------------------------------------------------
void player::update()
{
    // Update idle status
    if( vStatus == -1 ) yAccel = 0.0f;
    if( hStatus == -1 ) xAccel = 0.0f;

    // Update vertical acceleration
    if( vStatus == P_UP ) yAccel += Y_ACCEL;
    if( vStatus == P_DOWN ) yAccel -= Y_ACCEL;

    // Update horizontal acceleration
    if( hStatus == P_RIGHT && xAccel < abs(MAX_X_ACCEL) ) xAccel += X_ACCEL;
    else if( hStatus == P_LEFT && xAccel > MAX_X_ACCEL ) xAccel -= X_ACCEL;

    if( vStatus != -1 || hStatus != -1 ) body->SetLinearVelocity(b2Vec2(xAccel, yAccel));

    b2Vec2 vel = body->GetLinearVelocity();

    if( vel.y > fallingVel )
        falling = true;
    else
        falling = false;

    fallingVel = vel.y;
}

//--------------------------------------------------------------
void player::paint()
{
    ofEnableAlphaBlending();

    ofPoint newPos = getPosition();
    newPos.x -= width/2;
    newPos.y -= height/2;

    sprite[currentFrame].draw(newPos);

    //draw(); // Box2D
}

//--------------------------------------------------------------
void player::updateSprite()
{
    switch( vStatus )
    {
        case -1:        // Restore body after falling
                        if( currentFrame == 19 ) currentFrame = 17;
                        else if( currentFrame == 18 ) currentFrame = 16;
                        break;

        case P_UP:      // Flying
                        if( currentFrame <= rightStop ) currentFrame = 16;
                        else if( currentFrame != 16 ) currentFrame = 17;
                        break;
    }

    if( !flying && !falling )
    {
        switch( hStatus )
        {
            case -1:        // not walking
                            if( currentFrame <= rightStop || currentFrame == 16 ) currentFrame = 0;
                            else currentFrame = 15;
                            break;

            case P_LEFT:    // walking left
                            if( currentFrame < leftStart || currentFrame >= leftStop ) currentFrame = leftStart;
                            else currentFrame++;
                            break;

            case P_RIGHT:   // walking right
                            if( currentFrame < rightStart || currentFrame >= rightStop ) currentFrame = rightStart;
                            else currentFrame++;
                            break;
        }
    }

    if( flying )
    {
        switch( hStatus )
        {
            case P_LEFT: currentFrame = 17; break; // fly left
            case P_RIGHT: currentFrame = 16; break; // fly right
        }
    }

    if( falling )
    {
        if( currentFrame == 17 ) currentFrame = 19;
        else if( currentFrame == 16 ) currentFrame = 18;
    }
}

//--------------------------------------------------------------
void player::keyPressed( direction dir )
{
    if( dir == P_UP || dir == P_DOWN )
    {
        vStatus = dir;
        if( dir == P_UP ) flying = true;
    }
    else
    {
        hStatus = dir;
    }
}

//--------------------------------------------------------------
void player::keyReleased( direction dir )
{
    if( dir == P_UP || dir == P_DOWN )
    {
        vStatus = -1;
    }
    else
    {
        hStatus = -1;
    }
}


