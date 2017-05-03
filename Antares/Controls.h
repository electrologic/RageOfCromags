
/////////////////////////////////////
class CMouse{

	private:

		int x;
		int y;
		bool leftButtonState;

	public:

		void setCoordinate(int mouseX, int mouseY); // sould be call from winmain module
		void setLeftButtonPressed(bool state);

		int getX();
		int getY();
		bool getLeftButtonPressed(void);

};


enum{
	UP_ARROW_BUTTON = 0,
	DOWN_ARROW_BUTTON,
	LEFT_ARROW_BUTTON,
	RIGHT_ARROW_BUTTON,
	Z_BUTTON,
	P_BUTTON,
    F_BUTTON,
	ESC_BUTTON,
//    SPACE_BUTTON,
    ENTER_BUTTON,

    MAX_USED_KEYS,
};

class CKeys{

private:

    bool keyArray[MAX_USED_KEYS];

public:

	void initKeyArray();
	bool isPressed(unsigned int n);
	void setPressed(unsigned int n, bool isPressed);
};

// класс для проверки и установки триггера выхода из программы 
class CExitControl{

private: 

    bool onExit;
    bool onPause;

public:

    CExitControl();
    void setState(bool onExit);
    bool getState();
    void setPause(bool pause);
    bool isPause();
};