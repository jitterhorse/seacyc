#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

	yolo.setup();

	ipcams = ofx::Video::IpVideoGrabberSettings::fromFile("streams.json");

}

ofx::Video::IpVideoGrabberSettings& ofApp::getNextCamera()
{
	nextCamera = (nextCamera + 1) % ipcams.size();
	return ipcams[nextCamera];
}


void ofApp::reloadCameras() {

	if (grabber.isConnected()) {
		grabber.waitForDisconnect();
	}

	auto& settings = getNextCamera();

	if (settings.getType() == "mjpg") {
		grabber.setMaxReconnects(5);
		grabber.setAutoRetryDelay(2000);
		grabber.setURI(settings.getURL());// +"?resolution=320x240");
		grabber.connect();
		currentCam = MJPG;
	}
	
	else if (settings.getType() == "jpg") {
		imageold.clear();
		imagenew.clear();
		currentURL = settings.getURL();
		loadImagefromWeb();
		currentCam = JPG;
	}
	
}


void ofApp::update()
{
	if (currentCam == MJPG && grabber.isConnected()) {
		grabber.update();
		if (grabber.isFrameNew()) {
			lastResults = yolo.detect(grabber.getPixels());
			aspect = grabber.getHeight() / grabber.getWidth();
			scaleFactorW = 640 / grabber.getWidth();
			scaleFactorH = 480 / grabber.getHeight();
		}
	}
	
	else if (currentCam == JPG && imgGrabber.bDoneLoading && imagenew.getWidth() > 0) {
		lastResults = yolo.detect(imagenew.getPixels());
		imageold.cropFrom(imagenew, 0, 0, imagenew.getWidth(), imagenew.getHeight());
		aspect = imageold.getHeight() / imageold.getWidth();
		scaleFactorW = 640 / imageold.getWidth();
		scaleFactorH = 480 / imageold.getHeight();
		loadImagefromWeb();
	}
	


}

void ofApp::loadImagefromWeb() {
	//ofLogNotice(ofToString(currentURL));
	imgGrabber.loadFromURL(imagenew, ofToString(currentURL));
}


//--------------------------------------------------------------
void ofApp::draw(){
	//grabber.draw(ofVec2f(0, 0));

	bool action = false;

	if ((currentCam == MJPG && grabber.isConnected()) || (currentCam == JPG && imageold.getWidth() > 0)) action = true;
	//ofLogNotice(ofToString(action) + " / " + ofToString(currentCam) + " / " + ofToString(imageold.getWidth()));
	if (currentCam == MJPG && action == true) {
			grabber.draw(0, 0, grabber.getWidth()*scaleFactorW, (grabber.getHeight() * scaleFactorH) * ((float)aspect / (float)windowAspect));
	}

	else if (currentCam == JPG && action == true) {
			//ofLogNotice(ofToString(scaleFactorW) + " / " + ofToString(scaleFactorH) + " / " + ofToString(imageold.getWidth()));
			imageold.draw(0, 0, imageold.getWidth()*scaleFactorW, (imageold.getHeight()* scaleFactorH) * ((float)aspect / (float)windowAspect));
	}
	
	ofNoFill();

	for (auto& r : lastResults) {
		ofDrawRectangle(r.x*scaleFactorW , (r.y * scaleFactorH)* ((float)aspect / (float)windowAspect), r.w*scaleFactorW, (r.h * scaleFactorH)* ((float)aspect / (float)windowAspect));
		ofDrawBitmapString(yolo.getName(r.obj_id) + " - " + ofToString(r.prob) ,r.x*scaleFactorW,(r.y* scaleFactorH)* ((float)aspect / (float)windowAspect));
	}
	
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 2) + " // " + ofToString(grabber.getWidth()) + ", " + ofToString(grabber.getHeight()) + " : " + ofToString((float)aspect / (float)windowAspect));

}


void ofApp::keyPressed(int key)
{
	if (key == ' ')
	{
		ofLogNotice("HIT SPACE");
		reloadCameras();
	}
}