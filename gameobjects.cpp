//
//	Methods of Game Objects
//

#include "stdafx.h"
#include "Antares\\Antares.h"
#include "Antares\\timer.h"
#include "Antares\\crossdetect.h"
#include "gameobjects.h"

extern CTimer timer; // timer variable for animation


// player object visualisation constants
const unsigned int initialBlinksNumber = 19;
const float visibleBlinkPeriod = 0.1f;
const float invisibleBlinkPeriod = 0.04f;


////
bool CEnemy::isMotherObjectActive = NULL;

//////
CEnemy::CEnemy()
{
	currentFrame = 0;
	animateType = NOT_ANIMATED;
	isActive = true;
	pFrames = NULL;
	fireType = NOT_FIRE;
	fireInterval = 1.0f;
    fireProbability = MAX_FIRE_PROBABILITY;

	x = 400.0f;
	y = 500.0f;
	width = 32;
	height = 32;
	hitPoints = 20; // temp value
	deathTime = 0.0f; // temp value
	linkType = NOT_LINKED; // default link type
    score = 0;

    hitPointsThreshold = -100; // заведомо отрицательная величина, чтобы по-умолчанию, не возникала бы генерация дыма
    smokeEffectStart = 0;
    smokeInterval = 0.5f;
}

////
void CEnemy::init(float xf, float yf, int tType, float vxmax, float vxmin, float xamp, float vyf)
{
	flashPeriod = 0.1f;	// set time for flash after enemy was hitted
	isFlashing = false;
	playDeathFlag = false;

	linkType = NOT_LINKED;
	turretAllowedDirections = TURRET_MASK_ALL;

	fireStartPeriod = timer.getTime();

	x = xf;
	y = yf;
	vx = vxmax;
	vy = vyf;

	trajectoryType = tType;

	if (tType == SIN_MOVEMENT)
	{
		if ((vxmax > 0 && vxmin < 0) || (vxmax < 0 && vxmin > 0))
			MessageBox(NULL, L"Error calculating acceleration by x axis!", L"Error!", MB_OK);

		vxMax = flabs(vxmax);
		vxMin = flabs(vxmin);
		xAmp = flabs(xamp);
		xAxis = flabs(x);

		xAccel = (vxmax - vxmin) / xamp;

		if (xAccel == 0)
			trajectoryType = LIN_MOVEMENT;
		else
		{
			trajectoryType = SIN_MOVEMENT;

			if (xAccel > 0)
				halfWave = PLUS_HALF_WAVE;
			else
				halfWave = MINUS_HALF_WAVE;

			vxPhase = VX_FALLING_PHASE; // abs value of vx decrease
		}

		xAccel = flabs(xAccel);

	}
}

////
void CEnemy::uninit()
{
	x = 0;
	y = 0;
	vx = 0;
	vy = 0;
	width = 0;
	height = 0;
    cycledAnimatePhase = 0;
	//	uninitFrames();
}

////
void CEnemy::setImage(CSprite img)
{image = img;}

////
void CEnemy::setMask(CCrossMask mask)
{this->mask = mask;}

////
CCrossMask CEnemy::getMask()
{return mask;}

////
void CEnemy::setFrames(CFramedObject *fobj)
{
	pFrames = fobj;
	width = fobj->getWidth();
	height = fobj->getHeight();
}

//// 
RECT CEnemy::getCurrentFrame()
{
	return pFrames->getFrame(currentFrame);
}

////
void CEnemy::setCurrentFrame(unsigned int frame)
{
	if (pFrames == NULL)
		MessageBox(NULL, L"Framed object not initialized", L"Error!", MB_OK);
	else
	{
		if (frame < pFrames->getMaxFrames())
			currentFrame = frame;
		else
			MessageBox(NULL, L"Frame not found", L"Error!", MB_OK);
	}

}

///
int CEnemy::draw()
{
	if (currentFrame >= pFrames->getMaxFrames())
	{
		MessageBox(NULL, L"Number of frame is too large", L"Error CEnemy!", MB_OK);
		return -1;
	}
	else
	if (width < 2 || height < 2)
	{
		MessageBox(NULL, L"Dimensions is too low", L"Error!", MB_OK);
		return -2;
	}
	else
	if (isVisible == false)
		return -3;
	else
	{
		if (isFlashing == true)
		{
			if ((timer.getTime() - flashStartTime) < flashPeriod)
			{
				// draw flashing sprite
				// color balance turns red
				image.drawSpriteEx((int)x - width / 2, (int)y - height / 2, pFrames->getFrame(currentFrame), 
								   NO_SCALE, NOT_REFLECT, 255, 128, 128);
			} else 
				isFlashing = false;
		} else // draw normal sprite
			image.drawSprite((int)x - width / 2, (int)y - height / 2, pFrames->getFrame(currentFrame), NO_SCALE, NOT_REFLECT);

	}

	return 0;
}

////
bool CEnemy::isEnemyAlive()
{return isActive;}

///
int CEnemy::getX0()
{return ((int)x - width/2);}

///
int CEnemy::getY0()
{return ((int)y - height / 2);}

///
int CEnemy::getX()
{return (int)x;}

///
int CEnemy::getY()
{return (int)y;}

////
void CEnemy::setHitPoints(int h)
{hitPoints = h;}

////
void CEnemy::setHit(int hitValue)
{
	// decrease hitpoint value
	hitPoints -= hitValue;

	// if hitpoints < 0, set 'killed' status
	if (hitPoints <= 0)
	{
		playDeathFlag = true;
		deathStartTime = timer.getTime();
		trajectoryType = LIN_MOVEMENT;
	}

	// start flashing
	isFlashing = true;
	flashStartTime = timer.getTime();
}

////
void CEnemy::setDeathTime(float t)
{deathTime = t;}

////
void CEnemy::setCrossingDamage(int dmg)
{crossingDamage = dmg;}

///
int CEnemy::getCrossingDamage()
{return crossingDamage;}

////
bool CEnemy::isPlayDeath()
{return playDeathFlag;}

////
void CEnemy::setAnimateType(unsigned int aType)
{animateType = aType;}

////
void CEnemy::setAnimateInterval(float aInterval)
{animateInterval = aInterval;}

////
void CEnemy::setFireType(int fType)
{fireType = fType;}

///
int CEnemy::getFireType()
{return fireType;}

////
void CEnemy::setFireInterval(float fInterval)
{fireInterval = fInterval;}

/////
float CEnemy::getFireInterval()
{return fireInterval;}

/////
float CEnemy::getLastFireTime()
{return fireStartPeriod;}

////
void CEnemy::setLastFireTime()
{fireStartPeriod = timer.getTime();}

////
void CEnemy::correctLastFireTime(float time)
{fireStartPeriod = time;}

////
void CEnemy::setFireProbability(unsigned int fireProbability)
{
    if (fireProbability <= MAX_FIRE_PROBABILITY)
        this->fireProbability = fireProbability;
    else 
        this->fireProbability = MAX_FIRE_PROBABILITY;
}

////
unsigned int CEnemy::getFireProbability()
{return fireProbability;}

////
void CEnemy::setMissleNumber(int n)
{missleNumber = n;}

///
int CEnemy::getMissleNumber()
{return missleNumber;}

////
void CEnemy::setLinkType(unsigned char linkType)
{this->linkType = linkType;}

////
void CEnemy::setTurretDirections(unsigned char turretAllowedDirections)
{this->turretAllowedDirections = turretAllowedDirections;}

////
unsigned char CEnemy::getTurretDirections()
{return turretAllowedDirections;}

////
void CEnemy::setScore(unsigned int score)
{this->score = score;}

////
unsigned int CEnemy::getScore()
{return score;}

////
void CEnemy::processMovement(float timeDelta) 
{
	// sin movement processing
	if (trajectoryType == SIN_MOVEMENT)
	{
		if (halfWave == PLUS_HALF_WAVE)
		{
			if (vxPhase == VX_FALLING_PHASE)
			{
				if (x - xAxis >= xAmp)
					vxPhase = VX_RISING_PHASE;
				else
					vx = -xAccel * (x - xAxis) + vxMax;
			}
			else
			{
				// if vxPhase == VX_RISING_PHASE
				if (x <= xAxis)
				{
					vxPhase = VX_FALLING_PHASE;
					halfWave = MINUS_HALF_WAVE;
				}
				else
					vx = -(xAccel * (xAmp + xAxis - x) + vxMin);
			}
		}
		else
		{
			// if halfWave == MINUS_HALF_WAVE
			if (vxPhase == VX_FALLING_PHASE)
			{
				if (xAxis - x >= xAmp)
					vxPhase = VX_RISING_PHASE;
				else
					vx = xAccel * (xAxis - x) - vxMax;
			}
			else
			{
				// if vxPhase == VX_RISING_PHASE
				if (x >= xAxis)
				{
					vxPhase = VX_FALLING_PHASE;
					halfWave = PLUS_HALF_WAVE;
				}
				else
					vx = xAccel * (x - xAxis + xAmp) + vxMin;
			}
		}
	}


	// checking if play death or normal mode
	if (playDeathFlag == true)
	{
		if (timer.getTime() - deathStartTime > deathTime)
			isActive = false;
	} else 
	{
		// update coordinates
		x += vx*timeDelta;
		y += vy*timeDelta;
	}


	// is object for visible or/and active
    if (((int)x < -(int)width * 2) || ((int)x >(800 + (int)width * 2)) || ((int)y < -(int)height * 2))
	{
		isVisible = false;
		isActive = false; // out of action limits
	}
	else
	{
//		isActive = true;

		if ((int)y >(600 + (int)height / 2))
			isVisible = false; // out of visible limits
		else
			isVisible = true;
	}

	// process cicled animation of enemy
	if (animateType == CICLED_ANIMATED)
	{
		if ((timer.getTime() - startAnimPeriod) > animateInterval)
		{
			startAnimPeriod = timer.getTime();

			if (currentFrame < (pFrames->getMaxFrames() - 1))
				currentFrame++;
			else
				currentFrame = 0;
		}
	}

    // process cicled animation of enemy
    if (animateType == CICLED_ANIMATED)
    {
        if ((timer.getTime() - startAnimPeriod) > animateInterval)
        {
            startAnimPeriod = timer.getTime();

            if (currentFrame < (pFrames->getMaxFrames() - 1))
                currentFrame++;
            else
                currentFrame = 0;
        }
    }

    // обработка типа анимации PINPONG объекта противника
    if (animateType == PINPONG_ANIMATED)
	{
		if ((timer.getTime() - startAnimPeriod) > animateInterval)
		{
			startAnimPeriod = timer.getTime();

            // если находимся в фазе повышения номера кадров
            if (cycledAnimatePhase == 0)
            {
                if (currentFrame < (pFrames->getMaxFrames() - 1))
                    currentFrame++;
                else 
                {   // достигли предела максимального кол-ва кадров
                    cycledAnimatePhase = 1; // следующая фаза - понижение номера кадров при анимации
                    currentFrame--;
                }
                    
            } else // если находимся в фазе понижения номера кадров (cycledAnimatePhase == 1)
            {
                if (currentFrame > 0)
                    currentFrame--;
                else
                {   // достигли предела максимального кол-ва кадров
                    cycledAnimatePhase = 0; // следующая фаза - повышение номера кадров при анимации
                    currentFrame++;
                }
            }

		} // end of next frame time occured
    } // end of if animateType == PINPONG_ANIMATED


	// chose the frame for NORMAL_ANIMATED type enemy
	// there are 3 frames allowed to use
	if (animateType == NORMAL_ANIMATED)
	{
		if (vx >  CENTER_SPEED_X_THRESHOLD) currentFrame = RIGHT_MOVE_FRAME;
		else
		if (vx < -CENTER_SPEED_X_THRESHOLD) currentFrame = LEFT_MOVE_FRAME;
		else
			currentFrame = CENTER_MOVE_FRAME;
	}


	// process links between objects
	switch (linkType)
	{
		case MOTHER_OBJECT:
			isMotherObjectActive = isActive;
			break;

		case DAUGHTER_OBJECT:
			if (isMotherObjectActive == false)
				isActive = false; // delete daughter object if mother objects is dead
			break;
	}

}

////
void CEnemy::kill()
{isActive = false;}

////
void CEnemy::setSmokeControl(int hitPointsThreshold, float smokeInterval, int xSmokeOffset, int ySmokeOffset)
{
    this->xSmokeOffset = xSmokeOffset;
    this->ySmokeOffset = ySmokeOffset;
    this->hitPointsThreshold = hitPointsThreshold; // устанавливаем порог HP, ниже которого идет генерация дыма 
    this->smokeInterval = smokeInterval; // устанавливаем интервал времени генерации нового объекта эффекта дыма 
}

////
bool CEnemy::processSmoke(CTimer *timer)
{
    // если порог, после которого можно производить генерацию дыма пройден
    if (hitPoints < hitPointsThreshold)
    {
        if ((timer->getTime() - smokeEffectStart) > smokeInterval)
        {
            smokeEffectStart = timer->getTime();

            return true;

        } else
            return false;
    }else
        return false;
}

///
int CEnemy::getSmokeX()
{return ((int)x + xSmokeOffset);}

///
int CEnemy::getSmokeY()
{return ((int)y + ySmokeOffset);}


////////////////////////////////////// 
///////// Methods of CPlayer /////////

////
CPlayer::CPlayer()
{
	currentFrame = 0;
	x = 390.0f;
	y = 30.0f;
    score = 0;

    // установка текущего и максимального значений жизни игрока по-умолчанию
    lives = 3;
    maxLives = 3;
}

////
void CPlayer::setImage(CSprite img)
{image = img;}

////
void CPlayer::setMask(CCrossMask mask)
{this->mask = mask;}

////
CCrossMask CPlayer::getMask()
{return mask;}


///
int CPlayer::draw()
{
	if (currentFrame >= pFrames->getMaxFrames())
	{
		MessageBox(NULL, L"Number of frame is too large", L"Error!", MB_OK);
		return -1;
	}
	else
	if (width < 2 || height < 2)
	{
		MessageBox(NULL, L"Dimensions is too low", L"Error!", MB_OK);
		return -2;
	}
	else
	if (isVisible == false)
		return -3;
	else
		image.drawSprite((int)x - width / 2, (int)y - height / 2, pFrames->getFrame(currentFrame), NO_SCALE, NOT_REFLECT);

	return 0;
}

////
void CPlayer::setFrames(CFramedObject *fobj)
{
	pFrames = fobj;
	width = fobj->getWidth();
	height = fobj->getHeight();
}

////
RECT CPlayer::getCurrentFrame()
{return pFrames->getFrame(currentFrame);}


////
void CPlayer::setLives(int maxLives)
{
    if (maxLives > PLAYER_MAX_LIVES)
    {
        lives = PLAYER_MAX_LIVES;
        this->maxLives = PLAYER_MAX_LIVES;
    }
    else
    {
        lives = maxLives;
        this->maxLives = maxLives;
    }
}

////
void CPlayer::init(float xf, float yf, float vxf, float vyf)
{
	isActive = true;
	isVisible = true;
	isInitialBlinking = true;
	startBlinkInterval = timer.getTime();
	blinkInterval = visibleBlinkPeriod;
	blinkCounter = initialBlinksNumber;

	playDeathFlag = false;

	x = xf; x0 = xf;
	y = yf; y0 = yf;
	
	if (vxf < 0) vx = vxf * (-1);
	else vx = vxf;

	if (vyf < 0) vy = vyf * (-1);
	else vy = vyf;

	currentFrame = 0;
}

////
void CPlayer::processMovement(float timeDelta)
{
	// checking if play death or normal mode
	if (playDeathFlag == false)
	{
		// [de]increase players coordinate & checking game limits
		if ((direction & MOVE_DOWN) && (y > height / 2)) y += -vy*timeDelta;
		else{
			if ((direction & MOVE_UP) && (y < 600 - height / 2)) y += vy*timeDelta;
			else y += 0;
		}

		if ((direction & MOVE_LEFT) && (x > width / 2))
		{
			x += -vx*timeDelta;
			currentFrame = PLAYER_FRAME_LEFT;
		}else
		{
            if ((direction & MOVE_RIGHT) && (x < 800 - width / 2))
			{
				x += vx*timeDelta;
				currentFrame = PLAYER_FRAME_RIGHT;
			}else
			{
				x += 0;
				currentFrame = PLAYER_FRAME_NORMAL;
			}
		}

	}else
	{
		// waiting for playing death end
		if (timer.getTime() - deathStartTime > deathTime)
		{
			if (lives > 0)
			{
				lives--; // reduce lives number

				if (lives == 0)
				{
					// lives is over - inactivate & hide player
					isActive = false;	// player inactive
					isVisible = false;
				}
				else{
					// for next live: set player to base position
					playDeathFlag = false;
					x = x0;
					y = y0;
					isInitialBlinking = true;
				}
			}
		}
	}

	// processing initial blinking of player object
	if (isInitialBlinking == true)
	{
		if ((timer.getTime() - startBlinkInterval) > blinkInterval)
		{
			startBlinkInterval = timer.getTime();	
			blinkCounter--;

			if (blinkCounter > 0)
			{
				if (isVisible == true)
				{
					isVisible = false;
					blinkInterval = invisibleBlinkPeriod;
				}
				else
				{
					isVisible = true;
					blinkInterval = visibleBlinkPeriod;
				}
			}
			else
			{
				blinkCounter = initialBlinksNumber;
				isInitialBlinking = false;
			}

		}
	}

}

////
void CPlayer::setHit(int hitValue)
{
	// decrease hitpoint value
	hitPoints -= hitValue;

	// if hitpoints < 0, set 'killed' status
	if (hitPoints < 0)
	{
		playDeathFlag = true;
		deathStartTime = timer.getTime();
	}

	// start flashing
//	isFlashing = true;
//	flashStartTime = timer.getTime();
}

////
void CPlayer::setDirection(unsigned char dir)
{direction = dir;}

////
void CPlayer::setMissleInterval(float mInterval)
{missleInterval = mInterval;}

////
void CPlayer::setLastMissleTime(float mTime)
{lastMissleTime = mTime;}

////
bool CPlayer::isReadyToFire(float currentTime)
{
	if (currentTime >= (lastMissleTime + missleInterval))
		return true;
	else
		return false;
}

/////
float CPlayer::getX()
{return x;}

/////
float CPlayer::getY()
{return y;}

///
int CPlayer::getX0()
{return ((int)x - width / 2);}

///
int CPlayer::getY0()
{return ((int)y - height / 2);}

////
void CPlayer::setCrossingDamage(int dmg)
{crossingDamage = dmg;}

////
void CPlayer::setHitPoints(int hp)
{hitPoints = hp;}

///
int CPlayer::getCrossingDamage()
{return crossingDamage;}

///
int CPlayer::getHitPoints()
{return hitPoints;}

////
void CPlayer::setDeathTime(float t)
{deathTime = t;}

////
bool CPlayer::isPlayDeath()
{return playDeathFlag;}

////
bool CPlayer::isBlinking()
{return isInitialBlinking;}

////////////
int CPlayer::getLivesNumber()
{return lives;}

////
void CPlayer::addLife()
{if (lives < maxLives) lives++;}

////
bool CPlayer::isPlayerActive()
{return isActive;}

////////////
unsigned int CPlayer::getWidth()
{return width;}

////////////
unsigned int CPlayer::getHeight()
{return height;}

////
void CPlayer::incrementScore(unsigned int score)
{
    this->score += score;
    
    // !!! временная защита от превышения максимально допустимого числа набранных игроком очков 
    if (this->score >= PLAYER_MAX_SCORE) this->score = PLAYER_MAX_SCORE - 1;
}

////
unsigned int CPlayer::getScore()
{return score;}

////
int CPlayer::getMaxLives()
{return maxLives;}

////
void CPlayer::clearScore()
{score = 0;}

////////////////////////////////////// 
///////// Methods of CMissle /////////

////
CMissle::CMissle()
{
	x = 390.0f;
	y = 30.0f;
	vx = 50.0f;
	vy = 0.0f;
	currentFrame = 0;
	damage = 10; // temp value
}

////
void CMissle::setImage(CSprite img)
{image = img;}

////
void CMissle::setMask(CCrossMask mask)
{this->mask = mask;}

////
CCrossMask CMissle::getMask()
{return mask;}

////
void CMissle::setFrames(CFramedObject *fobj)
{
	pFrames = fobj;
	width = fobj->getWidth();
	height = fobj->getHeight();
}

////
RECT CMissle::getCurrentFrame()
{return pFrames->getFrame(currentFrame);}

///
int CMissle::draw()
{
	if (currentFrame >= pFrames->getMaxFrames())
	{
		MessageBox(NULL, L"Number of frame is too large", L"Error CMissle!", MB_OK);
		return -1;
	}
	else
	if (width < 2 || height < 2)
	{
		MessageBox(NULL, L"Dimensions is too low", L"Error!", MB_OK);
		return -2;
	}
	//	else
	//	if (isVisible == false)
	//		return -3;
	else
		image.drawSprite((int)x - width / 2, (int)y - height / 2, pFrames->getFrame(currentFrame), NO_SCALE, NOT_REFLECT);

	return 0;
}

////
void CMissle::processMovement(float timeDelta)
{
	// is object for visible or/and active
    if (((int)x < -(int)width / 2) || ((int)x >(800 + (int)width / 2)) || ((int)y > 600 + (int)height / 2) || (int)y < -(int)height / 2)
	{
		isActive = false; // out of action limits
	} 
	else
	{
		x += vx*timeDelta;
		y += vy*timeDelta;
	}

	if ((timer.getTime() - startAnimPeriod) > animateInterval)
	{
		startAnimPeriod = timer.getTime();

		if (currentFrame < (pFrames->getMaxFrames() - 1))
			currentFrame++;
		else
			currentFrame = 0;

	}
}
 
////
bool CMissle::isMissleAlive()
{return isActive;}

///
int CMissle::getX0()
{return ((int)x - width / 2);}

///
int CMissle::getY0()
{return ((int)y - height / 2);}

////
void CMissle::setDamage(int d)
{damage = d;}

////
void CMissle::setAliveStatus(bool aStatus)
{isActive = aStatus;}

///
int CMissle::getDamage()
{return damage;}

////
void CMissle::setTargetType(int targ)
{target = targ;}

///
int CMissle::getTargetType()
{return target;}

////
void CMissle::setAnimateInterval(float aInterval)
{animateInterval = aInterval;}

////
void CMissle::setSpeed(float vxf, float vyf)
{vx = vxf; vy = vyf;}

////
void CMissle::setSpeedValue(float sValue)
{speedValue = sValue;}

/////
float CMissle::getSpeedValue()
{return speedValue;}

////
void CMissle::init(float xf, float yf)
{
	isActive = true;
	startAnimPeriod = timer.getTime();
	x = xf;
	y = yf;
	currentFrame = 0;
}

////
void CMissle::kill()
{isActive = false;}

////////////////////////////////////// 
///////// Methods of CEffect /////////

////
void CEffect::setImage(CSprite img)
{image = img;}

////
void CEffect::setFrames(CFramedObject *fobj)
{
	pFrames = fobj;
	width = fobj->getWidth();
	height = fobj->getHeight();
}

////
bool CEffect::isAlive()
{return isActive;}

////
void CEffect::setFramePeriod(float fPeriod)
{framePeriod = fPeriod;}

////
void CEffect::init(float xf, float yf, float vxf, float vyf, float tDelay, unsigned int rep)
{
	currentFrame = 0;
	currentRepetition = 0;
	isActive = true;
	isVisible = false;
	timeStarted = timer.getTime();
	timeDelay = tDelay;

	x = xf;
	y = yf;
	vx = vxf;
	vy = vyf;
	
	if (rep == 0)
		MessageBox(NULL, L"Number of effects repetitions should be > 0", L"Error!", MB_OK);
	else
		nRepetition = rep;
}

////
int CEffect::draw()
{
	if (currentFrame >= pFrames->getMaxFrames())
	{
		MessageBox(NULL, L"Number of frame is too large", L"Error CEffect!", MB_OK);
		return -1;
	}
	else
	if (width < 2 || height < 2)
	{
		MessageBox(NULL, L"Dimension too low", L"Error!", MB_OK);
		return -2;

	} else
	if (isVisible == false)
		return -3;
	else
	if ((timer.getTime() - timeStarted) > timeDelay)	// skip delay period
		image.drawSprite((int)x - width / 2, (int)y - height / 2, pFrames->getFrame(currentFrame), NO_SCALE, NOT_REFLECT);

	return 0;
}

////
void CEffect::processEffect(float timeDelta)
{
	if (isActive == true)
	{
		if (isVisible == false)
		{
			if ((timer.getTime() - timeStarted) > timeDelay)	// skip initial delay period
			{
				isVisible = true;
				currentFrame = 0;
				currentFrameStart = timer.getTime();
			}
		}
		else
		{
			x += vx*timeDelta;
			y += vy*timeDelta;

			if ((timer.getTime() - currentFrameStart) > framePeriod)
			{
                currentFrame++;
                currentFrameStart = timer.getTime();

                if (currentFrame == pFrames->getMaxFrames())
                {
                    currentFrame = 0;

                    currentRepetition++;
                    if (currentRepetition == nRepetition)
                    {
                        currentRepetition = 0;
                        isActive = false;
                        isVisible = false;
                    }
                }
			}

		}

	}
}
 
///////
CEffect::CEffect()
{
	framePeriod = 0.1f;
	timeDelay = 0;
	nRepetition = 1;
	currentRepetition = 0;
}

////
void CEffect::kill()
{isActive = false;}

////////////////////////////////////// 
////////// Methods of CStar //////////

/////
CStar::CStar()
{

}

////
void CStar::init(unsigned int n, float xf, float yf, float vyf)
{
	x = xf;
	y = yf;
	vy = vyf;

	if (n < pFrames->getMaxFrames())
		currentFrame = n;
	else
		MessageBox(NULL, L"Can't set this frame number for star!", L"Error!", MB_OK);

	isActive = true;
}

////
void CStar::processMovement(float timeDelta)
{
	if (y < -((float)height / 2))
		isActive = false;
	else
		y += vy * timeDelta;
}

////
///
int CStar::draw()
{
	if (currentFrame >= pFrames->getMaxFrames())
	{
		MessageBox(NULL, L"Number of frame is too large", L"Error CStar!", MB_OK);
		return -1;
	}
	else
	if (width < 2 || height < 2)
	{
		MessageBox(NULL, L"Dimensions is too low", L"Error!", MB_OK);
		return -2;
	}
	//	else
	//	if (isVisible == false)
	//		return -3;
	else
		image.drawSprite((int)x - width / 2, (int)y - height / 2, pFrames->getFrame(currentFrame), NO_SCALE, NOT_REFLECT);

	return 0;
}

////
void CStar::setImage(CSprite img)
{image = img;}

////
void CStar::setFrames(CFramedObject *fobj)
{
	pFrames = fobj;
	width = fobj->getWidth();
	height = fobj->getHeight();
}

////
bool CStar::isAlive()
{return isActive;}


///////////////////////////////////////////////////////////////

////
void CNumbers::init(CSprite img, int x0, int y0, int sizeX, int sizeY)
{
    this->img = img;
    this->x0 = x0;
    this->y0 = y0;
    this->sizeX = sizeX;
    this->sizeY = sizeY;
}

////
void CNumbers::drawDigit(int x, int y, unsigned char number)
{
    RECT r;

    if (number > 4 && number < 10)
    {
        r.top = y0 + sizeY;
        r.bottom = y0 + 2 * sizeY - 1;

        r.left = x0 + (number - 5) * sizeX;
        r.right = x0 + (number - 5) * sizeX + sizeX - 1;

    } else
    if (number < 5)
    {
        r.top = y0;
        r.bottom = y0 + sizeY - 1;

        r.left = x0 + number * sizeX;
        r.right = x0 + number * sizeX + sizeX - 1;
    }

    img.drawSprite(x, y, r, NO_SCALE, NOT_REFLECT);
}

////
void CNumbers::draw(int x, int y, unsigned char digits, unsigned int value)
{
    ///
    unsigned int val = value;
    unsigned int divider = 1;
    unsigned char i;

    // вычисляем начальное значение делителя (число на которое нужно поделить входное значение, 
    // чтобы в результате в целой части ответа было значение величины старшего разряда 
    for (i = 0; i < digits - 1; i++)
        divider *= 10;

    // вывод разрядов 
    for (i = 1; i < digits + 1; i++)
    {
        this->drawDigit(x + (i - 1) * sizeX, y, val / divider);

        val = val % divider;
        divider = divider / 10; // уменьшить величину делителя на 1 разряд 
    }
}


///////////////////////////////////////////////////////////////

////
void CLifeIndicator::init(int x1, int x2, int y1, int y2, unsigned char status, CSprite img)
{
    RECT tempRect;

    if (x2 > x1)
    {
        tempRect.left = x1;
        tempRect.right = x2;
    } else
    {
        tempRect.left = x2;
        tempRect.right = x1;
    }

    if (y2 > y1)
    {
        tempRect.top = y1;
        tempRect.bottom = y2;

    }else
    {
        tempRect.top = y2;
        tempRect.bottom = y1;
    }


    if (status == ALIVE_TILE)
        aliveRect = tempRect;
    else
    if (status == DEAD_TILE)
        deadRect = tempRect;

    this->img = img;
}

////
void CLifeIndicator::draw(int x, int y, unsigned char status)
{
    if (status == ALIVE_TILE)
        img.drawSprite(x, y, aliveRect, NO_SCALE, NOT_REFLECT);
    else
    if (status == DEAD_TILE)
        img.drawSprite(x, y, deadRect, NO_SCALE, NOT_REFLECT);
}


///////////////////////////////////////////////////////////////

//////
CLevel::CLevel()
{
    // обнуление счетчиков 
    readedEntries   = 0;
    currentEntry    = 0;
    currentLevel    = 0;
    lastEntryTime   = 0;
}

///
int CLevel::load(char fileName[MAX_FILE_NAME_LENGTH])
{
    FILE *fp; 
    int  eol = 0; // для поиска маркера {EOF = (-1)}

    readedEntries   = 0; // пока ни одной записи не прочитано из файла 
    currentEntry    = 0; // номер текущей считываемой записи 

    if (fopen_s(&fp, fileName, "rt") != 0)
    {
        MessageBox(NULL, L"Error opening level file!", L"Error!", MB_OK);
        return 0; // выход из ф-ии чтения уровня, если не удалось обнаружить файл с таким именем 
    }
        

    while (eol != EOF)
    {
        // считать интервал времени активации параметров записи и кол-во объектов в записи 
        eol = fscanf_s(fp, "%f %i", &entry[readedEntries].timeWait, &entry[readedEntries].amount);

        // выход из цикла, если не можем произвести считывание 
        if (eol == EOF) break;

        // защита от превышения считывания допустимого кол-ва объектов в записи 
        if (entry[readedEntries].amount > MAX_OBJECTS_IN_ENTRY)
            entry[readedEntries].amount = MAX_OBJECTS_IN_ENTRY;

        // цикл считывания параметров объектов текущей записи 
        for (unsigned int i = 0; i < entry[readedEntries].amount; i++)
        {
            // считывание типа объекта, начальной координаты x, начальной координаты y 
            eol = fscanf_s(fp, "%i %f %f", &entry[readedEntries].levelObject[i].type,
                                           &entry[readedEntries].levelObject[i].x,
                                           &entry[readedEntries].levelObject[i].y);

            // считывание типа траектории объекта 
            eol = fscanf_s(fp, "%u", &entry[readedEntries].levelObject[i].tType);

            // считывание максимальной, минимальной скоростей, амплитуды перемещения по x, скорости по y 
            eol = fscanf_s(fp, "%f %f %f %f", &entry[readedEntries].levelObject[i].vxMax,
                                              &entry[readedEntries].levelObject[i].vxMin,
                                              &entry[readedEntries].levelObject[i].xAmp,
                                              &entry[readedEntries].levelObject[i].vy);

            // считывание кода разрешенных направлений стрельбы (действует, если противник - туррель) 
            eol = fscanf_s(fp, "%u", &entry[readedEntries].levelObject[i].tDir);

            // статус соединения - устанавливает отсутствия связи / дочерний объект / материнский объект 
            eol = fscanf_s(fp, "%u", &entry[readedEntries].levelObject[i].lStat);

            if (eol == EOF) break;
        }
        
        // защита от считывания большего числа записей в уровне, чем предусмотрено
        if (readedEntries < MAX_LEVEL_ENTRIES - 1)
            readedEntries++;
        else
        {
            MessageBox(NULL, L"Level file has too many entries!", L"Error!", MB_OK);
            break;  // выход из цикла считывания записей 
        }
            
    }


    fclose(fp);

    // !!! если не было ошибок - инкремент номера уровня 
    currentLevel++;

/*
/////   проверка 
/////
    fopen_s(&fp, "level_debug.txt", "wt");

    for (unsigned i = 0; i < readedEntries; i++)
    {
        fprintf_s(fp, "%f %u\n", entry[i].timeWait, entry[i].amount);

        for (unsigned int j = 0; j < entry[i].amount; j++)
        {
            fprintf_s(fp, "%i %f %f ", entry[i].levelObject[j].type, entry[i].levelObject[j].x, entry[i].levelObject[j].y);
            fprintf_s(fp, "%i %f %f %f %f\n", entry[i].levelObject[j].tType, 
                                              entry[i].levelObject[j].vxMax, 
                                              entry[i].levelObject[j].vxMin, 
                                              entry[i].levelObject[j].xAmp, 
                                              entry[i].levelObject[j].vy);
        }
    }

    fclose(fp);
/////
/////
*/
    return 1;
}


///////////////////////////////////////////////////////////////

//////////
CLevelList::CLevelList()
{
    addedLevels = 0;
    currentLevel = 0;
}

////
void CLevelList::add(char fileName[MAX_FILE_NAME_LENGTH])
{
    for (unsigned int i = 0; i < MAX_FILE_NAME_LENGTH; i++)
        this->fileName[addedLevels][i] = fileName[i];

    addedLevels++;
}


///////////////////////////////////////////////////////////////

/////
float CGameMenu::changeItemDelay;
float CGameMenu::itemSelectTime;
CSprite CGameMenu::img;

/////////
CGameMenu::CGameMenu()
{
    itemsAmount = 0;
    currentItem = 0;
    itemSelectTime = 0;
}

////
void CGameMenu::addItem(int x, int y, unsigned int width)
{
    if (itemsAmount < MAX_MENU_ITEMS)
    {
        itemCoord[itemsAmount].x = x;
        itemCoord[itemsAmount].y = y;
        itemCoord[itemsAmount].width = width;

        itemsAmount++;
    }
}

////
void CGameMenu::setSelectorRect(int x1, int x2, int y1, int y2)
{
    if (x2 > x1)
    {
        r.left = x1;
        r.right = x2;
    }else 
    {
        r.left = x2;
        r.right = x1;
    }

    if (y2 > y1)
    {
        r.top = y1;
        r.bottom = y2;
    }else 
    {
        r.top = y2;
        r.bottom = y1;
    }
}

////
void CGameMenu::selectNextItem(float time)
{
    if ((time - itemSelectTime) > changeItemDelay)
    {
        itemSelectTime = time;

        if (currentItem < itemsAmount - 1)
            currentItem++;
        else
            currentItem = 0;
    }
}

////
void CGameMenu::selectPrevItem(float time)
{
    if ((time - itemSelectTime) > changeItemDelay)
    {
        itemSelectTime = time;

        if (currentItem > 0)
            currentItem--;
        else
            currentItem = itemsAmount - 1;
    }
}

////
void CGameMenu::drawSelector()
{
    int n = itemCoord[currentItem].width / (r.right - r.left + 1);

    for (int i = 0; i < n; i++)
    {
        img.drawSprite(itemCoord[currentItem].x + i * (r.right - r.left + 1), itemCoord[currentItem].y, r, NO_SCALE, NOT_REFLECT);
    }
}

////
unsigned int CGameMenu::getSelected()
{return currentItem;}

////
void CGameMenu::setSelected(unsigned int n)
{currentItem = n;}


///////////////////////////////////////////////////////////////

///////////
CColorFlash::CColorFlash()
{
    r1 = g1 = b1 = 127;
    r2 = g2 = b2 = 255;
    period = 1.0f;
    timePhaseStart = 0;
}

////
void CColorFlash::init(unsigned int color1, unsigned int color2, float period, float time)
{
    b1 = color1 & 0x000000FF;
    g1 = (color1 >> 8) & 0x000000FF;
    r1 = (color1 >> 16) & 0x000000FF;

    b2 = color2 & 0x000000FF;
    g2 = (color2 >> 8) & 0x000000FF;
    r2 = (color2 >> 16) & 0x000000FF;

    this->period = period;
    timePhaseStart = time;
    phase = RISING_PHASE;
}

////
void CColorFlash::process(float time)
{
    float colorDelta; // приращение компоненты цвета

    if (time >= (timePhaseStart + period))
    {
        unsigned char rTemp, gTemp, bTemp;

        // фиксируем время начала новой фазы
        timePhaseStart = time;

        // меняем компоненты цвета начальной и конечной фаз
        rTemp = r2; gTemp = g2; bTemp = b2;
        r2 = r1; g2 = g1; b2 = b1;
        r1 = rTemp; g1 = gTemp; b1 = bTemp;

        // переключаем фазу
        if (phase == RISING_PHASE)
            phase = FALLING_PHASE;
        else 
            phase = RISING_PHASE; 

    } else 
    {
        colorDelta = (time - timePhaseStart) / period;

        if (r2 >= r1) r = r1 + (unsigned char)((float)(r2 - r1) * colorDelta);
        else r = r2 + (unsigned char)((float)(r1 - r2) * (1.0f - colorDelta));

        if (g2 >= g1) g = g1 + (unsigned char)((float)(g2 - g1) * colorDelta);
        else g = g2 + (unsigned char)((float)(g1 - g2) * (1.0f - colorDelta));

        if (b2 >= b1) b = b1 + (unsigned char)((float)(b2 - b1) * colorDelta);
        else b = b2 + (unsigned char)((float)(b1 - b2) * (1.0f - colorDelta));
    }
}

////
unsigned char CColorFlash::getRed()
{return r;}

////
unsigned char CColorFlash::getGreen()
{return g;}

////
unsigned char CColorFlash::getBlue()
{return b;}