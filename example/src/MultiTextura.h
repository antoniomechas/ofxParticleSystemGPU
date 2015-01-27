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
