#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif


#define MUNCHIECOUNT 100
#define GHOSTCOUNT 10
#define WALL 100
// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;



// structure definiton for the pacman 
struct Player
{
	float speedMulti;
	float speed;
	int frameTime;
	int direction;
	int frame;
	int score;
	bool dead;
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
};

struct Enemy
{
	int eFrameCount;
	Rect* eRect;
	Texture2D* eTexture;
	Vector2* ePosition;
	int eFrame;
	int eFrameTime;
	bool eaten;
	bool colliding;

};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
	int frame;
	int frameTime;
};
struct Menus
{
	Texture2D* background;
	Rect* rectangle;
	Vector2* stringPosition;

};
struct Wall
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:


	//input methods, mouse and keyboard
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	//check methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckStart(Input::KeyboardState* state, Input::Keys startKey);
	void CheckViewportCollision();
	void CheckGhostCollision();
	void CheckMunchieCollision();
	void CheckWallCollision();

	//update methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchies(Enemy*, int elapsedTime);
	void UpdateGhosts(MovingEnemy*, int elapsedTime);
	// Data to represent Pacman
	//Vector2* _pacmanPosition;
	//Rect* _pacmanSourceRect;
	//Texture2D* _pacmanTexture;

	// Data to represent Munchie
	
	Enemy* _munchie[MUNCHIECOUNT];

	MovingEnemy* _ghost[GHOSTCOUNT];

	Wall* _wall[WALL];
	
	//data for the cherry
	Enemy* _cherry;
	// Position for pause String for menu text
	Vector2* _stringPosition;
	
	//pacman animation speed
	const int _cPacmanFrameTime;
	//the munchies const time
	const int cMunchieFrameTime;
	const int _cGhostFrameTime;
	
	//pacman data 
	Player* _pacman;
	
	//data for the pause menu
	Menus* _menu;
	/*
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	*/

	//sound effect pointer
	SoundEffect* _pop;

	bool _paused;
	bool _gameStart;
	//to check if p has been pressed
	bool _pKeyDown;
	//to check if space has been pressed
	bool _spacePressed;
	//to update all munchies
	

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);


};