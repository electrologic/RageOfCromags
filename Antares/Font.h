class CFont
{
private:

	static const int maxStringLength = 256;
	static const int maxSymbols = 256;

	RECT symbolMap[maxSymbols];
	char yShift[maxSymbols];
	CSprite img;
	unsigned int symbolsGap;
	unsigned char r, g, b;

public:

	CFont();
	int loadMapTxt(char fileName[80]);
	int loadMap(char fileName[80]);
	int saveRfmFile(char rfmFileName[80]);
	void setSpaceSymbol(int x1, int y1, int x2, int y2);
	void loadImageBmp(char imageFileName[80], char imageAlphaFileName[80]);
	void loadImageDat(char imageFileName[80]);
	void uninit();
	void drawSymbol(int x, int y, unsigned char c);
	void drawString(int x, int y, unsigned char *s);
	void setSymbolGap(unsigned int symbolsGap);
	void setColor(unsigned r, unsigned g, unsigned b);
	
};