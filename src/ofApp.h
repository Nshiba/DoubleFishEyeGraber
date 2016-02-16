#pragma once

#include "ofMain.h"
#include "ofxUI.h"

//#define USE_MOVIE
// #define USE_CAMERA 1
#define USE_DEBUGIMAGE 1


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        bool isFirst = true;
		
        ofTrueTypeFont  mFont;
        
        // - - - -  - - - - Camera Paramters set - - - -  - - - -  - - - - //
        ofVec3f             lenDistortionParam; //K1, K2, K3
        class CameraParams
        {
        public:
            ofPoint         mFishEyeCenter;
            float           mRadius;
            float           mFIshEyeFOV;
            ofVec3f         mCameraParam_angle;
            bool            enable;

        };
        CameraParams    mCameraParams[2];

        // - - - - CPU DEBUG - - - -  - - - -  - - - -  - - - -  - - - - //
        ofImage         fishEyeImage[2];
        ofImage         eqImage[2];


        // - - - - - GPU shading process - - - - //
        bool            enable_GLSL;
        ofShader        shader;
        ofImage         maskImage;

#ifdef USE_MOVIE
        ofVideoPlayer   player;
#elif defined USE_CAMERA 
        ofVideoGrabber vGrabber[2]; 
#endif
        
        //Processing code module
        void            generateConveted(ofImage &image, int i);

        // - - - - - - - - - //
        ofFbo           output;

        // - - - - - - - - - //
        float           mDispScale;
        bool            isConvet_enabled;
        ofPoint         equiPosToPixPos(ofPoint pos);


        //GUI - - - - - - //
        ofxUICanvas     *guiParam;
        void            guiEvent(ofxUIEventArgs &e);//{;}

                                                    //Syphon output size control - - - - //
        void            setOutputSize(int equiWidth);

};
