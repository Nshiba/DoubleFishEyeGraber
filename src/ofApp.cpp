#include "ofApp.h"

//float srcX, srcY;

static const int OUTPUT_WIDTH = 2048;
static const int OUTPUT_HEIGHT = 1024;
static const int OUTPUT_WIDTH_1024 = 1024;


ofQuaternion ofQuatFromEul(ofVec3f eul)
{
    ofMatrix4x4 mat;
    mat.makeIdentityMatrix();
    mat.rotate(eul.z, 1.0, 0.0, 0.0);
    mat.rotate(eul.x, 0.0, 0.0, 1.0);
    mat.rotate(eul.y, 0.0, 1.0, 0.0);
    ofQuaternion _q = mat.getRotate();

    return _q;
}

//--------------------------------------------------------------
void ofApp::setup() {
    // ofSetBackgroundAuto(false);

    // - - - - GLSL - - - - - - - - - - - - - - - - //
    shader.load("JKH3");

    // - edge blur mask, can be custumized with PNG
    maskImage.loadImage("MaskingTexture.png");

#ifdef USE_MOVIE

    player.loadMovie("testInput0.mp4");

#elif defined USE_CAMERA
    vGrabber[0].setDeviceID(3);
    vGrabber[0].initGrabber(1920, 1080);
    vGrabber[1].setDeviceID(7);
    vGrabber[1].initGrabber(1920, 1080);

#elif defined USE_DEBUGIMAGE

    fishEyeImage[0].loadImage("testEq_0.png");
    fishEyeImage[1].loadImage("testEq_1.png");

#endif

    // - - - - FBO - - - - - - - - - - - - - - - - //
    output.allocate(OUTPUT_WIDTH, OUTPUT_HEIGHT);
    output.begin();
    ofClear(0, 0, 0, 255);
    output.end();

    // - - - - PARAMTERES   -- -- - - - //

    for (int i = 0; i < 2; i++)
    {
        mCameraParams[i].mFIshEyeFOV = 251;
        mCameraParams[i].mFishEyeCenter = ofPoint(1920 / 2.0, 1080 / 2.0);
        mCameraParams[i].mRadius = 900;
        mCameraParams[i].mFIshEyeFOV = 251;
        mCameraParams[i].mCameraParam_angle = ofPoint(0);
        mCameraParams[i].enable = true;
    }

    //-
    mFont.loadFont("font/DINB.ttf", 48);

    // - - - - SETTING UP UI  -- -- - - - //

    guiParam = new ofxUICanvas(ofGetWidth() * 0.7, 0, ofGetWidth() * 0.3, ofGetHeight());
    guiParam->setTheme(OFX_UI_THEME_HACKER);
    guiParam->setFont("font/DINB.ttf");
    guiParam->addLabel("Double head Fish Eye conversion");
    guiParam->addFPS();


    guiParam->addSpacer();
    guiParam->addLabel("syphon screen size");
    guiParam->addLabelButton("2048x1024", false);
    guiParam->addLabelButton("1024x512", false);

    guiParam->addSpacer();

    guiParam->addToggle("Convert", &isConvet_enabled);

    enable_GLSL = false;
    guiParam->addToggle("enable_GLSL", &enable_GLSL);

    guiParam->addSlider("K1", -0.20, 0.20, &lenDistortionParam.x);
    guiParam->addSlider("K2", -0.20, 0.20, &lenDistortionParam.y);
    guiParam->addSlider("K3", -0.20, 0.20, &lenDistortionParam.z);

    //Where did I get this parameter..?
    lenDistortionParam = ofVec3f(0.0, -0.115, 0.0);

    //lenDistortionParam
    guiParam->addSpacer();
    guiParam->addSpacer();

    for (int i = 0; i < 2; i++)
    {

        guiParam->addLabel("Camera" + ofToString(i));

        guiParam->addSpacer();

        guiParam->addLabelToggle("Enable Texture", &mCameraParams[i].enable);

        guiParam->addSpacer();

        guiParam->addSlider(ofToString(i) + ":FishEyeCenterX", 0, 1920, &mCameraParams[i].mFishEyeCenter.x);
        guiParam->addSlider(ofToString(i) + ":FishEyeCenterY", 0, 1080, &mCameraParams[i].mFishEyeCenter.y);
        guiParam->addSlider(ofToString(i) + ":FishEyeRadius", 0, 1920, &mCameraParams[i].mRadius);
        guiParam->addSlider(ofToString(i) + ":FishEyeFOV", 0, 360, &mCameraParams[i].mFIshEyeFOV);

        guiParam->addSpacer();

        guiParam->addSlider(ofToString(i) + ":ROT X", 0, 360, &mCameraParams[i].mCameraParam_angle.x);
        guiParam->addSlider(ofToString(i) + ":ROT Y", 0, 360, &mCameraParams[i].mCameraParam_angle.y);
        guiParam->addSlider(ofToString(i) + ":ROT Z", 0, 360, &mCameraParams[i].mCameraParam_angle.z);

        guiParam->addSpacer();
        guiParam->addSpacer();

    }

    guiParam->loadSettings("cameraParams.xml");

    ofAddListener(guiParam->newGUIEvent, this, &ofApp::guiEvent);


}

//--------------------------------------------------------------
void ofApp::update() {


#ifdef USE_MOVIE
    player.update();
#elif defined USE_CAMERA
    vGrabber[0].update();
    vGrabber[1].update();

    if (!vGrabber[0].isFrameNew() || !vGrabber[1].isFrameNew())
    {
        return;
    }
#elif defined USE_DEBUGIMAGE


#endif

    //CPU conversion
    /*
    if(isConvet_enabled)
    {
    generateConveted(eqImage[0], 0);
    generateConveted(eqImage[1], 1);
    }
    */

    // - - - - - - - - - - - - - - - - - - - - - - - - - //
    output.begin();
    ofClear(0, 0, 0, 255);

    float w = OUTPUT_WIDTH;
    float h = OUTPUT_HEIGHT;

    if (true) {
        shader.begin();

        shader.setUniform1f("destX", w);
        shader.setUniform1f("destY", h);

#ifdef USE_MOVIE

        //DO nothing.

#elif defined USE_CAMERA

        shader.setUniform1i("sourceType", 0);
        shader.setUniform1f("srcX", vGrabber[0].getWidth());
        shader.setUniform1f("srcY", vGrabber[0].getHeight());

        //sourceType
        shader.setUniformTexture("texture0", vGrabber[0].getTexture(), 10);
        shader.setUniformTexture("texture1", vGrabber[1].getTexture(), 11);
        shader.setUniformTexture("texture2", maskImage, 12);

#elif defined USE_DEBUGIMAGE
        shader.setUniform1f("srcX", fishEyeImage[0].getWidth());
        shader.setUniform1f("srcY", fishEyeImage[0].getHeight());

        shader.setUniformTexture("texture0", fishEyeImage[0], 10);
        shader.setUniformTexture("texture1", fishEyeImage[1], 11);
        shader.setUniformTexture("texture2", maskImage, 12);

#endif
        //maskImage
        //lenDistortionParam

        shader.setUniform1f("distK1", lenDistortionParam.x);
        shader.setUniform1f("distK2", lenDistortionParam.y);
        shader.setUniform1f("distK3", lenDistortionParam.z);

        for (int i = 0; i < 2; i++) {
            shader.setUniform1f("fishEye_Radius" + ofToString(i), mCameraParams[i].mRadius);
            shader.setUniform1f("fishEye_FOV" + ofToString(i), mCameraParams[i].mFIshEyeFOV);
            shader.setUniform2f("fishEye_Center" + ofToString(i), mCameraParams[i].mFishEyeCenter.x, mCameraParams[i].mFishEyeCenter.y);

            ofQuaternion q = ofQuatFromEul(mCameraParams[i].mCameraParam_angle);
            ofMatrix4x4 mat;
            mat.makeIdentityMatrix();
            mat.rotate(q);
            shader.setUniformMatrix4f("cameraParam_angleMatrix" + ofToString(i), mat);

            // shader.setUniform1i("enableTexture" + ofToString(i), (mCameraParams[i].enable) ? 0 : 0);
            shader.setUniform1i("enableTexture" + ofToString(i), 1);

            //mCameraParams[i].enable

        }

    }

    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2f(0, 0);

    glTexCoord2f(0, h);
    glVertex2f(0, h);

    glTexCoord2f(w, h);
    glVertex2f(w, h);

    glTexCoord2f(w, 0);
    glVertex2f(w, 0);

    glEnd();

    if (true) {
        shader.end();
    }

    output.end();

    ofImage image;
    ofPixels pixels;
    output.readToPixels(pixels);
    image.setFromPixels(pixels);

    if (isFirst)
    {
        image.saveImage("first.png");
        isFirst = false;
    }
    else
    {
        image.saveImage("foo.png");

    }
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - //

    // mainOutputSyphonServer.publishTexture(&output.getTextureReference());


}

static float t = 0.1;

//--------------------------------------------------------------
void ofApp::draw() {

    ofClear(0);

    ofSetColor(255);
    //元映像を表示する。
    int _NUM = 2;
    for (int i = 0; i < _NUM; i++)
    {

        bool isAllocated = false;
        float _w, _h;
#ifdef USE_MOVIE

#elif defined USE_CAMERA
        isAllocated = true;
        _w = vGrabber[i].getWidth();
        _h = vGrabber[i].getHeight();

        //[i]
#elif defined USE_DEBUGIMAGE
        isAllocated = fishEyeImage[i].isAllocated();
        _w = fishEyeImage[i].getWidth();
        _h = fishEyeImage[i].getHeight();

#endif

        if (isAllocated)
        {
            ofPushMatrix();
            ofTranslate(ofGetWidth() / _NUM * i, 0);
            ofSetColor(255);

#ifdef USE_MOVIE

#elif defined USE_CAMERA

            ofPushMatrix();
            ofScale((ofGetWidth() / (float)_NUM) / (float)vGrabber[i].getWidth(), ofGetWidth() / (float)_NUM / (float)vGrabber[i].getWidth());
            vGrabber[i].draw(ofPoint(0, 0));
            ofPopMatrix();

#elif defined USE_DEBUGIMAGE
            fishEyeImage[i].draw(
                0, 0,
                ofGetWidth() / _NUM,
                ofGetWidth() / _NUM * ((float)_h / (float)_w));

#endif

            //Draw some
            float mDispScale = (ofGetWidth() / _NUM) / _w;

            ofSetColor(255, 255, 120);
            ofCircle(mCameraParams[i].mFishEyeCenter * mDispScale, 5);
            ofSetCircleResolution(40);
            ofNoFill();
            ofCircle(mCameraParams[i].mFishEyeCenter * mDispScale, mCameraParams[i].mRadius * mDispScale);

            //ofDrawAxis(100);

            ofPopMatrix();
        }
    }

    ofSetColor(255);
    ofTranslate(0, ofGetHeight() * 0.4);

    float _w = ofGetWidth();

    // output.draw(0, 1080);
    
    if (output.isAllocated())
    {
        // std::cout << "isAlloated" << std::endl;

        ofPushMatrix();
        output.draw(0, 0, _w, _w * 0.5);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofPopMatrix();

    }
    else
    {
        std::cout << "notAlloated" << std::endl;

    }
    

    ofSetupScreen();
}

//--------------------------------------------------------------
void ofApp::exit()
{
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

    if (key == ' ')
    {
        isConvet_enabled = !isConvet_enabled;
    }
    else if (key == 's')
    {
        guiParam->saveSettings("cameraParams.xml");
    }
    else if (key == 'f')
    {
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

    /*
    if(ofGetKeyPressed('c'))
    {
    ofPoint drag( x / mDispScale - mFishEyeCenter.x, y / mDispScale - mFishEyeCenter.y);
    mRadius = drag.length();
    }else{
    mFishEyeCenter.x = x / mDispScale;
    mFishEyeCenter.y = y / mDispScale;
    }
    */
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    /*
    if(ofGetKeyPressed('c'))
    {
    mFishEyeCenter.x = x / mDispScale;
    mFishEyeCenter.y = y / mDispScale;
    }
    */
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

void ofApp::mouseEntered(int x, int y)
{
}

void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

//ここの処理をシェーダにする
//This process is done By CPU, and was ported in GSLS
void ofApp::generateConveted(ofImage &image, int i)
{
    const static ofColor NanCol(0, 10, 0);

    // - - - - test - - - - - - - - - - - - - - - - //
    image.allocate(2024 / 8, 1024 / 8, OF_IMAGE_COLOR);

    float srcY = image.getHeight();
    float srcX = image.getWidth();

    ofRectangle rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = fishEyeImage[i].getWidth();
    rect.height = fishEyeImage[i].getHeight();

    for (int x = 0; x < image.getWidth(); x++)
    {
        for (int y = 0; y < image.getHeight(); y++)
        {
            //PHI - vertical
            //THETA - horizontal

            //これは、直径かな？
            //float R = 1800.0;
            float R = 2 * mCameraParams[i].mRadius;

            //check inside FishEyeImage
            float PHI = (y / srcY - 0.5) * PI;
            float THETA = (x / srcX - 0.5) * 2.0 * PI;

            //
            ofVec3f psph;
            psph.x = cos(PHI) * sin(THETA);
            psph.y = cos(PHI) * cos(THETA);
            psph.z = sin(PHI);


            //mCameraParam
            ofQuaternion q = ofQuatFromEul(mCameraParams[i].mCameraParam_angle);
            float ang;
            ofVec3f vec;
            q.getRotate(ang, vec);
            psph.rotate(ang, vec);

            //ここで変換する？

            float fishTheta = atan2(psph.z, psph.x);
            float fishphi = atan2(sqrt(psph.x*psph.x + psph.z*psph.z), psph.y);
            float r = R * fishphi / (mCameraParams[i].mFIshEyeFOV / 180.0 * PI);


            ofPoint fishCenter = mCameraParams[i].mFishEyeCenter;
            ofPoint ref;
            ref.x = fishCenter.x + r * cos(fishTheta);
            ref.y = fishCenter.y + r * sin(fishTheta);

            //getPosInFishEyeImage(horDeg, vertDeg, testCenter, R);

            if (rect.inside(ref))
            {
                image.setColor(x, y, fishEyeImage[i].getColor(ref.x, ref.y));
            }
            else {
                // cout << "OUT SIDE OF IMAGE" << endl;
                image.setColor(x, y, NanCol);
            }

        }
    }
    image.update();

}

void ofApp::setOutputSize(int equiWidth)
{
    output.allocate(equiWidth, equiWidth / 2);
}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    if (name == "2048x1024")
    {
        ofxUIButton *button = (ofxUIButton *)e.getButton();
        if (button->getValue())
        {
            setOutputSize(2048);
        }

    }
    else if (name == "1024x512")
    {
        ofxUIButton *button = (ofxUIButton *)e.getButton();
        if (button->getValue())
        {
            setOutputSize(1024);
        }
    }
}
