// Antares.cpp
#include "Antares.h"


//unsigned int  GlobalSpriteNumber = 0;
unsigned int CSprite::GlobalSpriteNumber = 0;
unsigned int CFramedObject::GlobalCFramedObjectNumber = 0;

////////////////////////////////
//
//   CAntares Methods
//
////////////////////////////////

////
void CAntares::init(unsigned int width, unsigned int height, HDC hdc)
{
	HGLRC hglrc;

	hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);

	// set viewing projection 
	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();

	// ortho projection of the screen
	glOrtho(0.0F, (float)width, 0.0F, (float)height, 0.0F, 400.0F);

	glEnable(GL_LIGHT0);

	// enable texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//	glEnable(GL_MULTISAMPLE);
	//	glEnable(GL_LINE_SMOOTH); 
	//	glEnable(GL_BLEND);

}

////
void CAntares::uninit(void)
{
	// nop

}

////
void CAntares::resize(unsigned int width, unsigned int height)
{
	// set viewport to cover the window 
	glViewport(0, 0, width, height);
}

////
void CAntares::setVSync(bool enabled) // true -- VSync включен, false -- выключен
{
	PFNWGLSWAPINTERVALEXTPROC wglSwapInterval = NULL;

	wglSwapInterval = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapInterval) wglSwapInterval(enabled ? 1 : 0);
}

////
void CAntares::startRedraw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
}

////
void CAntares::stopRedraw(HDC hDC)
{
	glFlush();
	glDisable(GL_TEXTURE_2D);

	SwapBuffers(hDC);
}

////
void CAntares::setupPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	/* size */
		1,						/* version */
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,		/* support double-buffering */
		PFD_TYPE_RGBA,			/* color type */
		32,						/* prefered color depth */
		0, 0, 0, 0, 0, 0,		/* color bits (ignored) */
		0,						/* no alpha buffer */
		0,						/* alpha bits (ignored) */
		0,						/* no accumulation buffer */
		0, 0, 0, 0,				/* accum bits (ignored) */
		16,						/* depth buffer */
		0,						/* no stencil buffer */
		0,						/* no auxiliary buffers */
		PFD_MAIN_PLANE,			/* main layer */
		0,						/* reserved */
		0, 0, 0,				/* no layer, visible, damage masks */
	};
	int pixelFormat;

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (pixelFormat == 0) {
		MessageBox(WindowFromDC(hDC), L"ChoosePixelFormat failed.", L"Error",
			MB_ICONERROR | MB_OK);
		exit(1);
	}

	if (SetPixelFormat(hDC, pixelFormat, &pfd) != TRUE) {
		MessageBox(WindowFromDC(hDC), L"SetPixelFormat failed.", L"Error",
			MB_ICONERROR | MB_OK);
		exit(1);
	}
}


////////////////////////////////
//
//   CSprite Methods
//
////////////////////////////////

////
CSprite::CSprite(void)
{
//	GlobalSpriteNumber = 0;
}

////
void CSprite::createSprite(void)
{
	// Текстура 

	// Индексирование новой текстуры:
	Number = GlobalSpriteNumber;
	glBindTexture(GL_TEXTURE_2D, Number);
	GlobalSpriteNumber++;

	// Повторение текстуры по коорд. s и t если за пределы её выйдем:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Режим выбора ближайшего текселя для текстуры больше и меньше объекта
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, SideSize, SideSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_ptr);
	//							        

}

////
void CSprite::drawSprite(int x, int y, RECT r, unsigned int scale, unsigned char mir)
{
	float	x1, x2, y1, y2;
	RECT rc;

	// Проверка порядка следования границ рамки + коррекция:
	if (r.left > r.right)
	{
		rc.left = r.right; rc.right = r.left + 1;
	}
	else
	{
		rc.right = r.right + 1; rc.left = r.left;
	}

	if (r.top > r.bottom)
	{
		rc.top = r.bottom; rc.bottom = r.top + 1;
	}
	else
	{
		rc.bottom = r.bottom + 1; rc.top = r.top;
	}

	// вычисление текстурных координат:
	x1 = (float)(rc.left) / (float)SideSize;
	y2 = (float)(rc.top) / (float)SideSize;

	x2 = (float)(rc.right) / (float)SideSize;
	y1 = (float)(rc.bottom) / (float)SideSize;

	// масштабирование при выводе:
	rc.right = rc.right << scale;
	rc.bottom = rc.bottom << scale;


	// выбор текстуры:
	glBindTexture(GL_TEXTURE_2D, Number);

	// Четырехугольник:
	glBegin(GL_QUADS);

	glNormal3f(2.0F, 2.0F, 1.0F);

	if (mir == X_REFLECT)
	{
		// Зеркальное слева-направо отражение спрайта:

		glTexCoord2f(x1, 1 - y2);
		glVertex2i(x + (rc.right - rc.left), y + (rc.bottom - rc.top));

		glTexCoord2f(x2, 1 - y2);
		glVertex2i(x, y + (rc.bottom - rc.top));

		glTexCoord2f(x2, 1 - y1);
		glVertex2i(x, y);

		glTexCoord2f(x1, 1 - y1);
		glVertex2i(x + (rc.right - rc.left), y);
	}
	else
	{
		// Без отзеркаливания слева-направо:

		glTexCoord2f(x2, 1 - y2);
		glVertex2i(x + (rc.right - rc.left), y + (rc.bottom - rc.top));

		glTexCoord2f(x1, 1 - y2);
		glVertex2i(x, y + (rc.bottom - rc.top));

		glTexCoord2f(x1, 1 - y1);
		glVertex2i(x, y);

		glTexCoord2f(x2, 1 - y1);
		glVertex2i(x + (rc.right - rc.left), y);
	}

	glEnd();
}

////
void CSprite::drawSpriteEx(int x, int y, RECT r, unsigned int scale, unsigned char mir, 
						   unsigned char rColor, unsigned char gColor, unsigned char bColor)
{
	float	x1, x2, y1, y2;
	RECT rc;

	// Проверка порядка следования границ рамки + коррекция:
	if (r.left > r.right)
	{
		rc.left = r.right; rc.right = r.left + 1;
	}
	else
	{
		rc.right = r.right + 1; rc.left = r.left;
	}

	if (r.top > r.bottom)
	{
		rc.top = r.bottom; rc.bottom = r.top + 1;
	}
	else
	{
		rc.bottom = r.bottom + 1; rc.top = r.top;
	}

	// вычисление текстурных координат:
	x1 = (float)(rc.left) / (float)SideSize;
	y2 = (float)(rc.top) / (float)SideSize;

	x2 = (float)(rc.right) / (float)SideSize;
	y1 = (float)(rc.bottom) / (float)SideSize;

	// масштабирование при выводе:
	rc.right = rc.right << scale;
	rc.bottom = rc.bottom << scale;


	// выбор текстуры:
	glBindTexture(GL_TEXTURE_2D, Number);

	// Четырехугольник:
	glBegin(GL_QUADS);

	glNormal3f(2.0F, 2.0F, 1.0F);

	if (mir == X_REFLECT)
	{
		// Зеркальное слева-направо отражение спрайта:
		glColor3ub(rColor, gColor, bColor);

		glTexCoord2f(x1, 1 - y2);
		glVertex2i(x + (rc.right - rc.left), y + (rc.bottom - rc.top));

		glTexCoord2f(x2, 1 - y2);
		glVertex2i(x, y + (rc.bottom - rc.top));

		glTexCoord2f(x2, 1 - y1);
		glVertex2i(x, y);

		glTexCoord2f(x1, 1 - y1);
		glVertex2i(x + (rc.right - rc.left), y);

		glColor3ub(255, 255, 255); // set to normal color
	}
	else
	{
		// Без отзеркаливания слева-направо:
		glColor3ub(rColor, gColor, bColor);

		glTexCoord2f(x2, 1 - y2);
		glVertex2i(x + (rc.right - rc.left), y + (rc.bottom - rc.top));

		glTexCoord2f(x1, 1 - y2);
		glVertex2i(x, y + (rc.bottom - rc.top));

		glTexCoord2f(x1, 1 - y1);
		glVertex2i(x, y);

		glTexCoord2f(x2, 1 - y1);
		glVertex2i(x + (rc.right - rc.left), y);

		glColor3ub(255, 255, 255); // set to normal color
	}

	glEnd();
}

////
void CSprite::deleteSprite(void)
{
	free(img_ptr);
	img_ptr = NULL;
}

////
void CSprite::loadBMP(char name[80], char name_alpha[80])
{

	unsigned char  temp, temp1;
	unsigned char  pix[3];
	unsigned char  endline[3];
	unsigned int   width, height, ii, jj, tt;
	unsigned int   junk;
	unsigned int   x_size, y_size;

	unsigned char *header;
	FILE		  *fp;


	//////////
	if (!(fp = fopen(name, "rb")))
		MessageBox(NULL, L"File Not Found", L"Error", MB_OK);

	// Считывание заголовка в память:
	header = (unsigned char *)malloc(54 + 1);
	fread(header, 54, 1, fp);

	// Считывание ширины изображения:
	temp = header[18];
	temp1 = header[19];
	width = (unsigned int)(temp1 << 8) + temp;

	// Считывание высоты изображения:
	temp = header[22];
	temp1 = header[23];
	height = (unsigned int)(temp1 << 8) + temp;

	// Вычисление кол-ва junk-байтов:
	junk = width % 4;
	// junk = (~(n*24-1)>>3)&3; // более универсальная ф-ла


	/// Вычисление размеров буффера для получения
	/// квадратной текстуры:
	x_size = width;
	y_size = height;

	if (x_size<2 || y_size<2)
		MessageBox(NULL, L"Illegal size", L"Error", MB_OK);

	/// Коррекция размера по ширине:
	ii = 1;
	while (x_size >(unsigned int)(2 << (ii - 1)))
		ii++;

	x_size = 2 << (ii - 1);

	/// Коррекция размера по высоте:
	ii = 1;
	while (y_size > (unsigned int)(2 << (ii - 1)))
		ii++;

	y_size = 2 << (ii - 1);

	/// Максимальный размер:
	if (y_size > x_size)
		x_size = y_size;
	else
		y_size = x_size;
	///



	// Выделение памяти под массив растров:
	img_ptr = (unsigned char *)malloc(x_size*y_size * 4 + 1);

	// Заполнение этого пространства памяти 0-и:
	memset(img_ptr, 0, x_size*y_size * 4);

	fseek(fp, 54, SEEK_SET);


	for (jj = 0; jj<height; jj++)
	{
		for (ii = 0; ii<width; ii++)
		{
			// Считываем BGR байты для очередного пикселя:
			fread(pix, 3, 1, fp);

			// Смещение для текущего пикселя в буффере Image:
			tt = jj*x_size * 4 + ii * 4;

			// Меняем порядок BGR -> RGB
			img_ptr[tt + 0] = pix[2];
			img_ptr[tt + 1] = pix[1];
			img_ptr[tt + 2] = pix[0];

			// Альфа-канал устанавливается в 0xFF:
			img_ptr[tt + 3] = 0xFF;

		}


		// Считывание трех байтов - маркера конца строки:
		fread(endline, junk, 1, fp);
	}


	Width = width;
	Height = height;
	SideSize = x_size; // = y_size;

	fclose(fp);

	//////////
	if (name_alpha)
	{
		// Считывание файла с альфа-каналом:
		fp = fopen(name_alpha, "rb");


		// Считывание заголовка в память:
		//header = (unsigned char *)malloc(54+1);
		fread(header, 54, 1, fp);

		// Считывание ширины изображения:
		temp = header[18];
		temp1 = header[19];
		width = (unsigned int)(temp1 << 8) + temp;

		// Считывание высоты изображения:
		temp = header[22];
		temp1 = header[23];
		height = (unsigned int)(temp1 << 8) + temp;

		//
		// ... Добавить проверку на совпадение размеров

		// Вычисление кол-ва junk-байтов:
		junk = width % 4;
		// junk = (~(n*24-1)>>3)&3; // более универсальная ф-ла

		// Выделение памяти под массив растров:
		//Image->img_ptr = (unsigned char *)malloc(width*height*4+1);

		fseek(fp, 54, SEEK_SET);


		for (jj = 0; jj<height; jj++)
		{
			for (ii = 0; ii<width; ii++)
			{
				// Считываем BGR байты для очередного пикселя:
				fread(pix, 3, 1, fp);

				// Смещение для текущего пикселя в буффере Image:
				tt = jj*x_size * 4 + ii * 4;

				// Альфа-канал устанавливается в 0xFF:
				img_ptr[tt + 3] = pix[0];
			}

			// Считывание трех байтов - маркера конца строки:
			fread(endline, junk, 1, fp);
		}
	}

	free(header);


	// Перенос пикселей внутри буффера:
	for (ii = 0; ii<Height; ii++)
		memcpy(&img_ptr[SideSize*(SideSize - ii - 1) * 4],
		&img_ptr[SideSize*(Height - 1 - ii) * 4],
		Width * 4);

	// Заполнение области, откуда был перенос 0-и 
	for (ii = 1; ii<(SideSize - Height + 1); ii++)
		memset(&img_ptr[SideSize*(SideSize - Height - ii) * 4], 0, Width * 4);
}

//// Считывание файлов собственного формата (с альфа-каналом)
void CSprite::loadDAT(char name[80])
{
	unsigned int ii, jj;
	unsigned int x_size, y_size;
	FILE *fp;

	fp = fopen(name, "rb");

	fread(&Width, 2, 1, fp);
	fread(&Height, 2, 1, fp);


	/// Вычисление размеров буффера для получения
	/// квадратной текстуры:
	x_size = Width;
	y_size = Height;

	if (x_size<2 || y_size<2)
		MessageBox(NULL, L"Illegal size", L"Error", MB_OK);

	/// Коррекция размера по ширине:
	ii = 1;
	while (x_size >(unsigned int)(2 << (ii - 1)))
		ii++;

	x_size = 2 << (ii - 1);

	/// Коррекция размера по высоте:
	ii = 1;
	while (y_size > (unsigned int)(2 << (ii - 1)))
		ii++;

	y_size = 2 << (ii - 1);

	/// Максимальный размер:
	if (y_size > x_size)
		x_size = y_size;
	else
		y_size = x_size;
	///

	SideSize = x_size;


	if (!(img_ptr = (unsigned char *)malloc(x_size*y_size * 4 + 1)))
		MessageBox(NULL, L"Out of Memory!", L"Error", MB_OK);

	memset(img_ptr, 0, x_size*y_size * 4);

	for (jj = 0; jj < Height; jj++)
	{
		for (ii = 0; ii < Width; ii++)
		{
			fread(&img_ptr[jj*x_size * 4 + ii * 4], 4, 1, fp);
		}
	}


	// Перенос пикселей внутри буффера:
	for (ii = 0; ii < Height; ii++)
		memcpy(&img_ptr[SideSize*(SideSize - ii - 1) * 4],
		&img_ptr[SideSize*(Height - 1 - ii) * 4],
		Width * 4);

	// Заполнение области, откуда был перенос 0-и 
	for (ii = 1; ii<(SideSize - Height + 1); ii++)
		memset(&img_ptr[SideSize*(SideSize - Height - ii) * 4], 0, Width * 4);


	fclose(fp);
}

////
void CAntares::setRedrawColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	// clear color and depth buffers 
	glClearColor(r, g, b, a);
}


/////////////////////////////////
//	Methods of CFramedObject:  //
/////////////////////////////////

CFramedObject::CFramedObject()
{
	GlobalCFramedObjectNumber++;
}

////
void CFramedObject::initFrames(unsigned int frames)
{
	if (!(frame = (RECT *)malloc(frames*sizeof(RECT))))
		MessageBox(NULL, L"Can't allocate memory for next frame", L"Error!", MB_OK);

	memset(frame, 0, frames*sizeof(RECT));
	frameCount = frames;
}

////
void CFramedObject::uninitFrames()
{
	free(frame);
	frame = NULL;
	frameCount = 0;
}

///
int CFramedObject::setFrame(unsigned int n, unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2)
{
	if (n >= frameCount)
	{
		MessageBox(NULL, L"Can't init frame", L"Error!", MB_OK);
		return -1;
	}


	// sort by Y
	if (y1 > y2)
	{
		frame[n].top = y2;
		frame[n].bottom = y1;
		height = y1 - y2 + 1;
	}
	else
	{
		frame[n].top = y1;
		frame[n].bottom = y2;
		height = y2 - y1 + 1;
	}
	// sort by X
	if (x1 > x2)
	{
		frame[n].right = x1;
		frame[n].left = x2;
		width = x1 - x2 + 1;
	}
	else
	{
		frame[n].right = x2;
		frame[n].left = x1;
		width = x2 - x1 + 1;
	}

	return 0;
}

////
RECT CFramedObject::getFrame(unsigned int n)
{
	if (n >= frameCount)
		MessageBox(NULL, L"No frame with this number", L"Error!", MB_OK);

	return frame[n];
}

////
unsigned int CFramedObject::getMaxFrames()
{
	return frameCount;
}

////
unsigned int CFramedObject::getWidth()
{
	return width;
}

////
unsigned int CFramedObject::getHeight()
{
	return height;
}


