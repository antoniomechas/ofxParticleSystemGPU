/*
 *  MultiTextura.h
 *
 *  Antoniomechas - 2015, http://antoniomechas.net 
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

class MultiTextura
{

	public:
		
		MultiTextura();

		ofTexture	*getTexture		( );
		void		addImage		( string strImage );
		void		getTextureInfo	( int id, ofPoint *pos, float *width, float *height);
		int			getNumImages	( );
		void		bind			( );
		void		unbind			( );
		void		reset			( );
		void		addAlphabet		( );

	private:
	
		void		regeneraTexturaGlobal	( );
		void		addTexture				( ofTexture *tex );

		ofFbo		fboGlobal;	//Guarda todas las texturas en una sola textura

		struct TEXTURE_DATA
		{
			ofFbo		fboTexture;
			ofPoint		posIni;
			int			width;
			int			height;
		};

		TEXTURE_DATA			textureData[50];	// guarda las posiciones y tamaño de cada textura en la textura global
		int						numTextures;

		int						globalWidth;	// Tamaño de la textura global
		int						globalHeight;	// Tamaño de la textura global

		ofTrueTypeFont			font;

};
