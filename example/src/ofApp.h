#pragma once

#include "ofMain.h"
#include "ofxParticleSystemGPU.h"
#include "ofxMioFlowGLSL.h"
#include "MultiTextura.h"

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
	
	private:

		void					setupParticles	( );
		void					reSetup			( int w, int h );					// We call this after on init and after a reset
		//void					renderFrame		( float _width, float _height );
		void					updateParticles	( );
		ofTexture				*getWarpedImg	( ofTexture *texOrigen );

		int						width;
		int						height;
		
		ofImage					img;

		ofxParticleSystemGPU	particleSystem;
		ofxMioFlowGLSL			mioFlow;
		MultiTextura			multiTextura;

		ofShader				vectorFieldShader;
		ofFbo					fbo;
		ofFbo					fboVectorField;

		ofVideoGrabber			videoIn;
		ofFbo					fboWarp;
		ofShader				shaderWarp;

		//--------------------------------------------------------
		//gui
		//
		void					setupGui		( );
		void					save			( int preset );
		void					load			( int preset );

		bool					bGuiVisible;
		ofxPanel				gui_GPUPart;
		ofxToggle				reset;
		ofxIntSlider			particlesW;
		ofxIntSlider			particlesH;
		ofxFloatSlider			oFlowLambdaI;
		ofxFloatSlider			oFlowBlurAmount;
		ofxFloatSlider			oFlowDisplaceAmount;

		int						iPreset;

};
