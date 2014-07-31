#include "ofApp.h"
#include <iostream>
//--------------------------------------------------------------
void ofApp::setup(){
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.setVerbose(true);
    //we can now get back a list of devices.
	vector<ofVideoDevice> devices = vidGrabber.listDevices();
	
    for(int i = 0; i < devices.size(); i++){
		cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
	}
    vidGrabber.setDeviceID(1);
    vidGrabber.initGrabber(videoWidth,videoHeight);
    
#else
    vidPlayer.loadMovie("fingers.mov");
    vidPlayer.play();
#endif
    
    
    pixelStripBoxFull.allocate(videoWidth, videoHeight, 3);
    lerpFull.allocate(pixelStripBoxFull);
    circlePixels.allocate((int)(1.802 * 3.142 * horizonRadius), 5, 3);
    circleLine.allocate(circlePixels);
    serialPixels.allocate(160, 1, 3);
    
    
    subDivisionSize = 10;
    
    
    
	bLearnBakground = true;
	threshold = 80;
    
#ifdef _USE_NUM_LEDS
    int divisor = gcd(videoWidth, videoHeight);
    int widthAspect = videoWidth/divisor;
    int heightAspect = videoHeight/divisor;
    // float subDivisionfloat = (float)(videoHeight/(numLEDs/(2*(widthAspect/heightAspect*videoHeight)+2*videoHeight)));
    
    if (videoWidth/divisor == 16 && videoHeight/divisor == 9) {  //for 16:9
        subDivisionSize = round((float)(videoHeight/((NUM_LEDS_FL/LEDS_PER_CELL_FL)/(50.0/9.0)))); //videoHeight/(numLEDs/(2*(video);
        
    }else if((float)videoHeight/videoWidth == (float)4/3){ //for 4:3
        subDivisionSize = videoHeight/(numLEDs/(14/3));
    }else{                                              //Visual error reporting
        subDivisionSize = 300;
    }
#else
    subDivisionSize = 51;
#endif
}

//--------------------------------------------------------------
void ofApp::update(){
	ofBackground(100,100,100);
    
    bool bNewFrame = false;
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
#else
    vidPlayer.update();
    bNewFrame = vidPlayer.isFrameNew();
#endif
    
	if (bNewFrame){
        
#ifdef _USE_LIVE_VIDEO

        pixelStripBoxFull = vidGrabber.getPixelsRef();
#else
       pixelStripBoxFull =  vidPlayer.getPixelsRef();
#endif
        

        
        
         int g=0;
        while (g < pixelStripBoxFull.getHeight()) {  //move to new row
            
            int g_prime = g;
            int vertSubSize = subDivisionSize;
            
            if (subDivisionSize + g >= pixelStripBoxFull.getHeight()) {
                g_prime = pixelStripBoxFull.getHeight() - g;
                vertSubSize = g_prime;
            }
        
            int  i = 0;
            while (i < pixelStripBoxFull.getWidth()) { //fill in row
                
                int i_prime = i;
                int horSubSize = subDivisionSize;
                if (subDivisionSize + i >= pixelStripBoxFull.getWidth()) {
                    i_prime = pixelStripBoxFull.getWidth() - i;
                    horSubSize = i_prime;
                }
                
                
                //get colors in top-left and bottom-right corners of matrix
                ofColor a = pixelStripBoxFull.getColor(i, g);
                ofColor b = pixelStripBoxFull.getColor(subDivisionSize+i_prime-1, subDivisionSize+g_prime-1);
                
                //linear interpretation between colors
                ofColor lerpedBox = b.lerp(a, 0.5);
                
              
                
                //fill all pixels in submatrix with lerpd color
                for (int j=0; j < horSubSize; j++) {
                    
                    for (int k=0; k < vertSubSize; k++) {
                        
                        
                        int indexAdjX = j+i;
                        int indexAdjY = k+g;
                        
                       // if(k+g < pixelStripBoxFull.getHeight()) pixelStripBoxFull.setColor(indexAdjX, indexAdjY, lerpedBox);
                        pixelStripBoxFull.setColor(indexAdjX, indexAdjY, lerpedBox);
                        switch (g) {
                            case 0:
                                //insert values from lerpedBox into LEDColorBuffer[indexAdjX]
                                break;
                                
                            default:
                                break;
                        }
                    }
                }
                i+=subDivisionSize;
            }
            g+=subDivisionSize;
        }
       
        circleMidpoint_get(videoWidth/2-70, videoHeight/2-50, horizonRadius, ofColor::mediumAquaMarine);
        circleLine.loadData(circlePixels);
        circleMidpoint(videoWidth/2-70, videoHeight/2-50, horizonRadius, ofColor::mediumAquaMarine);
//        for (int t=0; t < circlePixels.size()/5; t++) {
//            serialPixels[t] = circlePixels[t]*127/255;
//        }
        circleSimple(videoWidth/2, videoHeight/2, 100, ofColor::magenta);
       //fillColorArray(videoWidth, videoHeight , subDivisionSize, pixelStripBoxFull, LEDColorBuffer);
        lerpFull.loadData(pixelStripBoxFull);
        
        for(int cir = 0; cir < serialPixels.getWidth(); cir++){
            
            serialPixels.setColor(cir, 0, circlePixels.getColor(cir*circlePixels.getWidth()/serialPixels.getWidth(), 0));
        }

        
        colorOutput = pixelStripBoxFull.getColor(subDivisionSize/2, videoHeight - subDivisionSize/2);
        int temp_index = 0; //top edge of video
        for(float x = subDivisionSize/2; x < videoWidth; x+=subDivisionSize){
            ofColor ColorOut = pixelStripBoxFull.getColor((int)x,0);
            LEDColorBuffer[temp_index][0] = (int)ColorOut.r;
            LEDColorBuffer[temp_index][1] = (int)ColorOut.g;
            LEDColorBuffer[temp_index][2] = (int)ColorOut.b;
            temp_index++;
        }
        //right edge
        for(float y = subDivisionSize/2; y < videoHeight; y+=subDivisionSize){
            ofColor ColorOut = pixelStripBoxFull.getColor(videoWidth,(int)y);
            LEDColorBuffer[temp_index][0] = (int)ColorOut.r;
            LEDColorBuffer[temp_index][1] = (int)ColorOut.g;
            LEDColorBuffer[temp_index][2] = (int)ColorOut.b;
            temp_index++;
        }
        //bottom edge
        for(float x = videoWidth - subDivisionSize/2; x > 0; x-=subDivisionSize){
            ofColor ColorOut = pixelStripBoxFull.getColor((int)x,videoHeight);
            LEDColorBuffer[temp_index][0] = (int)ColorOut.r;
            LEDColorBuffer[temp_index][1] = (int)ColorOut.g;
            LEDColorBuffer[temp_index][2] = (int)ColorOut.b;
            temp_index++;
        }
        //left edge
        for(float y = videoHeight - subDivisionSize/2; y > 0; y-=subDivisionSize){
            ofColor ColorOut = pixelStripBoxFull.getColor(0,(int)y);
            LEDColorBuffer[temp_index][0] = (int)ColorOut.r;
            LEDColorBuffer[temp_index][1] = (int)ColorOut.g;
            LEDColorBuffer[temp_index][2] = (int)ColorOut.b;
            temp_index++;
        }
        bigIndex = temp_index;
	}
  }

//--------------------------------------------------------------
void ofApp::draw(){
    
	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
    //lerpFull.drawSubsection(0, 0, videoWidth, videoHeight, 0, 0);
    
    lerpFull.draw(0, 0);
    ofRect(4,4, 1189, 6);
    circleLine.draw(5, 5);
    
	// then draw the contours:
    
	ofFill();
	   
	// finally, a report:
	ofSetHexColor(0xff00ff);
    
   // ofRect(4,4, 1189, 6);
	stringstream reportStr;
	reportStr << "color info:" << endl
   
    << "round 148.5 " << (int)(148.5) << " " << endl
    << "circle.size()/4 " <<  circlePixels.getColor((circlePixels.size()/4)/3-149, 0) <<endl
    << "circle size /2 " <<  circlePixels.getColor((circlePixels.size()/2)/3-149, 0) << " " << endl
    << "3 circle /4" <<  circlePixels.getColor(circlePixels.size()/4-149, 0) << endl
    << ", fps: " << ofGetFrameRate() <<endl;
	ofDrawBitmapString(reportStr.str(), 20, 600);
    for(int tot = subDivisionSize/2; tot < videoHeight; tot+=subDivisionSize){
        ofCircle(subDivisionSize/2, tot, 1);
    }
    for(int cir = 0; cir < 160; cir++){
        
        ofSetColor(serialPixels.getColor(cir, 0));
        
        ofCircle(cir*videoWidth/160, 600, 4);
    }
    for(int cir = 0; cir < 160; cir++){
        
        ofSetColor(circlePixels.getColor(cir*circlePixels.getWidth()/160, 0));
        serialPixels.setColor(cir, 0, circlePixels.getColor(cir*circlePixels.getWidth()/160, 0));
        ofCircle(cir*videoWidth/160, 700, 4);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
	switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

void ofApp::fillColorArray(int vidWidth, int vidHeight, int subSize, ofPixels &pixels, int colorBuffer[][3]){
    
    int temp_index = 0; //top edge of video
    for(int x = subSize/2; x < vidWidth; x+=subSize){
        ofColor ColorOut = pixels.getColor(x,0);
        colorBuffer[temp_index][0] = ColorOut.r;
        colorBuffer[temp_index][1] = ColorOut.g;
        colorBuffer[temp_index][2] = ColorOut.b;
        temp_index++;
    }
    //right edge
    for(int y = subSize/2; y < vidHeight; y+=subSize){
        ofColor ColorOut = pixels.getColor(vidWidth,y);
        colorBuffer[temp_index][0] = ColorOut.r;
        colorBuffer[temp_index][1] = ColorOut.g;
        colorBuffer[temp_index][2] = ColorOut.b;
        temp_index++;
    }
    for(int x = vidWidth - subSize/2; x > 0; x-=subSize){
        ofColor ColorOut = pixels.getColor(x,vidHeight);
        colorBuffer[temp_index][0] = ColorOut.r;
        colorBuffer[temp_index][1] = ColorOut.g;
        colorBuffer[temp_index][2] = ColorOut.b;
        temp_index++;
    }
    //left edge
    for(int y = vidHeight - subSize/2; y > 0; y-=subSize){
        ofColor ColorOut = pixels.getColor(0,y);
        colorBuffer[temp_index][0] = ColorOut.r;
        colorBuffer[temp_index][1] = ColorOut.g;
        colorBuffer[temp_index][2] = ColorOut.b;
        temp_index++;
    }

    
}

void ofApp::circlePoints(int cx, int cy, int x, int y, ofColor pix)
{
   // int act = Color.red.getRGB();
    
    if (x == 0) {
        pixelStripBoxFull.setColor(cx, cy +y, ofColor::orange);
        pixelStripBoxFull.setColor(cx, cy - y, ofColor::magenta);
        pixelStripBoxFull.setColor(cx + y, cy, ofColor::purple);
        pixelStripBoxFull.setColor(cx - y, cy, ofColor::yellow);
        xEquals0+=4;
    } else
        if (x == y) {
            pixelStripBoxFull.setColor(cx + x, cy + y, ofColor::orange);
            pixelStripBoxFull.setColor(cx - x, cy + y, pix);
            pixelStripBoxFull.setColor(cx + x, cy - y, ofColor::magenta);
            pixelStripBoxFull.setColor(cx - x, cy - y, pix);
            xEqualsY+=4;
        } else
            if (x < y) {
                pixelStripBoxFull.setColor(cx + x, cy + y, ofColor::orange);
                pixelStripBoxFull.setColor(cx - x, cy + y, ofColor::green);
                pixelStripBoxFull.setColor(cx + x, cy - y, ofColor::magenta);
                pixelStripBoxFull.setColor(cx - x, cy - y, pix);
                pixelStripBoxFull.setColor(cx + y, cy + x, ofColor::orange);
                pixelStripBoxFull.setColor(cx - y, cy + x, ofColor::green);
                pixelStripBoxFull.setColor(cx + y, cy - x, ofColor::magenta);
                pixelStripBoxFull.setColor(cx - y, cy - x, pix);
                xLessThanY+=8;
            }
}

void ofApp::circleMidpoint(int xCenter, int yCenter, int radius, ofColor pix)
{
    
    int x = 0;
    int y = radius;
    int p = (5 - radius*4)/4;
    
    circlePoints(xCenter, yCenter, x, y, pix);
    while (x < y) {
        x++;
        if (p < 0) {
            p += 2*x+1;
        } else {
            y--;
            p += 2*(x-y)+1;
        }
        circlePoints(xCenter, yCenter, x, y, pix);
    }
   // cout << "TotalPoints: " << xEquals0 + xEqualsY + xLessThanY << endl;
   // cout << "Horizon Radius attempt: " << (int)(1.802 * 3.142 * horizonRadius) << endl;
    
    xLessThanY = 0;
    xEqualsY = 0;
    xEquals0 = 0;
}
void ofApp::circleSimple(int xCenter, int yCenter, int radius, ofColor c)
{
    
    int x, y, r2;
    
    r2 = radius * radius;
    for (x = -radius; x <= radius; x++) {
        y = (int) (sqrt(r2 - x*x) + 0.5);
        pixelStripBoxFull.setColor(xCenter + x, yCenter + y, c);
        pixelStripBoxFull.setColor(xCenter + x, yCenter - y, c);
    }
}
void ofApp::circlePoints_get(int cx, int cy, int x, int y, ofColor pix)
{
    // int act = Color.red.getRGB();
    
    if (x == 0) {
        for (int yDim = 0; yDim < 4; yDim++) {
        circlePixels.setColor(0, yDim, pixelStripBoxFull.getColor(cx, cy +y));
        circlePixels.setColor((circlePixels.size()/4)/3, yDim, pixelStripBoxFull.getColor(cx, cy - y));
        circlePixels.setColor((circlePixels.size()/2)/3, yDim, pixelStripBoxFull.getColor(cx + y, cy));
        circlePixels.setColor(circlePixels.size()/4, yDim, pixelStripBoxFull.getColor(cx - y, cy));
        }
        xEquals0+=4;
    } else
        if (x == y) {
            for (int yDim = 0; yDim < 4; yDim++) {
            circlePixels.setColor(x, yDim, pixelStripBoxFull.getColor(cx + x, cy + y));
            circlePixels.setColor((circlePixels.size()/4)/3 + x, yDim, pixelStripBoxFull.getColor(cx - x, cy + y));
            circlePixels.setColor(circlePixels.size()/4+x, yDim, pixelStripBoxFull.getColor(cx + x, cy - y));
            circlePixels.setColor((circlePixels.size()/2)/3 + x, yDim, pixelStripBoxFull.getColor(cx - x, cy - y));
            }
            xEqualsY+=4;
        } else
            if (x < y) {
                for (int yDim = 0; yDim < 4; yDim++) {
                
                circlePixels.setColor(297-x, yDim, pixelStripBoxFull.getColor(cx + x, cy + y));
                circlePixels.setColor((int)((circlePixels.size()/4)/3) + x, yDim, pixelStripBoxFull.getColor(cx - x, cy + y));
                circlePixels.setColor((int)(circlePixels.size()/4) + (x), yDim, pixelStripBoxFull.getColor(cx + x, cy - y));
                circlePixels.setColor((int)((circlePixels.size()/2)/3) + (297 - x), yDim, pixelStripBoxFull.getColor(cx - x, cy - y));
                circlePixels.setColor((x), yDim, pixelStripBoxFull.getColor(cx + y, cy + x));
                circlePixels.setColor((int)((circlePixels.size()/4)/3) + (297 - x) , yDim, pixelStripBoxFull.getColor(cx - y, cy + x));
                circlePixels.setColor((int)(circlePixels.size()/4) + (297 - x), yDim, pixelStripBoxFull.getColor(cx + y, cy - x));
                circlePixels.setColor((int)((circlePixels.size()/2)/3) + x, yDim, pixelStripBoxFull.getColor(cx - y, cy - x));
                }
                xLessThanY+=8;
            }
    
}

void ofApp::circlePoints_get_width(int cx, int cy, int x, int y, ofColor pix)
{
    // int act = Color.red.getRGB();
    
    if (x == 0) {
        for (int yDim = 0; yDim < 4; yDim++) {
            circlePixels.setColor(circlePixels.getWidth()/4, yDim, pixelStripBoxFull.getColor(cx, cy +y));
            circlePixels.setColor((3*circlePixels.getWidth()/4), yDim, pixelStripBoxFull.getColor(cx, cy - y));
            circlePixels.setColor(0, yDim, pixelStripBoxFull.getColor(cx + y, cy));
            circlePixels.setColor(circlePixels.getWidth()/2, yDim, pixelStripBoxFull.getColor(cx - y, cy));
        }
        xEquals0+=4;
    } else
        if (x == y) {
            for (int yDim = 0; yDim < 4; yDim++) {
                circlePixels.setColor(x, yDim, pixelStripBoxFull.getColor(cx + x, cy + y));
                circlePixels.setColor((circlePixels.getWidth()/4) + x, yDim, pixelStripBoxFull.getColor(cx - x, cy + y));
                circlePixels.setColor(3*circlePixels.getWidth()/4+x, yDim, pixelStripBoxFull.getColor(cx + x, cy - y));
                circlePixels.setColor((circlePixels.getWidth()/2) + x, yDim, pixelStripBoxFull.getColor(cx - x, cy - y));
            }
            xEqualsY+=4;
        } else
            if (x < y) {
                for (int yDim = 0; yDim < 4; yDim++) {
                    
                    circlePixels.setColor(297-x, yDim, pixelStripBoxFull.getColor(cx + x, cy + y));
                    circlePixels.setColor((int)((circlePixels.getWidth()/4)) + x, yDim, pixelStripBoxFull.getColor(cx - x, cy + y));
                    circlePixels.setColor((int)(3*circlePixels.getWidth()/4) + (x), yDim, pixelStripBoxFull.getColor(cx + x, cy - y));
                    circlePixels.setColor((int)((circlePixels.getWidth()/2)) + (297 - x), yDim, pixelStripBoxFull.getColor(cx - x, cy - y));
                    circlePixels.setColor((x), yDim, pixelStripBoxFull.getColor(cx + y, cy + x));
                    circlePixels.setColor((int)((circlePixels.getWidth()/4)) + (297 - x) , yDim, pixelStripBoxFull.getColor(cx - y, cy + x));
                    circlePixels.setColor((int)(3*circlePixels.getWidth()/4) + (297 - x), yDim, pixelStripBoxFull.getColor(cx + y, cy - x));
                    circlePixels.setColor((int)((circlePixels.getWidth()/2)) + x, yDim, pixelStripBoxFull.getColor(cx - y, cy - x));
                }
                xLessThanY+=8;
            }
    
}

void ofApp::circleMidpoint_get(int xCenter, int yCenter, int radius, ofColor pix)
{
    
    int x = 0;
    int y = radius;
    int p = (5 - radius*4)/4;
    
    circlePoints_get_width(xCenter, yCenter, x, y, pix);
    while (x < y) {
        x++;
        if (p < 0) {
            p += 2*x+1;
        } else {
            y--;
            p += 2*(x-y)+1;
        }
        circlePoints_get_width(xCenter, yCenter, x, y, pix);
      //  cout << "x: " << x << " y: " << y << endl;

    }
    //cout << "x: " << x << " y: " << y << endl;
    

//    cout << "Horizon Radius attempt: " << (int)(1.802 * 3.142 * horizonRadius) << endl;
    
    xLessThanY = 0;
    xEqualsY = 0;
    xEquals0 = 0;
}

