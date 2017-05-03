// Antares.h

#include <Windows.h>
#include <GL/gl.h> 
#include <stdio.h>


enum {
	NO_SCALE = 0,	// must = 0
	SCALE_X2,		// must = 1
	SCALE_X4,		// must = 2
	X_REFLECT,
	NOT_REFLECT,
};
 

typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);


class CSprite{

	private:

		unsigned char *img_ptr;
		unsigned int  Width;
		unsigned int  Height;
		unsigned int  Number;
		unsigned int  SideSize;
		static unsigned int  GlobalSpriteNumber;

	public:

		void createSprite(void);
		void drawSprite(int x, int y, RECT r, unsigned int scale, unsigned char mir);
		void drawSpriteEx(int x, int y, RECT r, unsigned int scale, unsigned char mir, 
						  unsigned char rColor, unsigned char gColor, unsigned char bColor);
		void deleteSprite(void);
		void loadBMP(char name[80], char name_alpha[80]);
		void loadDAT(char name[80]);
		CSprite(void);
};


class CFramedObject
{
	private:

		static unsigned int GlobalCFramedObjectNumber;
		unsigned int frameCount;
		unsigned int width;
		unsigned int height;
		RECT *frame;

	public:

		CFramedObject();
		void initFrames(unsigned int frames);
		void uninitFrames();
		int  setFrame(unsigned int n, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2);
		RECT getFrame(unsigned int n);
		unsigned int getWidth();
		unsigned int getHeight();
		unsigned int getMaxFrames();
};


class CAntares{

	public:

		void init(unsigned int width, unsigned int height, HDC hdc);
		void resize(unsigned int width, unsigned int height);
		void setVSync(bool enabled);

		void startRedraw(void);
		void stopRedraw(HDC hDC);
		void setRedrawColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

		void setupPixelFormat(HDC hDC);
		void uninit(void);
};

