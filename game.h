
class CGame{

	public:

		void init(void);
		void uninit(void);
		void render(void);
		void controller(void);

};


/////
enum{
    PLAY = 0,
    PAUSE,
    ASK_TO_ABORT,       // ��������� ������� �������� ���� � ����� � ������� ����
    LIFE_OVER,          // ����� ���������
    SHOW_GAME_OVER,     // ����������� ��������� � ����� ����
    MAIN_TITLE,         // ����������� ���������� ������ ����
    READY_TO_START,     // ����������� ���������� ������ ���� � ���������� ������ ����� ����
    SHOW_LEVEL_NUMBER,  // ����������� ������ ������ 
    ASK_TO_EXIT,        // ��������� ������� ����� �� ���� � �������
    THE_END,            // ��������� ��������� ����� ����������� ���� ������� 
};


class CGameView
{
private:

    int state;
    float pauseStartTime;
    float pauseDuration;
    float timeNewPhase;     // ������� ������� � ������ ����� ����
    bool  gameStartError;   // ���� ������ ��� ������� ����
    CTimer *timer;

public:

    CGameView(CTimer *timer);
    float getPauseDuration();
    void setState(int state);
    int getState();
    bool getGameStartError();    // ��������� �������� ����� �������� ������ ��� ������� ���� 
    void setGameStartError(bool isError); // ��������� ����� ������ ������� ���� 
    // ������ � ������ ��� ������ � �������� ������� � ����� ����
    float getTimeNewPhase();
    void setTimeNewPhase(float timeNewPhase);
    void prepareForPlayLevel(float time); // ��������� �������� �������� ����� ������� ������
};

