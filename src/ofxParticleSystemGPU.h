/*
 *  ofxParticleSystemGPU.h
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
#pragma once

#include "ofMain.h"
#include "ofxGui.h"

/**
    * For now, only uses RGBA and TEXTURE_RECTANGLE to make usage simpler
*/
class ofxParticleSystemGPU
{
public:
    static const string UNIFORM_PREFIX;
    static const unsigned FLOATS_PER_TEXEL = 4;
        
    // you don't have to use these but makes
    // code more readable
    enum DataTextureIndex
    {
        POSITION,
        VELOCITY,
		COLOR,
		POS_INI,
		TEXTURE_COORDS,
		SIZE_LIFE_MAXDST,
		LAST_INDEX
    };

	enum DynamicTextures
    {
        COLOR_STATIC,
        VECTOR_FIELD,
        OPTICAL_FLOW
    };
        
    ofxParticleSystemGPU();
        
    void		init				( unsigned width, unsigned height, int screenW , int screenH );

	void		update				( );
    void		draw				( );
        
    void		loadDataTexture		( unsigned idx, float* data, unsigned x = 0, unsigned y = 0, unsigned width = 0, unsigned height = 0 );
    void		zeroDataTexture		( unsigned idx, unsigned x = 0, unsigned y = 0, unsigned width = 0, unsigned height = 0 );
        
    unsigned	getWidth			( ) const { return width; }
    unsigned	getHeight			( ) const { return height; }
    unsigned	getNumFloats		( ) const { return numFloats; }
        
    void		setTextureLocation	( unsigned textureLocation ) { this->textureLocation = textureLocation; }
        
    ofVboMesh&	getMeshRef			( ) { return mesh; }
        
    // advanced
    ofShader&	getUpdateShaderRef	( ) { return updateShader; }
    ofShader&	getDrawShaderRef	( ) { return drawShader; }
        
    void		save				( const string& fileName );
    void		load				( const string& fileName );
        
	void		loadIntoTexture		( DynamicTextures texIndex, ofTexture *texture );
	void		loadIntoTexture		( DynamicTextures texIndex, float* data, unsigned x, unsigned y );
	void		zeroDynamicTexture	( DynamicTextures texIndex );

	void		setMultiTexture		( ofTexture *tex );

	// Parámetros

	ofxFloatSlider		minVelocidad;			// si la velocidad es menor que esta y esta cerca de su posición inicial, la coloca en la posición inicial y la para
	ofxFloatSlider		damping;			// las particulas vuelven al origen con cierta intensidad
	ofxColorSlider		particleColor1;			// color de las partículas
	ofxColorSlider		particleColor2;			// color de las partículas
	ofxToggle			useOpticalFlow;			//  
	ofxFloatSlider		opticalFlow;			//  
	ofxToggle			useVectorField;			//  
	ofxFloatSlider		vectorField;			//  
	ofxToggle			useParticleTexture;		// Si la particula se dibuja utilizando textura o no 
	ofxToggle			useEnlargeByVelocity;	// La particula se alarga segun la velocidad si > 0
	ofxFloatSlider		enlargeByVelocity;		// La particula se alarga segun la velocidad si > 0
	ofxToggle			useSizeByVelocity;		// La particula aumenta de tamaño segun la velocidad si > 0
	ofxFloatSlider		sizeByVelocity;			// La particula aumenta de tamaño segun la velocidad si > 0
	ofxToggle			colorStatic;			// el color lo asigna desde una textura, el color de la particula sera el color de su posición en la textura
	ofxToggle			useVelocidadColor;		// La particula tiene un color mas brillante con la velocidad
	ofxFloatSlider		velocidadColor;			// será un divisor de la velocidad
	ofxFloatSlider		radiusSquared;			// El atractor
	ofxVec2Slider		gravity;				// gravedad
	ofxFloatSlider		backToOrigin;			// las particulas vuelven al origen con cierta intensidad
	ofxToggle			originStatic;			// las particulas se dibujan desde el origen hasta donde estén
	ofxToggle			blackWhenStill;			// las particulas se apagan cuando no tienen velocidad
	ofxFloatSlider		particleSizeMin;		// Tamaño de la partícula min
	ofxFloatSlider		particleSizeMax;		// Tamaño de la partícula max
	ofxToggle			useVida;				// Tiene en cuenta la vida de las partículas
	ofxFloatSlider		vidaMin;				// ticks de vida minimos
	ofxFloatSlider		vidaMax;				// ticks de vida maximos
	ofxToggle			useMaxDst;				// Tiene en cuenta la distancia máxima de la particula desde su origen
	ofxFloatSlider		maxDstMin;				// maxima distancia desde el punto inicial MIN
	ofxFloatSlider		maxDstMax;				// maxima distancia desde el punto inicial MAX
	ofxIntSlider		wind;					// 0  =  nada, 1 = viento, 2 = lluvia
	ofxToggle			useMultiTex;			// Utiliza multitextura

private:
        
	void					texturedQuad	( float x, float y, float width, float height, float s, float t );
    void					setUniforms		( ofShader& shader );
        
    ofFbo					fbos[2];

    ofVboMesh				mesh;
    ofShader				updateShader, drawShader;
		
	ofTexture				texStaticColor;		// textura del tamaño de la pantalla, que indica el color de cada pixel
	ofTexture				texVectorField;		// textura del tamaño de la pantalla, indica vector field en cada pixel (r,g; b = 0);
	ofTexture				texOpticalFlow;		// textura del tamaño de la pantalla, indica vector field en cada pixel (r,g; b = 0);

	ofTexture				*texMultiTexture;	// textura para la multitextura
	bool					bMultiTextureSet;	// si no está a true, no se puede utilizar multitextura

	ofShader				renderShader;

    unsigned				currentReadFbo;
    unsigned				textureLocation;
    unsigned				width, height, numFloats;

	int						screenWidth, screenHeight;

	//renderShader
	ofImage					sparkImg;
	int						imgWidth, imgHeight;
	int						textureRes;

};

