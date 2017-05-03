#include "Antares.h"
#include "Font.h"


CFont::CFont()
{
	// initial color components
	r = 255;
	g = 255;
	b = 255;

	// initial space between symbols
	symbolsGap = 2;
}

///
int CFont::loadMapTxt(char fileName[80])
{
	FILE *fp;
	int i, x1, y1, x2, y2, sh;
	unsigned char c;

	if (fopen_s(&fp, fileName, "rt") != 0)
	{
		MessageBox(NULL, L"Error opening font map file!", L"Error", MB_OK);
		return 0;
	}

	for (i = 0; i < maxSymbols; i++)
	{
		if (!fscanf(fp, "%c %i %i %i %i %i\n", &c, &x1, &y1, &x2, &y2, &sh))
			break;

		symbolMap[c].left = x1;
		symbolMap[c].top = y1;
		symbolMap[c].right = x2;
		symbolMap[c].bottom = y2;
		yShift[c] = sh;
	}

	fclose(fp);

	return 1;
}

////
void CFont::setSpaceSymbol(int x1, int y1, int x2, int y2)
{
	symbolMap[0x20].left	= x1;
	symbolMap[0x20].top		= y1;
	symbolMap[0x20].right	= x2;
	symbolMap[0x20].bottom	= y2;
}

////
void CFont::loadImageBmp(char imageFileName[80], char imageAlphaFileName[80])
{
	img.loadBMP(imageFileName, imageAlphaFileName);
	img.createSprite();
}

////
void CFont::loadImageDat(char imageFileName[80])
{
	img.loadDAT(imageFileName);
    img.createSprite();
}

////
void CFont::uninit()
{
	img.deleteSprite();
}

////
void CFont::drawSymbol(int x, int y, unsigned char c)
{
	img.drawSpriteEx(x, y + yShift[c], symbolMap[c], NO_SCALE, NOT_REFLECT, r, g, b);
}

////
void CFont::drawString(int x, int y, unsigned char *s)
{
	unsigned char c;
	unsigned int i = 0;

	do{
		c = s[i];
		img.drawSpriteEx(x, y + yShift[c], symbolMap[c], NO_SCALE, NOT_REFLECT, r, g, b);

		i++;
		x += symbolMap[c].right - symbolMap[c].left + symbolsGap;
	} while (c != (unsigned char)'\n' && i < maxStringLength);

}

////
void CFont::setSymbolGap(unsigned int symbolsGap)
{
	this->symbolsGap = symbolsGap;
}

////
void CFont::setColor(unsigned r, unsigned g, unsigned b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

///
int CFont::saveRfmFile(char rfmFileName[80])
{
	FILE *fp;
	int i;
	char c;

	if (fopen_s(&fp, rfmFileName, "wb") != 0)
	{
		MessageBox(NULL, L"Can't create file for writing RFM data!", L"Error!", MB_OK);
		return 0;
	}

	for (i = 0; i < maxSymbols; i++)
	{
		c = (unsigned char)i;
		fwrite(&c, 1, 1, fp);
		fwrite(&symbolMap[i], sizeof(RECT), 1, fp);
		fwrite(&yShift[i], 1, 1, fp);
	}

	fclose(fp);
	return 1;
}

///
int CFont::loadMap(char rfmFileName[80])
{
	FILE *fp;
	int i;
	unsigned char c;

	if (fopen_s(&fp, rfmFileName, "rb") != 0)
	{
		MessageBox(NULL, L"Can't open file for writing RFM data!", L"Error!", MB_OK);
		return 0;
	}

	for (i = 0; i < maxSymbols; i++)
	{
		fread(&c, 1, 1, fp);
		fread(&symbolMap[c], sizeof(RECT), 1, fp);
		fread(&yShift[c], 1, 1, fp);
	}

	fclose(fp);
	return 1;
}