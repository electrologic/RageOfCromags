
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
    ASK_TO_ABORT,       // состо€ние запроса сбросить игру и выйти в главное меню
    LIFE_OVER,          // игрок уничтожен
    SHOW_GAME_OVER,     // отображение сообщени€ о конце игры
    MAIN_TITLE,         // отображение заглавного экрана игры
    READY_TO_START,     // отображение заглавного экрана игры и готовность начать новую игру
    SHOW_LEVEL_NUMBER,  // отображение номера уровн€ 
    ASK_TO_EXIT,        // состо€ние запроса выйти из игры в систему
    THE_END,            // финальное состо€ние после прохождени€ всех уровней 
};


class CGameView
{
private:

    int state;
    float pauseStartTime;
    float pauseDuration;
    float timeNewPhase;     // отметка времени о начале новой фазы
    bool  gameStartError;   // флаг ошибки при запуске игры
    CTimer *timer;

public:

    CGameView(CTimer *timer);
    float getPauseDuration();
    void setState(int state);
    int getState();
    bool getGameStartError();    // получение значени€ флага признака ошибки при запуске игры 
    void setGameStartError(bool isError); // установка флага ошибки запуска игры 
    // геттер и сеттер дл€ работы с отметкой времени о новой фазе
    float getTimeNewPhase();
    void setTimeNewPhase(float timeNewPhase);
    void prepareForPlayLevel(float time); // установка исходных значений перед началом уровн€
};

