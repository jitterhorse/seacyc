#ifndef _imgCam
#define _imgCam

#include "ofMain.h"
#include "ofxThreadedImageLoader.h"

class imgCam : public ofThread{

public:
		imgCam() {

		}

		void setup(string urlin) {
			this->url = urlin;
			startThread();
			ofLogNotice("thread set up ready!");
		}

		void threadedFunction() {
			while (isThreadRunning()) {
				lock();
				imgLoad.loadFromURL(image, url);
				unlock();
				ofLogNotice("thread is running");
			}
		}

		bool isRunning() {
			return isThreadRunning();
		}

		string url;
		ofImage image;
		ofxThreadedImageLoader imgLoad;

};
#endif