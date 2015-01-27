#include "ofApp.h"

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480

//--------------------------------------------------------------
void ofApp::setup(){
	
	bGuiVisible = false;
	
	particlesW = 1280;
	particlesH = 800;
	
	setupGui();
	
	width = 1280;
	height = 800;
	
	videoIn.setDeviceID(0);
	videoIn.setDesiredFrameRate(60);
	videoIn.initGrabber(VIDEO_WIDTH, VIDEO_HEIGHT, true);
	
	setupParticles();
	
	iPreset = 1;
	load( iPreset );
	
	bDebug = false;

}


void ofApp::setupParticles()
{

	fboWarp.allocate(width, height);	

    particleSystem.init(particlesW, particlesH, width, height);
    
	reSetup (particlesW, particlesH);
    
	img.loadImage("images/Marilin.png");
	//img.loadImage("images/foto.jpg");
	img.resize(width, height);
	particleSystem.loadIntoTexture(ofxParticleSystemGPU::DynamicTextures::COLOR_STATIC, &img.getTextureReference());
	
	vectorFieldShader.load("shaders/GPUParticles/vectorfield2");
    
	ofFbo::Settings s;
    //s.internalformat = GL_RGBA32F_ARB;
    s.internalformat = GL_RGBA;
    s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    s.minFilter = GL_NEAREST;
    s.maxFilter = GL_NEAREST;
    s.wrapModeHorizontal = GL_CLAMP;
    s.wrapModeVertical = GL_CLAMP;
    s.width = width;
    s.height = height;
    s.numColorbuffers = 1;
	
	//FBO para el vector Field
	s.width = VIDEO_WIDTH;
	s.height = VIDEO_HEIGHT;
	fboVectorField.allocate(s);
	
	mioFlow.setup(VIDEO_WIDTH, VIDEO_HEIGHT);

	multiTextura.addAlphabet();
	particleSystem.setMultiTexture(multiTextura.getTexture());
}


void ofApp::reSetup(int w, int h)
{
    // initial positions
    // use new to allocate 4,000,000 floats on the heap rather than
    // the stack

	float* particlePosns = new float[w * h * 4];
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            unsigned idx = y * w + x;
            //particlePosns[idx * 4] = 400.f * x / (float)w - 200.f; // particle x
            //particlePosns[idx * 4 + 1] = 400.f * y / (float)h - 200.f; // particle y
            particlePosns[idx * 4] = (float)x * ((float)width / (float)w); // particle x
            particlePosns[idx * 4 + 1] = (float)y * ((float)height / (float)h); // particle y
            particlePosns[idx * 4 + 2] = 0.f; // particle z
            particlePosns[idx * 4 + 3] = 0.f; // dummy
        }
    }
    particleSystem.loadDataTexture(ofxParticleSystemGPU::POSITION, particlePosns);
    particleSystem.loadDataTexture(ofxParticleSystemGPU::POS_INI, particlePosns);

	//color
    ofColor c1 = particleSystem.particleColor1;
	ofColor c2 = particleSystem.particleColor2;
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            unsigned idx = y * w + x;
            //particlePosns[idx * 4] = ofNoise(x,y);
            //particlePosns[idx * 4 + 1] = 0.5f + ofNoise(x, y, 100) * 0.5f;
            //particlePosns[idx * 4 + 2] = 0.5f + ofNoise(x, y, 600) * 0.5f; // particle z
			particlePosns[idx * 4] = ofRandom((float)c1.r / 255.0, (float)c2.r / 255.0);
            particlePosns[idx * 4 + 1] = ofRandom((float)c1.g / 255.0, (float)c2.g / 255.0);
            particlePosns[idx * 4 + 2] = ofRandom((float)c1.b / 255.0, (float)c2.b / 255.0);
            particlePosns[idx * 4 + 3] = 1.f; // dummy
        }
    }
    particleSystem.loadDataTexture(ofxParticleSystemGPU::COLOR, particlePosns);

	//SIZE_LIFE_MAXDST
	//R = size
	//G = life
	//B = maxDst (maximum distance the particle will arrive from pos_ini)
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            unsigned idx = y * w + x;
            particlePosns[idx * 4] = ofRandom(particleSystem.particleSizeMin, particleSystem.particleSizeMax); //size
            particlePosns[idx * 4 + 1] = ofRandom(particleSystem.vidaMin, particleSystem.vidaMax); //life
            particlePosns[idx * 4 + 2] = ofRandom(particleSystem.maxDstMin, particleSystem.maxDstMax); //maxDst
            particlePosns[idx * 4 + 3] = 0; // dummy
        }
    }

	particleSystem.loadDataTexture(ofxParticleSystemGPU::SIZE_LIFE_MAXDST, particlePosns);
	
	//TEXTURE_COORDS
	//R = x
	//G = y
	//B = width
	//A = height
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            ofPoint p;
			float wi,he;
			multiTextura.getTextureInfo(ofRandom(0,24), &p, &wi, &he);
			
			unsigned idx = y * w + x;
            particlePosns[idx * 4] = p.x; // x
            particlePosns[idx * 4 + 1] = p.y; // y
            particlePosns[idx * 4 + 2] = wi; // width
            particlePosns[idx * 4 + 3] = he; // height
        }
    }
    particleSystem.loadDataTexture(ofxParticleSystemGPU::TEXTURE_COORDS, particlePosns);

	
	delete[] particlePosns;

    // initial velocities
    particleSystem.zeroDataTexture(ofxParticleSystemGPU::VELOCITY);

	// Por defecto se podrán añadir un máximo de 5000 particulas por frame, redondeado a lo que corresponda por filas
	//int lineas = 1;
	//emitterMax = lineas * w;
	//
	//delete [] emitterPOSITION;
	//emitterPOSITION = new float[emitterMax * 4];
	//
	//delete [] emitterVELOCITY;
	//emitterVELOCITY = new float[emitterMax * 4];
	//
	//delete [] emitterCOLOR;
	//emitterCOLOR = new float[emitterMax * 4];
	//
	//delete [] emitterPOS_INI;
	//emitterPOS_INI = new float[emitterMax * 4];
	//
	//delete [] emitterTEXTURE_COORDS;
	//emitterTEXTURE_COORDS = new float[emitterMax * 4];
	//
	//delete [] emitterSIZE_LIFE_MAXDST;
	//emitterSIZE_LIFE_MAXDST = new float[emitterMax * 4];
	//
	//emitterPointer = 0;
	//emitterNewCount = 0;

	particleSystem.loadIntoTexture(ofxParticleSystemGPU::DynamicTextures::COLOR_STATIC, &img.getTextureReference());

}

void ofApp::setupGui()
{
	gui_GPUPart.setup( "", "", 10, 10 ); 
	gui_GPUPart.setName("GPU Particulas");

	gui_GPUPart.add(reset.setup( "RESET", false));
	gui_GPUPart.add(particlesW.setup( "Num Particulas W", 1280, 1, 1280 * 2));
	gui_GPUPart.add(particlesH.setup( "Num Particulas H", 800, 1, 800 * 2));
	gui_GPUPart.add(particleSystem.gravity.setup( "gravity", ofVec2f(0,-0.5), ofVec2f(-100,-100), ofVec2f(100,100)));
	gui_GPUPart.add(particleSystem.particleColor1.setup( "part Color 1",ofColor(127,127,127),ofColor(0,0),ofColor(255,255)));
	gui_GPUPart.add(particleSystem.particleColor2.setup( "part Color 2",ofColor(127,127,127),ofColor(0,0),ofColor(255,255)));
	gui_GPUPart.add(particleSystem.useMultiTex.setup( "use Multi Tex", true));
	gui_GPUPart.add(particleSystem.particleSizeMin.setup( "Particle Size Min", 1.0f, 0.1f, 50.0f));
	gui_GPUPart.add(particleSystem.particleSizeMax.setup( "Particle Size Max", 1.0f, 0.1f, 50.0f));
	gui_GPUPart.add(particleSystem.minVelocidad.setup( "min Velocidad", 1.0f, 0.0f, 10.0f));
	gui_GPUPart.add(particleSystem.radiusSquared.setup( "radiusSquared", 200.0f, 10.0f, 600.0f));
	gui_GPUPart.add(particleSystem.damping.setup( "damping", 0.995f, 0.5f, 1.0f));
	gui_GPUPart.add(particleSystem.backToOrigin.setup( "Back to Origin", 0.0f, 0.0f, 2000.0f));
	gui_GPUPart.add(particleSystem.originStatic.setup( "Origin Static", true));
	gui_GPUPart.add(particleSystem.useParticleTexture.setup( "Particle Texture", true));
	gui_GPUPart.add(particleSystem.colorStatic.setup( "Color Static", true));
	gui_GPUPart.add(particleSystem.useEnlargeByVelocity.setup( "use Enlarge by vel", true));
	gui_GPUPart.add(particleSystem.enlargeByVelocity.setup( "Enlarge by vel", 1.0f, 0.0f, 1.0f));
	gui_GPUPart.add(particleSystem.useSizeByVelocity.setup( "use Size by vel", true));
	gui_GPUPart.add(particleSystem.sizeByVelocity.setup( "Size by vel", 1.0f, 0.0f, 0.1f));
	//gui_GPUPart.add(particleSystem.useVectorField.setup( "use Vector Field", true));
	//gui_GPUPart.add(particleSystem.vectorField.setup( "Vector Field", 0.1f, 1.0f, 500.0f));
	gui_GPUPart.add(particleSystem.useVelocidadColor.setup( "use Velocidad Color", true));
	gui_GPUPart.add(particleSystem.velocidadColor.setup( "Velocidad Color", 500.0f, 300.0f, 1000.0f));
	gui_GPUPart.add(particleSystem.blackWhenStill.setup( "Black when Still", true));
	//gui_GPUPart.add(particleSystem.useVida.setup( "useVida", true));
	//gui_GPUPart.add(particleSystem.vidaMin.setup( "Vida Min", 100.0f, 1.0f, 300.0f));
	//gui_GPUPart.add(particleSystem.vidaMax.setup( "Vida Max", 100.0f, 1.0f, 300.0f));
	gui_GPUPart.add(particleSystem.useMaxDst.setup( "useMaxDst", true));
	gui_GPUPart.add(particleSystem.maxDstMin.setup( "Max Dst Min", 100.0f, 1.0f, 100.0f));
	gui_GPUPart.add(particleSystem.maxDstMax.setup( "Max Dst Max", 100.0f, 1.0f, 1000.0f));
	//gui_GPUPart.add(particleSystem.wind.setup( "Wind", 0, 0, 2));

	gui_GPUPart.add(particleSystem.useOpticalFlow.setup( "use Optical Flow", true));
	gui_GPUPart.add(particleSystem.opticalFlow.setup( "Optical Flow", 0.1f, 1.0f, 2000.0f));
	gui_GPUPart.add(oFlowLambdaI.setup( "oFlowLambdaI", 0.1f, 0.0f, 1.0f));
	gui_GPUPart.add(oFlowBlurAmount.setup( "oFlowBlurAmount", 1.0f, 0.0f, 10.0f));
	gui_GPUPart.add(oFlowDisplaceAmount.setup( "oFlowDisplaceAmount", 0.5f, 0.0f, 1.0f));
}

//--------------------------------------------------------------
void ofApp::update(){

	videoIn.update();

	if (reset)
	{
		particleSystem.init( particlesW, particlesH, width, height );
		reSetup( particlesW, particlesH );
		reset = false;
	}

	if (videoIn.isFrameNew())
	{
		if (particleSystem.useOpticalFlow)
		{
			/*
			ofTexture *tex;
			tex = calibrador->getWarpedImg( &cam->getDepthMasked()->getTextureReference() );
			mioFlow.update(calibrador->fboWarp.getTextureReference(), oFlowLambdaI, oFlowBlurAmount, oFlowDisplaceAmount);
			particles.loadIntoTexture(ofxGpuParticles::DynamicTextures::OPTICAL_FLOW, &mioFlow.getFlowBlurTexture());
			*/
			mioFlow.update(videoIn.getTextureReference(), oFlowLambdaI, oFlowBlurAmount, oFlowDisplaceAmount);
			fboWarp.begin();
				ofClear(0);
				ofSetColor(255,255,255);
				mioFlow.getFlowBlurTexture().draw(0,0);
			fboWarp.end();
			particleSystem.loadIntoTexture(ofxParticleSystemGPU::DynamicTextures::OPTICAL_FLOW, &fboWarp.getTextureReference());
		}
		//if (particleSystem.useVectorField)
		//{
		//	fboVectorField.begin();
		//		ofClear(0,0);
		//		vectorFieldShader.begin();
		//			vectorFieldShader.setUniformTexture("tex0", videoIn.getTextureReference(), 0);
		//			vectorFieldShader.setUniform2f("screenSize", fboVectorField.getWidth(), fboVectorField.getHeight());
		//			renderFrame(fboVectorField.getWidth(), fboVectorField.getHeight());
		//		vectorFieldShader.end();
		//	fboVectorField.end();
		//	particleSystem.loadIntoTexture(ofxParticleSystemGPU::DynamicTextures::VECTOR_FIELD, &fboVectorField.getTextureReference());
		//}
	}


	particleSystem.update();

	ofSetWindowTitle( ofToString(ofGetFrameRate()) + " FPS" );

}

//--------------------------------------------------------------
void ofApp::draw(){

	if (bDebug)
	{
		drawDebug();
		return;
	}

	ofPushStyle();
	
	ofBackground(0,0,0);
	ofSetColor(255,255,255);

	particleSystem.draw();

	ofPopStyle();

	ofSetColor(255,255,255);

	if (bGuiVisible)
		gui_GPUPart.draw();

}

void ofApp::drawDebug()
{
	ofBackground(0,0,0);
	ofSetColor(255,255,255);
	videoIn.getTextureReference().draw(0,0);
	mioFlow.drawFlowGrid(640,480);
	mioFlow.drawFlowGridRaw(640,0);
	mioFlow.drawReposition(0,480);

}

//void ofApp::renderFrame(float _width, float _height)
//{
//	ofMesh quad;
//
//	quad.getVertices().resize(4);
//	quad.getTexCoords().resize(4);
//	quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
//
//	quad.setVertex(0, ofVec3f(0,0,0));
//	quad.setVertex(1, ofVec3f(_width,0,0));
//	quad.setVertex(2, ofVec3f(_width,_height,0));
//	quad.setVertex(3, ofVec3f(0,_height,0));
//			
//	quad.setTexCoord(0, ofVec2f(0,0));
//	quad.setTexCoord(1, ofVec2f(_width,0));
//	quad.setTexCoord(2, ofVec2f(_width,_height));
//	quad.setTexCoord(3, ofVec2f(0,_height));
//			
//	quad.draw(); 
//}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	if (key == ' ')
		bGuiVisible = !bGuiVisible;

	if (key == 's')
		save(iPreset);

	if (key >= '1' && key <= '9')
		load(key - '0');

	if (key == 'd')
		bDebug = !bDebug;
	
	if (key == 'v')
		videoIn.videoSettings();
}

void ofApp::save( int preset )
{
	gui_GPUPart.saveToFile("settings_" + ofToString(preset) + ".xml");
}

void ofApp::load( int preset )
{
	gui_GPUPart.loadFromFile("settings_" + ofToString(preset) + ".xml");
	iPreset = preset;
	reset = true;

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
