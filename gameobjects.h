//	Game Objects & Structures  
//
#define flabs( f ) ( f>0 ? f : -f )

////////
template <class T, unsigned int size>
class GameObjectList{

private:

	T *pObject;
	unsigned int currentElement;
	bool empty;

public:

	GameObjectList();
	~GameObjectList(void);
	void addObject(T obj);
	void removeObject(unsigned int n);
	unsigned int getElementsNumber();
	T getObject(unsigned int n);
	int reloadObject(unsigned int n, T obj);
};

///// Enemy Class

enum{
	LIN_MOVEMENT = 0,
	SIN_MOVEMENT,
};

enum{
	NOT_ANIMATED = 0,	// frames of object doesn't change
	CICLED_ANIMATED,	// frames changes by circle
    PINPONG_ANIMATED,   // ����������� ��� ��������: ��������� ������ ������, ����� ��������� ������ ������ ... 
	NORMAL_ANIMATED,	// 3 frames, see below
};

// frames of NORMAL_ANIMATED_OBJECTs
enum{
	CENTER_MOVE_FRAME = 0,
	RIGHT_MOVE_FRAME,
	LEFT_MOVE_FRAME,
};

enum{
	NOT_FIRE = 0,
	FIRE_DOWN_DIRECTION,
	FIRE_ANY_DIRECTION,
	FIRE_AS_TURRET,
};

enum{
	NOT_LINKED = 0,
	DAUGHTER_OBJECT,
	MOTHER_OBJECT, 
};

// enumeration of turret frames
enum{
	TURRET_FRAME_UP = 0,
	TURRET_FRAME_UP_RIGHT,
	TURRET_FRAME_RIGHT,
	TURRET_FRAME_DOWN_RIGHT,
	TURRET_FRAME_DOWN,
	TURRET_FRAME_DOWN_LEFT,
	TURRET_FRAME_LEFT,
	TURRET_FRAME_UP_LEFT,
};

// mask of allowed turret directions
enum{
	TURRET_MASK_UP = (1 << TURRET_FRAME_UP),
	TURRET_MASK_UP_RIGHT = (1 << TURRET_FRAME_UP_RIGHT),
	TURRET_MASK_RIGHT = (1 << TURRET_FRAME_RIGHT),
	TURRET_MASK_DOWN_RIGHT = (1 << TURRET_FRAME_DOWN_RIGHT),
	TURRET_MASK_DOWN = (1 << TURRET_FRAME_DOWN),
	TURRET_MASK_DOWN_LEFT = (1 << TURRET_FRAME_DOWN_LEFT),
	TURRET_MASK_LEFT = (1 << TURRET_FRAME_LEFT),
	TURRET_MASK_UP_LEFT = (1 << TURRET_FRAME_UP_LEFT),
	TURRET_MASK_ALL = 0xFF,
};

#define CENTER_SPEED_X_THRESHOLD	20.0f
#define MAX_FIRE_PROBABILITY        10  // ������������ �������� ����������� �������� ���� 

/////
class CEnemy
{
private:

	float	x;
	float	y;
	float	vx;
	float	vy;

	// trajectory variables (sin movement)
	float xAxis;  // x coordinate of oscillation center
	float xAccel; // acceleration by x axis
	float xAmp;	  // sin amplitude
	float vxMax;  // speed when obj crosses the axis
	float vxMin;  // speed when obj changes x-direction
    unsigned int score; // ���-�� �����, ������� ����������� �� ����������� ���������� 

	enum{
		PLUS_HALF_WAVE = 0,
		MINUS_HALF_WAVE,
		VX_RISING_PHASE,
		VX_FALLING_PHASE,
	};

	unsigned char	trajectoryType;
	unsigned char	halfWave;
	unsigned char	vxPhase;
	// end of trajectory variables

	bool	isFlashing;			// flag of flashing after hit
	float	flashStartTime;		// when enemy is hitted
	float	flashPeriod;		// interval of flashing after enemy was hitted

	bool	playDeathFlag;		// status of waiting for set alive to dead after critical hit got
	float	deathTime;			// time after enemy is alive after kill
	float	deathStartTime;		// moment of kill
	int		fireType;			// type of shooting
	float	fireInterval;		// time between 2 shoots
	float	fireStartPeriod;	// time stamp for counting fire interval
	int		missleNumber;
    unsigned char fireProbability; // ����������� �������� ���� 

	// link variables
	unsigned char linkType;
	static bool isMotherObjectActive;
	
	// turret property
	unsigned char turretAllowedDirections;

	////////////
	unsigned int	width;
	unsigned int	height;
	unsigned int	currentFrame; 
    unsigned char   cycledAnimatePhase; // ���� �������� (��������� ������ ������ ��� ���������). ��� �������� ���� ���-����
	unsigned int	animateType;		//
	float			animateInterval;	// animation parameters
	float			startAnimPeriod;	// 
	int		hitPoints;
	int		crossingDamage;
	bool	isVisible;
	bool	isActive;
	
	CSprite	image;
	CCrossMask mask;
	CFramedObject *pFrames;

    float    smokeEffectStart;          // ������ ������� ��������� ���������� ���������� ������� ������� ����
    int hitPointsThreshold;             // ����� �������� hit point'��, ���� �������� ���������� ��������� ����
    float    smokeInterval;             // �������� �������, ������ ������� ����� ������������� ����� ������ 
    int      xSmokeOffset;              // �������� ���������� X ����� ��������� ���� ������������ X ������ ������� CEnemy 
    int      ySmokeOffset;              // �������� ���������� Y ����� ��������� ���� ������������ Y ������ ������� CEnemy 

public:
	
	CEnemy();
	void	init(float xf, float yf, int tType, float vxmax, float vxmin, float xamp, float vyf);
	void	uninit();
	void	setImage(CSprite img);
	void    setMask(CCrossMask mask);
	CCrossMask getMask();
	void	setCurrentFrame(unsigned int frame);
	void	setFrames(CFramedObject *fobj);
	void	setAnimateType(unsigned int aType);
	void	setAnimateInterval(float aInterval);
	RECT	getCurrentFrame();
	int		getX0();
	int		getY0();
	int		getX();
	int		getY(); 
    int     getSmokeX();    // ������ ��� ��������� ���������� X ����� ��������� ����
    int     getSmokeY();    // ������ ��� ��������� ���������� Y ����� ��������� ����

	void	setHitPoints(int h);
	void	setCrossingDamage(int dmg);
	int		getCrossingDamage();
	void	setFireType(int fType);
	int		getFireType();
	void	setFireInterval(float fInterval);
	float	getFireInterval();
	float	getLastFireTime();
	void	setLastFireTime();
    void    correctLastFireTime(float time);
    void    setFireProbability(unsigned int fireProbability); // ��������� ����������� ������� ����� � ���������� �������� (] {0 - 10})
    unsigned int getFireProbability(); // ������ ��� ��������� �������� ����������� 
	void	setMissleNumber(int n);
	int		getMissleNumber();

	void	setHit(int hitValue);
	void	setDeathTime(float t);
	bool	isEnemyAlive();
	bool	isPlayDeath();
	void	setLinkType(unsigned char linkType);
	void	setTurretDirections(unsigned char turretAllowedDirections);
	unsigned char getTurretDirections();
    
    // ����� ����������� ��������� �� ������ ������� ����: 
    void    setSmokeControl(int hitPointsThreshold, float smokeInterval, int xSmokeOffset, int ySmokeOffset);
    bool    processSmoke(CTimer *timer);    // ����� ����������/���������� ��������� ������ ���������� ������� ������� ���� 

    void    setScore(unsigned int score);
    unsigned int getScore();

	void	processMovement(float timeDelta);
	int 	draw();
    void    kill();
};


///// Player Class

enum{
	MOVE_UP		= 0x01,
	MOVE_DOWN	= 0x02,
	MOVE_LEFT	= 0x04,
	MOVE_RIGHT	= 0x08,
};

enum{
	PLAYER_FRAME_NORMAL = 0,
	PLAYER_FRAME_RIGHT,
	PLAYER_FRAME_LEFT,
};

const unsigned int PLAYER_MAX_SCORE = 1000000;
const unsigned int PLAYER_MAX_LIVES = 15;

class CPlayer
{
private:

	float	x;
	float	y;
	float	x0;
	float	y0;
	float	vx;
	float	vy;
	unsigned char direction;

	unsigned int width;
	unsigned int height;
	unsigned int currentFrame;
	bool	isActive;
	bool	isVisible;

	// damage variables
	int		hitPoints;
	int		crossingDamage;
	bool	playDeathFlag;
	float	deathStartTime;
	float	deathTime;
	int		lives;
    int     maxLives;

	// interval variables
	float	missleInterval;
	float	lastMissleTime;
	bool	isInitialBlinking;	// 
	float   blinkInterval;		// initial blinking
	float	startBlinkInterval;	// parameters
	int		blinkCounter;		//

    unsigned int score;

	CSprite image;
	CCrossMask mask;
	CFramedObject *pFrames;
	
public:

	CPlayer();
    void	init(float xf, float yf, float vxf, float vyf);
	void	setMissleInterval(float mInterval);
	void	setLastMissleTime(float mTime);
	bool	isReadyToFire(float currentTime);
	int		getX0();
	int		getY0();
	float	getX();
	float	getY();
	unsigned int getWidth();
	unsigned int getHeight();
	void	processMovement(float timeDelta);
	void	setDirection(unsigned char dir);
	void	setImage(CSprite img);
	void    setMask(CCrossMask mask);
	CCrossMask getMask();
	void	setFrames(CFramedObject *fobj);
	RECT	getCurrentFrame();

	void	setCrossingDamage(int dmg);
	void	setHitPoints(int hp);
	void	setDeathTime(float t);
	void	setHit(int hitValue);
	int		getCrossingDamage();
	int		getHitPoints();
	int		getLivesNumber();
    void    addLife();
	bool	isPlayDeath();
	bool	isBlinking();
	bool	isPlayerActive();

    void    clearScore();
    void    incrementScore(unsigned int score);
    unsigned int getScore();
    int     getMaxLives();
    void    setLives(int maxLives);

	int 	draw();
};


///// Missle Class

enum{
	AGAINST_PLAYER = 0,
	AGAINST_ENEMY,
};

class CMissle
{
private:

	float	x;
	float	y;
	float	vx;
	float	vy;
	float	speedValue;

	unsigned int width;
	unsigned int height;
	unsigned int currentFrame;
	int		damage;
	float	animateInterval;	// animation parameters
	float	startAnimPeriod;	// 
	int		target;
	bool	isActive;

	CSprite image;
	CCrossMask mask;
	CFramedObject *pFrames;

public:

	CMissle();
	void	init(float xf, float yf);
	void	processMovement(float timeDelta);
	void	setImage(CSprite img);
	void	setMask(CCrossMask mask);
	CCrossMask getMask();
	void	setFrames(CFramedObject *fobj);
	RECT	getCurrentFrame();
	bool	isMissleAlive();
	void	setAliveStatus(bool aStatus);
	void	setTargetType(int targ);	// set target type
	int		getTargetType();			// who is a target: player or enemy?
	void	setAnimateInterval(float aInterval);
	int		getDamage();
	int		getX0();
	int		getY0();
	void	setDamage(int d);
	void	setSpeed(float vx, float vy);
	void	setSpeedValue(float sValue);
	float	getSpeedValue();
	int 	draw();
    void    kill();
};

///// Effect class
class CEffect
{
private:

	float	x;
	float	y;
	float	vx;
	float	vy;

	float	timeDelay;			// delay for start the effect
	float	timeStarted;		// time value of start the effect
	float	framePeriod;		// = 1 / animation_speed
	float	currentFrameStart;	// time value of current frame begin
	unsigned int nRepetition;	// number of iterations
	unsigned int currentRepetition;
	unsigned int width;
	unsigned int height;
	unsigned int currentFrame;
	bool	isActive;
	bool	isVisible;

	CSprite image;
	CFramedObject *pFrames;

public:

	CEffect();
	void	init(float xf, float yf, float vxf, float vyf, float tDelay, unsigned int rep);
	bool	isAlive();
	void	processEffect(float timeDelta);
	void	setImage(CSprite img);
	void	setFrames(CFramedObject *fobj);
	void	setFramePeriod(float fPeriod);
	int 	draw();
    void    kill();
};


///// Star class
class CStar
{
private:

	float	x;
	float	y;
	float	vy;
	unsigned int	width;
	unsigned int	height;
	unsigned int	currentFrame;
	bool	isActive;

	CSprite image;
	CFramedObject *pFrames;

public:

	CStar();
	void	setImage(CSprite img);
	void	setFrames(CFramedObject *fobj);
	void	init(unsigned int n, float xf, float yf, float vy);
	void	processMovement(float timeDelta);
	bool	isAlive();
	int 	draw();
};


///// 
class CNumbers
{
private:

    CSprite img;
    int x0, y0;
    int sizeX, sizeY;

public:

    void init(CSprite img, int x0, int y0, int sizeX, int sizeY);
    void drawDigit(int x, int y, unsigned char number);
    void draw(int x, int y, unsigned char digits, unsigned int value);
};


/////////////////
/////////////////

const unsigned int MAX_OBJECTS_IN_ENTRY     = 50;  // ������������ ���-�� �������� ����� ������ � ������ 
const unsigned int MAX_LEVEL_ENTRIES        = 100; // ������������ ���-�� ������� � ������ 
const unsigned int MAX_FILE_NAME_LENGTH     = 200; // ������������ ���-�� �������� � ����� ������ 
const unsigned int MAX_LEVELS               = 3;   // ������������ ���-�� ������� � ���� 
/////
class CLevelEntry
{
public:

    float	timeWait;       // ����� ��������, ����� �������� ��������� ������ ����� ��������� � ����
    unsigned int amount;    // ���-�� �������� � ������� ������

    struct _levelObject
    {
        int		type;       // ��� ������� CEnemy
        float	x;          // ��������� ��������� �� X
        float	y;          // ��������� ��������� �� Y
        int		tType;      // ��� ���������� (�������� / ��������������)
        float	vxMax;      // ������������ �������� �� X (��� ��������. ����������) / �������� �� X (��� �������� ����������)
        float	vxMin;      // ����������� �������� �� X
        float	xAmp;       // ��������� ����������� �� X 
        float	vy;         // �������� �� ��� Y 
        unsigned char tDir; // ��� ��������� ����������� ���������� ��� ��������, ���� ��������� - ������� � 8-� ������������� 
        unsigned char lStat;// ������ ���������� (��� ���������� / �������� ������ / ����������� ������)
    } levelObject[MAX_OBJECTS_IN_ENTRY]; // ������ �������� ������� ������ ������ 

};

/////
class CLevel
{
public:

    unsigned int readedEntries;     // ���-�� ����������� ������� �� ����� ������
    unsigned int currentEntry;      // ������� ������� ��� ����
    unsigned int currentLevel;      // ������� ������� ��� ���� 
    float   lastEntryTime;          // ������� ������� ��������� ��������� ������ �������� �� ������
    char    stringName[20];         // ������ ��� �������� �������� ������

    CLevel();
    CLevelEntry entry[MAX_LEVEL_ENTRIES];
    int load(char fileName[MAX_FILE_NAME_LENGTH]);
};

/////
class CLevelList
{
public:

    char fileName[MAX_LEVELS][MAX_FILE_NAME_LENGTH]; // �������� ���� ������ ������� 
    unsigned int addedLevels;
    unsigned int currentLevel;

    CLevelList();
    void add(char fileName[MAX_FILE_NAME_LENGTH]);
};

/////////////////
/////////////////


/////
enum{
    ALIVE_TILE = 0,
    DEAD_TILE,
};

// ����� ���������� ������
class CLifeIndicator
{
private:
    
    RECT aliveRect;
    RECT deadRect;
    CSprite img;

public:

    void init(int x1, int x2, int y1, int y2, unsigned char status, CSprite img);
    void draw(int x, int y, unsigned char status);
};

/////////////////
/////////////////


enum{
    MENU_EXIT_TO_SYSTEM = 0,    // ������ ������������� ������ � �������
    MENU_EXIT_TO_MAIN,          // ������ ������������� �������� � ���������� ������
    MENU_CONTINUE,              // [�����] ���������� ����� ����� ���� 
    MENU_AMOUNT,
};

const unsigned int MAX_MENU_ITEMS = 10; // ������������ ���-�� ������ � ���� 

class CGameMenu
{
private:

    unsigned int itemsAmount;       // ���-�� ������� � ������� ���� 
    unsigned int currentItem;       // ������� �����, ��������� ���������� 
    RECT r;                         // ����� ��������� � ������ 

    struct _itemCoord{              // ���������� ��������� ��������� ��� ������ ������� ���� 
        int x;
        int y;
        unsigned int width;         // ������ ������� ��������� (������ ��������� ������ � ��������) 
    } itemCoord[MAX_MENU_ITEMS];

public:
    
    static float changeItemDelay;   // �������� ����� �������� � ������ ������ ���� 
    static float itemSelectTime;    // ������� ������� ������ �������� ������ ���� 
    static CSprite img;             // ����� � �����������(-���) ��� ����������� ���� 

   
    CGameMenu();
    void addItem(int x, int y, unsigned int width);
    void setSelectorRect(int x1, int x2, int y1, int y2);
    void selectNextItem(float time);    // ����� ���������� ������ ����
    void selectPrevItem(float time);    // ����� ����������� ������ ���� 
    void drawSelector();                // ��������� ���������
    unsigned int getSelected();         // ��������� ������ �������� ���������� ������ ����
    void setSelected(unsigned int n);   // ��������� �������� ���������� ������ ����
};

/////////////////
/////////////////

enum{
    RISING_PHASE = 0,
    FALLING_PHASE,
};

class CColorFlash
{
private:

    unsigned char r1, g1, b1;   // ���������� ����� ��������� ����
    unsigned char r2, g2, b2;   // ���������� ����� �������� ����
    unsigned char r, g, b;      // ���������� ����� � ������� ������ �������
    float timePhaseStart;       // ������� ������� ������ �������� ����
    float period;
    unsigned char phase;        // ����

public:

    CColorFlash();
    void init(unsigned int color1, unsigned int color2, float period, float time);
    void process(float time);
    unsigned char getRed();
    unsigned char getGreen();
    unsigned char getBlue();
};


/////////////////////////////////
//	Members of GameObjectList  //

//////// constructor
template <class T, unsigned int size>
GameObjectList<T, size>::GameObjectList()
{
	currentElement = 0;
	empty = true;

	if (!(pObject = new T[size]))
		MessageBox(NULL, L"Can't allocate GameObjectList", L"Error!", MB_OK);
}

//////// destructor
template <class T, unsigned int size>
GameObjectList<T, size>::~GameObjectList(void)
{
	empty = true;
	delete pObject;
}

//////// adding new object
template <class T, unsigned int size>
void GameObjectList<T, size>::addObject(T obj)
{
	if (currentElement < size)
	{
		currentElement++;
		pObject[currentElement - 1] = obj;

	}
	else
		MessageBox(NULL, L"Can't add new object to list", L"Error!", MB_OK);

	empty = false;
}

//////// removeing new object
template <class T, unsigned int size>
void GameObjectList<T, size>::removeObject(unsigned int n)
{
	//	unsigned int i;

	if (n < size)
	{
		//		for (i = n; i < currentElement; i++)
		//			pObject[i] = pObject[i + 1];
		//
		memcpy(&pObject[n], &pObject[n + 1], (currentElement - n - 1)*sizeof(T));
		currentElement--;
	}
	else
		MessageBox(NULL, L"Can't remove this object (wrong number)", L"Error!", MB_OK);
}

//////// getting number of added elements
template <class T, unsigned int size>
unsigned int GameObjectList<T, size>::getElementsNumber()
{
	if (empty == true)
		return 0;
	else
		return currentElement;
}

//////// 
template <class T, unsigned int size>
T GameObjectList<T, size>::getObject(unsigned int n)
{
	T emptyElement;
	memset(&emptyElement, 0, sizeof(T));

	if (empty == true)
	{
		MessageBox(NULL, L"No element(s)", L"Error!", MB_OK);
		return emptyElement;
	}
	else
	{
		if (n > currentElement)
		{
			MessageBox(NULL, L"Wrong number of element", L"Error!", MB_OK);
			return emptyElement;
		}
		else
			return pObject[n];
	}

}

//////// 
template <class T, unsigned int size>
int GameObjectList<T, size>::reloadObject(unsigned int n, T obj)
{
	if (empty == true)
	{
		MessageBox(NULL, L"No element(s)", L"Error!", MB_OK);
		return 0;
	}
	else
	{
		if (n > currentElement)
		{
			MessageBox(NULL, L"Wrong number of element", L"Error!", MB_OK);
			return 0;
		}

		pObject[n] = obj;
		return 1;
	}

}