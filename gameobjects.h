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
    PINPONG_ANIMATED,   // чередование фаз анимации: повышение номера кадров, затем понижение номера кадров ... 
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
#define MAX_FIRE_PROBABILITY        10  // максимальное значение вероятности открытия огня 

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
    unsigned int score; // кол-во очков, которые начисляются за уничтожение противника 

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
    unsigned char fireProbability; // вероятность открытия огня 

	// link variables
	unsigned char linkType;
	static bool isMotherObjectActive;
	
	// turret property
	unsigned char turretAllowedDirections;

	////////////
	unsigned int	width;
	unsigned int	height;
	unsigned int	currentFrame; 
    unsigned char   cycledAnimatePhase; // фаза анимации (повышение номера кадров или понижение). Для анимации типа пин-понг
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

    float    smokeEffectStart;          // момент времени генерации очередного экземпляра объекта эффекта дыма
    int hitPointsThreshold;             // порог значения hit point'ов, ниже которого происходит генерация дыма
    float    smokeInterval;             // интервал времени, спустя которое можно сгенерировать новый объект 
    int      xSmokeOffset;              // смещение координата X точки генерации дыма относительно X центра объекта CEnemy 
    int      ySmokeOffset;              // смещение координата Y точки генерации дыма относительно Y центра объекта CEnemy 

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
    int     getSmokeX();    // геттер для получения координаты X точки генерации дыма
    int     getSmokeY();    // геттер для получения координаты Y точки генерации дыма

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
    void    setFireProbability(unsigned int fireProbability); // установка вероятности открыть огонь в подходящей ситуации (] {0 - 10})
    unsigned int getFireProbability(); // геттер для получения значения вероятности 
	void	setMissleNumber(int n);
	int		getMissleNumber();

	void	setHit(int hitValue);
	void	setDeathTime(float t);
	bool	isEnemyAlive();
	bool	isPlayDeath();
	void	setLinkType(unsigned char linkType);
	void	setTurretDirections(unsigned char turretAllowedDirections);
	unsigned char getTurretDirections();
    
    // метод определения указателя на объект эффекта дыма: 
    void    setSmokeControl(int hitPointsThreshold, float smokeInterval, int xSmokeOffset, int ySmokeOffset);
    bool    processSmoke(CTimer *timer);    // метод разрешения/запрещения генерации нового экземпляра объекта эффекта дыма 

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

const unsigned int MAX_OBJECTS_IN_ENTRY     = 50;  // максимальное кол-во объектов одной записи в уровне 
const unsigned int MAX_LEVEL_ENTRIES        = 100; // максимальное кол-во записей в уровне 
const unsigned int MAX_FILE_NAME_LENGTH     = 200; // максимальное кол-во символов в файле уровня 
const unsigned int MAX_LEVELS               = 3;   // максимальное кол-во уровней в игре 
/////
class CLevelEntry
{
public:

    float	timeWait;       // время ожидания, после которого параметры записи будут загружены в игру
    unsigned int amount;    // кол-во объектов в текущей записи

    struct _levelObject
    {
        int		type;       // тип объекта CEnemy
        float	x;          // начальная координат по X
        float	y;          // начальная координат по Y
        int		tType;      // тип траектории (линейная / синусоидальная)
        float	vxMax;      // максимальная скорость по X (для синусоид. траектории) / скорость по X (для линейной траектории)
        float	vxMin;      // минимальная скорость по X
        float	xAmp;       // амплитуда перемещения по X 
        float	vy;         // скорость по оси Y 
        unsigned char tDir; // код доступных направлений направлени для стрельбы, если противник - туррель с 8-ю направлениями 
        unsigned char lStat;// статус соединения (нет соединения / дочерний объект / материнский объект)
    } levelObject[MAX_OBJECTS_IN_ENTRY]; // массив объектов текущей записи уровня 

};

/////
class CLevel
{
public:

    unsigned int readedEntries;     // кол-во прочитанных записей из файла уровня
    unsigned int currentEntry;      // счетчик записей для игры
    unsigned int currentLevel;      // счетчик уровней для игры 
    float   lastEntryTime;          // отметка времени активации последней записи объектов из уровня
    char    stringName[20];         // строка для хранения названия уровня

    CLevel();
    CLevelEntry entry[MAX_LEVEL_ENTRIES];
    int load(char fileName[MAX_FILE_NAME_LENGTH]);
};

/////
class CLevelList
{
public:

    char fileName[MAX_LEVELS][MAX_FILE_NAME_LENGTH]; // перечень имен файлов уровней 
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

// класс индикатора жизней
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
    MENU_EXIT_TO_SYSTEM = 0,    // диалог подтверждения выхода в систему
    MENU_EXIT_TO_MAIN,          // диалог подтверждения перехода к начальному экрану
    MENU_CONTINUE,              // [пункт] продолжить после конца игры 
    MENU_AMOUNT,
};

const unsigned int MAX_MENU_ITEMS = 10; // максимальное кол-во пунтов в меню 

class CGameMenu
{
private:

    unsigned int itemsAmount;       // кол-во пунктов в текущем меню 
    unsigned int currentItem;       // текущий пункт, выбранный селектором 
    RECT r;                         // рамка селектора в растре 

    struct _itemCoord{              // координаты положений селектора для разных пунктов меню 
        int x;
        int y;
        unsigned int width;         // ширина области выделения (ширина текстовой строки в пикселях) 
    } itemCoord[MAX_MENU_ITEMS];

public:
    
    static float changeItemDelay;   // задержка после перехода к новому пункту меню 
    static float itemSelectTime;    // отметка времени выбора текущего пункта меню 
    static CSprite img;             // растр с изображенем(-ями) для отображения меню 

   
    CGameMenu();
    void addItem(int x, int y, unsigned int width);
    void setSelectorRect(int x1, int x2, int y1, int y2);
    void selectNextItem(float time);    // выбор следующего пункта меню
    void selectPrevItem(float time);    // выбор предыдущего пункта меню 
    void drawSelector();                // отрисовка селектора
    unsigned int getSelected();         // получение номера текущего выбранного пункта меню
    void setSelected(unsigned int n);   // установка текущего выбранного пункта меню
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

    unsigned char r1, g1, b1;   // компоненты цвета начальной фазы
    unsigned char r2, g2, b2;   // компоненты цвета конечной фазы
    unsigned char r, g, b;      // компоненты цвета в текущий момент времени
    float timePhaseStart;       // отметка времени начала цветовой фазы
    float period;
    unsigned char phase;        // фаза

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