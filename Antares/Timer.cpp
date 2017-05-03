#include "Timer.h"

////
CTimer::CTimer()
{
    fps = 0;
    lastTime = 0;
}

////
void CTimer::setTime(float time)
{
	this->time = time;
}

////
void CTimer::setDeltaTime(float delta)
{
	this->deltaTime = delta;
}

////
void CTimer::setFPS(unsigned int fps)
{
	numberFPS = fps;
}

/////
float CTimer::getTime()
{
	return time;
}

/////
float CTimer::getDeltaTime()
{
	return deltaTime;
}

/////
unsigned int CTimer::getFPS()
{
	return numberFPS;
}

////
void CTimer::incrementFps()
{fps++;}

////
void CTimer::clearFps()
{fps = 0;}

////
unsigned int CTimer::getFps()
{return fps;}

////
void CTimer::setLastTime(float lastTime)
{this->lastTime = lastTime;}

/////
float CTimer::getLastTime()
{return lastTime;}