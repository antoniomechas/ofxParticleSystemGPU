#include "MultiTextura.h"

MultiTextura::MultiTextura()
{
	numTextures = 0;
	globalWidth = 0;
	globalHeight = 0;
}

void MultiTextura::reset( )
{
	numTextures = 0;
	globalWidth = 0;
	globalHeight = 0;
}

ofTexture *MultiTextura::getTexture()
{
	return ( &fboGlobal.getTextureReference() );
}

void MultiTextura::addImage( string imageName)
{
	ofImage img(imageName);
	if (img.isAllocated() && img.getWidth() > 0 && img.getHeight() > 0)
	{
		
		addTexture( &img.getTextureReference());
		
		regeneraTexturaGlobal();

	}
}

void MultiTextura::addAlphabet( )
{
	char letra[2];
	ofFbo temp;
	
	font.loadFont("verdana.ttf", 64, true, true);
	//font.setLineHeight(34.0f);
	//font.setLineHeight(84.0f);
	//font.setLetterSpacing(1.035);
	
	// Primero se mide el tamaño que tendran todas las letras dibujadas una debajo de la otra
	for (int i = 'A'; i < 'Z' ; i++)
	{
		letra[0] = i; letra[1] = '\0' ;
		ofRectangle rt = font.getStringBoundingBox(string(letra), 0, 0);
		temp.allocate( rt.getWidth(), rt.getHeight(), GL_RGBA);	
		
		temp.begin();
			ofSetColor(255,255,255);
			font.drawString(string(letra), -rt.x, -rt.y );
			//ofNoFill();
			//ofSetColor(255,0,0);
			//ofRect(1, 1, rt.width-1, rt.height-1);
		temp.end();

		cout << "Letra " << letra[0] << "x= " << rt.x << ", y= " << rt.y << ", width = " << rt.getWidth() << ", height = " << rt.getHeight() << endl;
		cout << rt;
		addTexture( &temp.getTextureReference() ); 
	}
	
	regeneraTexturaGlobal();

}

void MultiTextura::addTexture( ofTexture *tex)
{
	ofPoint pIni;
	if (numTextures == 0)
		pIni = ofPoint(0,0);
	else
		pIni = ofPoint(0, textureData[numTextures-1].posIni.y + textureData[numTextures-1].height);
	
	ofSetColor(255,255,255,255);
	
	textureData[numTextures].fboTexture.allocate(tex->getWidth(), tex->getHeight(), GL_RGBA);
	textureData[numTextures].fboTexture.begin();
		tex->draw(0,0);
	textureData[numTextures].fboTexture.end();
	textureData[numTextures].posIni = pIni;
	textureData[numTextures].height = tex->getHeight();
	textureData[numTextures].width = tex->getWidth();
	numTextures++;

	if (tex->getWidth() > globalWidth)
		globalWidth = tex->getWidth();
		
	globalHeight = globalHeight + tex->getHeight();

}

void MultiTextura::regeneraTexturaGlobal()
{

	fboGlobal.allocate(	globalWidth, globalHeight, GL_RGBA);

	ofPoint pos(0,0);
	fboGlobal.begin();
		ofClear(0,0);
		ofSetColor(255,255,255);
		for (int i = 0 ; i < numTextures ; i++)
		{
			textureData[i].fboTexture.draw(pos);
			pos.y = pos.y + textureData[i].fboTexture.getHeight();
		}
	fboGlobal.end();
}

void MultiTextura::getTextureInfo( int id, ofPoint *pos, float *width, float *height)
{
	*pos = textureData[id].posIni;
	*width = textureData[id].width;
	*height = textureData[id].height;
}

void MultiTextura::bind()
{
	fboGlobal.getTextureReference().bind();
}

void MultiTextura::unbind()
{
	fboGlobal.getTextureReference().unbind();
}
