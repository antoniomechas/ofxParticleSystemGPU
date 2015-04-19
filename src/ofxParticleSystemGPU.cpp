/*
 *  ofxParticleSystemGPU.cpp
 *
 *  Antoniomechas - 2015, http://antoniomechas.net 
 *	forked from the original source code found here: https://github.com/neilmendoza/ofxGpuParticles, by Neil Mendoza
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */

#include "ofxParticleSystemGPU.h"

const string ofxParticleSystemGPU::UNIFORM_PREFIX = "particles";
//const string GpuParticles::UPDATE_SHADER_NAME = "shaders/GPUParticles/update";
//const string GpuParticles::DRAW_SHADER_NAME = "shaders/GPUParticles/draw";
    
ofxParticleSystemGPU::ofxParticleSystemGPU() : currentReadFbo(0), textureLocation(0)
{
		bMultiTextureSet = false;
}
    
void ofxParticleSystemGPU::init(unsigned width, unsigned height, int screenW, int screenH)
{
    this->width = width;
    this->height = height;
    numFloats = width * height * FLOATS_PER_TEXEL;
    screenWidth = screenW;
	screenHeight = screenH;

    // fbos
    ofFbo::Settings s;
    s.internalformat = GL_RGBA32F_ARB;
    s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    s.minFilter = GL_NEAREST;
    s.maxFilter = GL_NEAREST;
    s.wrapModeHorizontal = GL_CLAMP;
    s.wrapModeVertical = GL_CLAMP;
    s.width = width;
    s.height = height;
    //s.numColorbuffers = numDataTextures;
    s.numColorbuffers = DataTextureIndex::LAST_INDEX;
    for (unsigned i = 0; i < 2; ++i)
    {
		fbos[i].allocate(s);
    }
        
	texStaticColor.allocate(screenW, screenH, GL_RGBA32F_ARB);
	texVectorField.allocate(screenW, screenH, GL_RGBA32F_ARB);
	texOpticalFlow.allocate(screenW, screenH, GL_RGBA32F_ARB);
	texMask.allocate(screenW, screenH, GL_RGBA32F_ARB);
	zeroDynamicTexture(DynamicTextures::COLOR_STATIC);
	zeroDynamicTexture(DynamicTextures::VECTOR_FIELD);
	zeroDynamicTexture(DynamicTextures::OPTICAL_FLOW);
	
	//renderFBO.allocate(s);

    // mesh
    mesh.clear();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            //mesh.addVertex(ofVec3f(200.f * x / (float)width - 100.f, 200.f * y / (float)height - 100.f, -500.f));
            mesh.addVertex( ofVec3f(x , y , 0));
            mesh.addTexCoord(ofVec2f(x, y));
        }
    }
    mesh.setMode(OF_PRIMITIVE_POINTS);
        
	//Solo carga los shaders una vez
	if (!updateShader.isLoaded())
    {
		updateShader.load("shaders/GPUParticles/update");
			
		// Frag, Vert and Geo shaders for the rendering process 
		renderShader.setGeometryInputType(GL_POINTS);
		renderShader.setGeometryOutputType(GL_TRIANGLE_STRIP);
		renderShader.setGeometryOutputCount(4);
		renderShader.load("shaders/GPUParticles/render.vert","shaders/GPUParticles/render.frag","shaders/GPUParticles/render.geom");

		// Loading and setings of the variables of the textures of the particles
		sparkImg.loadImage("images/spark.png");
		//sparkImg.loadImage("images/logo_alpha.png");

		imgWidth = sparkImg.getWidth();
		imgHeight = sparkImg.getHeight();

		////Parámetros
		particleSizeMin = 2.0f;
		colorStatic = false;
		vectorField = 10;				//  
		useParticleTexture = true;		// Si la particula se dibuja utilizando textura o no 
		useEnlargeByVelocity = true;	// La particula se alarga segun la velocidad si > 0
		enlargeByVelocity = 0.3f;		// La particula se alarga segun la velocidad si > 0
		useVelocidadColor = true;		// La particula tiene un color mas brillante con la velocidad
		velocidadColor = 300;			// será un divisor de la velocidad
		radiusSquared = 300;			// El atractor
		gravity = ofVec2f(0,0);			// gravedad
		backToOrigin = 0;
		useSizeByVelocity = false;
		sizeByVelocity = 0.0f;
		useVectorField = false;
		useOpticalFlow = true;
		useMultiTex = false;
	}


}
    
void ofxParticleSystemGPU::update()
{


	
	fbos[1 - currentReadFbo].begin(false);
		glPushAttrib(GL_ENABLE_BIT);
		// we set up no camera model and ignore the modelview and projection matrices
		// in the vertex shader, we make a viewport large enought to ensure the shader
		// is executed for each pixel
		glViewport(0, 0, width, height);
		glDisable(GL_BLEND);
		ofSetColor(255, 255, 255);
		fbos[1 - currentReadFbo].activateAllDrawBuffers();
        
		updateShader.begin();
			setUniforms(updateShader);
			//updater
			int texIndex = fbos[currentReadFbo].getNumTextures();
			//updateShader.setUniformTexture("texStaticColor", texStaticColor , texIndex++);
			if (useVectorField)
				updateShader.setUniformTexture("texVectorField", texVectorField , texIndex++);
			//if (useOpticalFlow)
			//	updateShader.setUniformTexture("texOpticalFlow", texOpticalFlow , texIndex++);
			updateShader.setUniform1f("minVelocidad", minVelocidad); 
			updateShader.setUniform1f("damping", damping); 
			float vField = 0.0f;
			if (useVectorField) vField = vectorField;
			updateShader.setUniform1f("vectorFieldScalar", vField); 
			float oFlow = 0;
			if (useOpticalFlow) oFlow = opticalFlow;
			updateShader.setUniform1f("opticalFlowScalar", oFlow); 
			updateShader.setUniform1f("radiusSquared", radiusSquared * radiusSquared);
			updateShader.setUniform3f("gravity", gravity->x, gravity->y, 0);
			updateShader.setUniform1f("backToOrigin", (float)backToOrigin);
			ofVec3f mouse(ofGetMouseX(), ofGetMouseY() , 0.f);
			updateShader.setUniform3fv("mouse", mouse.getPtr());
			updateShader.setUniform1f("elapsed", ofGetLastFrameTime());

			texturedQuad(-1, -1, 2, 2, width, height);
		updateShader.end();

		glPopAttrib();
        
    fbos[1 - currentReadFbo].end();
        
    currentReadFbo = 1 - currentReadFbo;
}
    
void ofxParticleSystemGPU::draw()
{

	//ofBackground(0);
//  
	//ofSetColor(100,255,255);
	//renderFBO.draw(0,0);
//  
	//ofSetColor(255);
	//ofDrawBitmapString("Fps: " + ofToString( ofGetFrameRate()), 15,15);

	//checkUpdateShader();

	ofDisableDepthTest();
	renderShader.begin();
		setUniforms(renderShader);

		// renderer
		int texIndex = fbos[currentReadFbo].getNumTextures();
			
		renderShader.setUniformTexture("sparkTex", sparkImg.getTextureReference() , texIndex++ );
		renderShader.setUniformTexture("texStaticColor", texStaticColor , texIndex++);
		//renderShader.setUniformTexture("texVectorField", texVectorField , texIndex++);

		if (useMultiTex && bMultiTextureSet)
			renderShader.setUniformTexture("multiTex", *texMultiTexture , texIndex++ );

		renderShader.setUniform1f("enlargeByVelocity", (useEnlargeByVelocity == true ? (float)enlargeByVelocity : 0.0f) ); 
		renderShader.setUniform1f("sizeByVelocity", (useSizeByVelocity == true ? (float)sizeByVelocity : 0.0f) ); 
		renderShader.setUniform1f("useParticleTexture", useParticleTexture); 
		renderShader.setUniform1f("blackWhenStill", blackWhenStill); 
		renderShader.setUniform1f("minVelocidad", minVelocidad); 
		renderShader.setUniform1f("useVida", useVida); 
		renderShader.setUniform1f("useMaxDst", useMaxDst); 
		renderShader.setUniform1i("useMultiTexture", (useMultiTex == true && bMultiTextureSet == true ? 1 : 0)); 
			
		renderShader.setUniform1f("factorVelocidadColor", (useVelocidadColor == true ? (float)velocidadColor : 0.0f)); 

		renderShader.setUniform1f("colorStatic", (float)colorStatic); 
		renderShader.setUniform1f("originStatic", originStatic ); 

		renderShader.setUniform1f("imgWidth", (float)sparkImg.getWidth());
		renderShader.setUniform1f("imgHeight", (float)sparkImg.getHeight());

		ofEnableBlendMode( OF_BLENDMODE_ALPHA );
		ofSetColor(255);

		mesh.draw();
    
		ofDisableBlendMode();

	renderShader.end();
		
}
    
void ofxParticleSystemGPU::setUniforms(ofShader& shader)
{
    for (unsigned i = 0; i < fbos[currentReadFbo].getNumTextures(); ++i)
    {
        ostringstream oss;
        oss << UNIFORM_PREFIX << ofToString(i);
        shader.setUniformTexture(oss.str().c_str(), fbos[currentReadFbo].getTextureReference(i), i + textureLocation);
    }
		
}
    
void ofxParticleSystemGPU::loadDataTexture(unsigned idx, float* data,
                                    unsigned x, unsigned y, unsigned width, unsigned height)
{
    if (idx < fbos[currentReadFbo].getNumTextures())
    {
        if (!width) width = this->width;
        if (!height) height = this->height;
        fbos[currentReadFbo].getTextureReference(idx).bind();
        glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, x, y, width, height, GL_RGBA, GL_FLOAT, data);
        fbos[currentReadFbo].getTextureReference(idx).unbind();
	}
    else ofLogError() << "Trying to load data from array into non-existent buffer.";
}
    
void ofxParticleSystemGPU::zeroDataTexture(unsigned idx,
                                    unsigned x, unsigned y, unsigned width, unsigned height)
{
    if (!width) width = this->width;
    if (!height) height = this->height;
    float* zeroes = new float[width * height * FLOATS_PER_TEXEL];
    memset(zeroes, 0, sizeof(float) * width * height * FLOATS_PER_TEXEL);
    loadDataTexture(idx, zeroes, x, y, width, height);
    delete[] zeroes;
}

   
void ofxParticleSystemGPU::texturedQuad(float x, float y, float width, float height, float s, float t)
{
    // TODO: change to triangle fan/strip
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x, y, 0);
        
    glTexCoord2f(s, 0);
    glVertex3f(x + width, y, 0);
        
    glTexCoord2f(s, t);
    glVertex3f(x + width, y + height, 0);
        
    glTexCoord2f(0, t);
    glVertex3f(x, y + height, 0);
    glEnd();

}
    
void ofxParticleSystemGPU::save(const string& fileName)
{
    ofstream fileStream(ofToDataPath(fileName, true).c_str());
    if (fileStream.is_open())
    {
        for (unsigned i = 0; i < fbos[currentReadFbo].getNumTextures(); ++i)
        {
            if (i) fileStream << "|";
            ofFloatPixels pixels;
            fbos[currentReadFbo].getTextureReference(i).readToPixels(pixels);
            for (unsigned j = 0; j < pixels.size(); ++j)
            {
                if (j) fileStream << ",";
                fileStream << pixels[j];
            }
        }
        fileStream.close();
    }
    else ofLogError() << "Could not save particle data to " << ofToDataPath(fileName, true);
}
    
void ofxParticleSystemGPU::load(const string& fileName)
{
    ifstream fileStream(ofToDataPath(fileName, true).c_str());
    if (fileStream.is_open())
    {
        string data((istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
        vector<string> textureData = ofSplitString(data, "|");
        for (unsigned i = 0; i < textureData.size(); ++i)
        {
            if (i < fbos[currentReadFbo].getNumTextures())
            {
                vector<string> floatsAsText = ofSplitString(textureData[i], ",");
                vector<float> floats(floatsAsText.size(), 0);
                for (unsigned j = 0; j < floats.size(); ++j)
                {
                    floats[j] = atof(floatsAsText[j].c_str());
                }
                loadDataTexture(i, &floats[0]);
            }
            else ofLogError() << "Trying to load data from file into non-existent buffer.";
        }
        fileStream.close();
    }
    else ofLogError() << "Could not load particle data from " << ofToDataPath(fileName, true);
}

void ofxParticleSystemGPU::loadIntoTexture( int texIndex, ofTexture *texture)
{
	switch (texIndex)
	{

		case DynamicTextures::COLOR_STATIC:
			texStaticColor = *texture;
			break;
			
		case DynamicTextures::VECTOR_FIELD:
			texVectorField = *texture;
			break;

		case DynamicTextures::OPTICAL_FLOW:
			texOpticalFlow = *texture;
			break;
		
		case DynamicTextures::MASK:
			texMask = *texture;
			break;
	}
}

void ofxParticleSystemGPU::loadIntoTexture( int texIndex, float* data, unsigned width, unsigned height)
{
	        
	texVectorField.bind();
    glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, data);
    texVectorField.unbind();

}

void ofxParticleSystemGPU::zeroDynamicTexture	( int texIndex )
{
	int w = texOpticalFlow.getWidth();
	int h = texOpticalFlow.getHeight();
	float* zeroes = new float[w * h * FLOATS_PER_TEXEL];
    memset(zeroes, 0, sizeof(float) * w * h * FLOATS_PER_TEXEL);
	
	ofTexture *fbo;
	switch (texIndex)
	{

		case DynamicTextures::COLOR_STATIC:
			fbo = &texStaticColor;
			break;
			
		case DynamicTextures::VECTOR_FIELD:
			fbo = &texVectorField;
			break;

		case DynamicTextures::OPTICAL_FLOW:
			fbo = &texOpticalFlow;
			break;

	}

	fbo->bind();
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, w, h, GL_RGBA, GL_FLOAT, zeroes);
	fbo->unbind();

	delete[] zeroes;

}

void ofxParticleSystemGPU::setMultiTexture ( ofTexture *tex)
{
	        
	texMultiTexture = tex;
	bMultiTextureSet =true;

}




