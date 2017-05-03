
class CCrossMask
{
private:

	RECT r;
	unsigned int nStringBytes;
	unsigned int nStrings;
	unsigned char *pData;

public:

	void load(char name[80]);
	void deleteMask(void);
	void setRect(RECT rc);
	RECT getRect();
	// temporary in public area (needs for ext func - crossDetect())
	unsigned int getStringBytesNumber();
	unsigned int getStringsNumber();
	unsigned char* getPtr();
};

bool crossDetect(int x1, int y1, CCrossMask m1, int x2, int y2, CCrossMask m2);