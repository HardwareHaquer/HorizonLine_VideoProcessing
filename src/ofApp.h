#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include <math.h>

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
// otherwise, we'll use a movie file

//uncomment this to define lerp subdivision size by number of LEDS
//otherwise set it to arbitrary size

#define _USE_NUM_LEDS

#define                     NUM_LEDS    1000
#define                     LEDS_PER_CELL  2
#define                     NUM_LEDS_FL 1000.0
#define                     LEDS_PER_CELL_FL  2.0


class ofApp : public ofBaseApp{
    
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
    void fillColorArray(int vidWidth, int vidHeight, int subSize, ofPixels &pixels, int colorBuffer[][3]);
    void circleMidpoint(int xCenter, int yCenter, int radius, ofColor c);
    void circlePoints(int cx, int cy, int x, int y, ofColor pix);
    
    void circleMidpoint_get(int xCenter, int yCenter, int radius, ofColor c);
    void circlePoints_get(int cx, int cy, int x, int y, ofColor pix);

    void circleSimple(int xCenter, int yCenter, int radius, ofColor c);
    
#ifdef _USE_LIVE_VIDEO
    ofVideoGrabber 		vidGrabber;
#else
    ofVideoPlayer 		vidPlayer;
#endif
    
 
    ofPixels                pixelStripBoxFull;
    ofPixels                circlePixels;


    ofTexture               lerpFull;
    ofTexture               circleLine;

    ofColor colorOutput;
  
    
    int 				threshold;
    bool				bLearnBakground;
    const int           videoHeight = 720;
    const int           videoWidth = 1280;
    
    int                 subDivisionSize;
     int                 bigIndex;
    int                 LEDColorBuffer[NUM_LEDS/LEDS_PER_CELL][3];
    const int                 numLEDs = NUM_LEDS;
    const int                 LEDsPerCell = LEDS_PER_CELL;
    int                 xEquals0 = 0;
    int                 xEqualsY = 0;
    int                 xLessThanY = 0;
    int                 horizonRadius = 210;

    
    
    
    
};

static int gcd (int a, int b){
    return (b==0) ? a : gcd(b, a%b);
}


