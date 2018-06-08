#pragma once



#include "ofMain.h"
#include "ofxYolo2.h"
#include "IPVideoGrabber.h"
#include "ofxThreadedImageLoader.h"

class ofApp : public ofBaseApp{

	ofxYolo2 yolo;

	ofx::Video::IPVideoGrabber grabber;
	std::vector<ofx::Video::IpVideoGrabberSettings> ipcams;

	ofx::Video::IpVideoGrabberSettings& getNextCamera();
	std::size_t nextCamera = -1;

	ofxThreadedImageLoader imgGrabber;
	ofImage imagenew, imageold;

	std::string currentURL;

	std::vector<bbox_t> lastResults;

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void reloadCameras();
		void loadImagefromWeb();

		float windowAspect = (float)480 / (float)640;
		float aspect;
		float scaleFactorH;
		float scaleFactorW;

		enum camType { MJPG, JPG };
		camType currentCam;
		
};
