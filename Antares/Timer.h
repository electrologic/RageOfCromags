
class CTimer{

	private:

		float time;
		float deltaTime;
		unsigned int numberFPS; // old
        unsigned int fps;
        float lastTime;

	public:

        CTimer();
		void setTime(float time);
		void setDeltaTime(float delta);
		void setFPS(unsigned int fps); // old
        void incrementFps();
        void clearFps();
        unsigned int getFps();
        void setLastTime(float lastTime);
        float getLastTime();

		float getTime();
		float getDeltaTime();
		unsigned int getFPS(); // old
};