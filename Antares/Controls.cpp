#include "Controls.h"

////
void CMouse::setCoordinate(int mouseX, int mouseY)
{
	x = mouseX;
	y = mouseY;
}

///
int CMouse::getX(void)
{
	return x;
}

///
int CMouse::getY(void)
{
	return y;
}

////
void CMouse::setLeftButtonPressed(bool state)
{
	leftButtonState = state;
}

////
bool CMouse::getLeftButtonPressed(void)
{
	return leftButtonState;
}

////////////////////////////////////////

////
bool CKeys::isPressed(unsigned int n)
{
	if (n < MAX_USED_KEYS)
		return keyArray[n];
	else
		return false;
}

////
void CKeys::setPressed(unsigned int n, bool isPressed)
{
	if (n < MAX_USED_KEYS)
		keyArray[n] = isPressed;
}

////
void CKeys::initKeyArray()
{
	for (int i = 0; i < MAX_USED_KEYS; i++)
		keyArray[i] = false;
}

////////////////////////////////////////

////
CExitControl::CExitControl()
{
    onExit = false;
    onPause = false;
}

////
void CExitControl::setState(bool onExit)
{this->onExit = onExit;}

////
bool CExitControl::getState()
{return onExit;}

////
void CExitControl::setPause(bool pause)
{onPause = pause;}

////
bool CExitControl::isPause()
{return onPause;}