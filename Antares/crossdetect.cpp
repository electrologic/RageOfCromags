#include <stdio.h>
#include <Windows.h>
#include "crossdetect.h"

extern bool allowDump;
FILE *fp;

////
void CCrossMask::load(char name[80])
{
	FILE *fp;

	if (fopen_s(&fp, name, "rb") != 0)
		MessageBox(NULL, L"Can't find mask file!", L"Error!", MB_OK);

	nStringBytes = 0; nStrings = 0;
	fread(&nStringBytes, 2, 1, fp);
	fread(&nStrings, 2, 1, fp);

	pData = (unsigned char *)malloc(nStrings * nStringBytes); // need to check?

	fread(pData, nStrings * nStringBytes, 1, fp);

	fclose(fp);
}

////
void CCrossMask::deleteMask(void)
{
	free(pData);
	pData = NULL;
}

////////////
unsigned int CCrossMask::getStringsNumber()
{
	return nStrings;
}

////////////
unsigned int CCrossMask::getStringBytesNumber()
{
	return nStringBytes;
}

////
void CCrossMask::setRect(RECT rc)
{
	r = rc;
}

////
RECT CCrossMask::getRect()
{
	return r;
}

//////////////
unsigned char* CCrossMask::getPtr()
{
	return pData;
}

////
void sortRect(RECT *rc)
{
	RECT outRect = *rc;

	if (rc->left > rc->right)
	{
		outRect.left = rc->right;
		outRect.right = rc->left;
	}

	if (rc->top > rc->bottom)
	{
		outRect.bottom = rc->top;
		outRect.top = rc->bottom;
	}

	*rc = outRect;
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////
bool crossDetect(int x1, int y1, CCrossMask m1, int x2, int y2, CCrossMask m2)
{
	RECT r1, r2;

	// check - need to sort by x? 
	if (x1 > x2)
	{
		CCrossMask tempMask = m1;
		int tempCoord;

		tempCoord = x1;
		x1 = x2;
		x2 = tempCoord;

		tempCoord = y1;
		y1 = y2;
		y2 = tempCoord;

		m1 = m2;
		m2 = tempMask;
	}

	r1 = m1.getRect();
	r2 = m2.getRect();

	// correct rectangles if it needs
	sortRect(&r1);
	sortRect(&r2);

	// ?
	// (*) need to sort rectangles ??
	// right must be > left, bottom must be > top
	// ? Maybe do this sorting in a body of CCrossMask::setRect() ???

	// checking rectangles crossing
	int width1, height1, width2, height2;

	width1 = r1.right - r1.left + 1;
	height1 = r1.bottom - r1.top + 1;

	width2 = r2.right - r2.left + 1;
	height2 = r2.bottom - r2.top + 1;

	// if rectangles dont cross => masks dont cross too
	if (x2 >= (x1 + width1)) return false;
	if (x1 >= (x2 + width2)) return false;
	if (y2 >= (y1 + height1)) return false;
	if (y1 >= (y2 + height2)) return false;

	// next (if rectangles have crossed)...

	int n1, n2; // bytes number of s1 & s2 for checking
	int x01, x02; // corrected starting point
	int  i, j; // iteration variables
	int b0; // first byte of s1 for check
	unsigned char firstMask1, lastMask1; // mask for first & last bytes of s1
	unsigned char firstMask2, lastMask2; // mask for first & last bytes of s2
	unsigned char part1, part2; // parts of s2 bytes
	unsigned char shift; // shift bytes s1 relate bytes s2
	unsigned char temp1, temp2;
	unsigned char *s1, *s2;

	n1 = (int)(r1.right >> 3) - (int)(r1.left >> 3) + 1; // amount of s1 bytes
	n2 = (int)(r2.right >> 3) - (int)(r2.left >> 3) + 1; // amount of s2 bytes

	firstMask1 = (unsigned char)(0xFF >> (r1.left & 7)); // N % M = N AND (M-1)
	lastMask1 = (unsigned char)(0xFF << (7 - (r1.right & 7)));

	firstMask2 = (unsigned char)(0xFF >> (r2.left & 7));
	lastMask2 = (unsigned char)(0xFF << (7 - (r2.right & 7)));

	x01 = x1 - (r1.left & 7); // N % M = N AND (M-1)
	x02 = x2 - (r2.left & 7); // 
	shift = (int)((x02 - x01) & 7); // bits shift between bytes of different strings

	////

	int nStringsToCheck, firstStr1, firstStr2;

	// calculate amount of strings to check (4 cases)
	if (y2 >= y1)
	{
		if ((y2 + height2) >= (y1 + height1))
			nStringsToCheck = height1 - (y2 - y1);	// #1
		else
			nStringsToCheck = height2;				// #2


		firstStr1 = y2 - y1 + m1.getStringsNumber() - (height1 + r1.top);  
		firstStr2 = m2.getStringsNumber() - (height2 + r2.top);  
	} 
	else
	{
		if ((y1 + height1) > (y2 + height2))
			nStringsToCheck = height2 - (y1 - y2);	// #3
		else
			nStringsToCheck = height1;				// #4


		firstStr1 = m1.getStringsNumber() - (height1 + r1.top); 
		firstStr2 = y1 - y2 + m2.getStringsNumber() - (height2 + r2.top);
	}

	b0 = (int)((x02 - x01) >> 3); // first compared byte of s1

	j = 0;
	for (j = 0; j < nStringsToCheck; j++)
	{

		s1 = m1.getPtr(); // get str1 pointer
		s2 = m2.getPtr(); // get str2 pointer

		// set string starting position
		s1 = &s1[(firstStr1 + j) * m1.getStringBytesNumber() + (int)(r1.left >> 3)]; 
		s2 = &s2[(firstStr2 + j)* m2.getStringBytesNumber() + (int)(r2.left >> 3)]; 

		part1 = 0;

		i = 0; // iteration variable

		do{
			// checking: need to use first/last/none mask for bytes s2
			if (i == 0) temp2 = firstMask2;
			else
			{
				if (i == n2 - 1) temp2 = lastMask2;
				else temp2 = 0xFF;
			}

			// checking: need to use first/last/none mask for bytes s1
			if ((b0 + i) == 0) temp1 = firstMask1;
			else
			{
				if ((b0 + i) == n1 - 1) temp1 = lastMask1;
				else temp1 = 0xFF;
			}

			part2 = (s2[i] & temp2) >> shift; 

			if (s1[b0 + i] & (part1 | part2))
				return true; // exit routine if there is crossing

			part1 = (s2[i] & temp2) << (8 - shift);

			i++;

		} while ((i < n2) && (b0 + i < n1)); 
	}

	return false;
}
