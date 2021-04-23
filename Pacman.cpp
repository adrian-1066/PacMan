#include "Pacman.h"

#include <sstream>

#include <iostream>

#include<fstream>

#include <time.h>




Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), cMunchieFrameTime(500), _cPacmanFrameTime(250), _cGhostFrameTime(200)
{
	
	_menu = new Menus();
	//pacman struct data
	_pacman = new Player();
	_pacman->dead = false;
	_pacman->score = 0;
	_pacman->direction = 0;
	_pacman->frameTime = 0;
	_pacman->frame = 0;
	_pacman->speed = 0.2f;
	_pacman->speedMulti = 1.0f;

	int i;

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i] = new Enemy();
		_munchie[i]->eaten = false;
		_munchie[i]->colliding = false;
		_munchie[i]->eFrameCount = rand() % 1;
		_munchie[i]->eFrame = 0;
		_munchie[i]->eFrameTime = rand() % 60 + 50;
	}

	for (int g = 0; g < GHOSTCOUNT; g++)
	{
		_ghost[g] = new MovingEnemy();
		_ghost[g]->direction = 0;
		_ghost[g]->speed = 0.2f;
		_ghost[g]->frameTime = 0;
		_ghost[g]->frame = 0;
	}
	_paused = false;
	_gameStart = true;
	_spacePressed = false;
	
	//initialised pop sound effect
	_pop = new SoundEffect();

	_cherry = new Enemy();

	for(int w = 0; w < WALL; w++)
	{
		_wall[w] = new Wall();
	}

	// to initialise the audio
	Audio::Initialise();
	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1920, 1080, false, 25, 25, "Pacman", 60);
	Input::Initialise();
	// to initialise the audio
	if (!Audio::IsInitialised())
	{
		std::cout << "audio is not initialised" << std::endl;
	}


	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}



Pacman::~Pacman()
{
	int i;
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchie[i]->eTexture;
		delete _munchie[i]->eRect;
		delete _munchie[i]->ePosition;
		delete _munchie[i];

	}
	for (int g = 0; g < GHOSTCOUNT; g++)
	{
		delete _ghost[g]->texture;
		delete _ghost[g]->sourceRect;
		delete _ghost[g]->position;
		delete _ghost[g];
	}
	for (int w = 0; 0 < WALL; w++)
	{
		delete _wall[w]->texture;
		delete _wall[w]->sourceRect;
		delete _wall[w]->position;
	}
	delete[] _wall;
	delete[] _ghost;
	delete[] _munchie;
	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _cherry->eTexture;
	delete _cherry->eRect;
	delete _pop;
}



void Pacman::LoadContent()
{
	srand(time(NULL));
	
	//load the start menu
	_menu->background = new Texture2D();
	_menu->background->Load("Textures/Transparency.png", true);
	_menu->rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
	

	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(32.0f, 64.0f, 32,32); // 0 0 is moving right, 32 32 is down. 0 64 is left

	//load the cherry
	_cherry->eTexture = new Texture2D();
	_cherry->eTexture->Load("Textures/Cherry.png", true);
	_cherry->ePosition = new Vector2(500.0f, 500.0f);
	_cherry->eRect = new Rect(0.0f, 0.0f, 32, 32);

	//to try and draw multiple
	
	//to load the pop sound
	_pop->Load("Sounds/pop.wav");

	if (!_pop->IsLoaded())
	{
		std::cout << "_pop member sound effect is not loaded" << std::endl;
	}
	// Load Munchie
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", false);
	int i;
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->eTexture = new Texture2D();
		_munchie[i]->eTexture = munchieTex;
		_munchie[i]->ePosition = new Vector2((rand() % Graphics::GetViewportWidth()) + NULL, (rand() % Graphics::GetViewportHeight()) + NULL);
		_munchie[i]->eRect = new Rect(0.0f, 0.0f, 12, 12);
	}
	//load the ghost
	for (int g = 0; g < GHOSTCOUNT; g++)
	{
		_ghost[g]->texture = new Texture2D();
		_ghost[g]->texture->Load("Textures/GhostBlue.png", false);
		_ghost[g]->position = new Vector2((rand() % Graphics::GetViewportWidth()) + NULL, (rand() % Graphics::GetViewportHeight()) + NULL);
		_ghost[g]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);

	}
	//load the walls
	ifstream inWall("wall_locations.txt");
	if (inWall.is_open())
	{
		int pos1;
		int pos2;
		int w = 0;
		while (inWall >> pos1 >> pos2)
		{
			_wall[w]->texture = new Texture2D();
			_wall[w]->texture->Load("Textures/Wall.png", false);
			_wall[w]->sourceRect = new Rect(0.0f, 0.0f, 12, 32);
			_wall[w]->position = new Vector2(pos2,pos1);
			w++;
		}
	}
	else
	{
		std::cout << "file could not be opened";
	}
	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
	
}
//to implement the input methods
void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	float pacmanSpeed = _pacman->speed * elapsedTime * _pacman->speedMulti;
	if (state->IsKeyDown(Input::Keys::D))
	{
		_pacman->position->X += pacmanSpeed; //Moves Pacman across X axis
		_pacman->frameTime += elapsedTime;
		_pacman->direction = 0;
		//_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	}
	else if (state->IsKeyDown(Input::Keys::A))
	{
		_pacman->position->X += -pacmanSpeed; //Moves Pacman across X axis left
		_pacman->frameTime += elapsedTime;
		_pacman->direction = 2;
		//_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;

	}
	else if (state->IsKeyDown(Input::Keys::W))
	{
		_pacman->position->Y += -pacmanSpeed;
		_pacman->frameTime += elapsedTime;
		_pacman->direction = 3;
		//_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	}
	else if (state->IsKeyDown(Input::Keys::S))
	{
		_pacman->position->Y += pacmanSpeed;
		_pacman->frameTime += elapsedTime;
		_pacman->direction = 1;
		//_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;

	}
	if (state->IsKeyDown(Input::Keys::R))
	{
		_cherry->ePosition->X = (rand() % Graphics::GetViewportWidth()) + NULL + 50;
		_cherry->ePosition->Y = (rand() % Graphics::GetViewportHeight()) + NULL + 50;
	}
	if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		_pacman->speedMulti = 2.0f;
	}
	else
	{
		_pacman->speedMulti = 1.0f;
	}

	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherry->ePosition->X = mouseState->X;
		_cherry->ePosition->Y = mouseState->Y;

	}


}
//
void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;

	}
	if (state->IsKeyUp(Input::Keys::P))
	{
		_pKeyDown = false;



	}


}

//
void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey)
{
	if (_spacePressed == true)
	{
	}
	else
	{
		//to start the game
		if (state->IsKeyDown(Input::Keys::SPACE))
		{

			_gameStart = !_gameStart;
			_spacePressed = true;

		}
	}

}

//
void Pacman::CheckViewportCollision()
{
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth())
	{
		_pacman->position->X = 32 - _pacman->sourceRect->Width;
	}

	if (_pacman->position->X + _pacman->sourceRect->Width < 32)
	{
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;
	}
	if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight())
	{
		_pacman->position->Y = 32 - _pacman->sourceRect->Height;
	}
	if (_pacman->position->Y + _pacman->sourceRect->Height < 32)
	{
		_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height;
	}


}

void Pacman::CheckGhostCollision()
{
	
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (int g = 0; g < GHOSTCOUNT; g++)
	{
		bottom2 = _ghost[g]->position->Y + _ghost[g]->sourceRect->Height;
		left2 = _ghost[g]->position->X;
		right2 = _ghost[g]->position->X + _ghost[g]->sourceRect->Width;
		top2 = _ghost[g]->position->Y;

		
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			g = GHOSTCOUNT;
		}
	}

}
void Pacman::CheckWallCollision()
{
	for (int g = 0; g < GHOSTCOUNT; g++)
	{
		int gBot = _ghost[g]->position->Y + _ghost[g]->sourceRect->Height;
		int wBot = 0;
		int gLeft = _ghost[g]->position->X;
		int wLeft = 0;
		int gRight = _ghost[g]->position->X + _ghost[g]->sourceRect->Width;
		int wRight = 0;
		int gTop = _ghost[g]->position->Y;
		int wTop = 0;
		for (int w = 0; w < WALL; w++)
		{
			int wBot = _wall[w]->position->Y + _wall[w]->sourceRect->Height;
			int wLeft = _wall[w]->position->X;
			int wRight = _wall[w]->position->X + _wall[w]->sourceRect->Width;
			int wTop = _wall[w]->position->Y;

			if ((gBot > wTop) && (gTop < wBot) && (gRight > wLeft) && (gLeft < wRight))
			{
				if (_ghost[g]->direction == 0)
				{
					_ghost[g]->direction = 1;
				}
				else if (_ghost[g]->direction == 1)
				{
					_ghost[g]->direction = 0;
				}
			}
		}
	}
}
void Pacman::CheckMunchieCollision()
{
	
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;
	for (int m = 0; m < MUNCHIECOUNT; m++)
	{
		bottom2 = _munchie[m]->ePosition->Y + _munchie[m]->eRect->Height;
		left2 = _munchie[m]->ePosition->X;
		right2 = _munchie[m]->ePosition->X + _munchie[m]->eRect->Width;
		top2 = _munchie[m]->ePosition->Y;
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_munchie[m]->eaten = true;
			Audio::Play(_pop);
			delete _munchie[m]->ePosition;
			_pacman->score = _pacman->score + 1;


		}
	}

}
//
void Pacman::UpdatePacman(int elapsedTime)
{
	if (_pacman->frameTime > _cPacmanFrameTime)
	{
		_pacman->frame++;
		if (_pacman->frame >= 2)
			_pacman->frame = 0;
		_pacman->frameTime = 0;


	}
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;

}

void Pacman::UpdateGhosts(MovingEnemy* ghost, int elapsedTime)
{
	if (ghost->direction == 0)// moves right
	{
		ghost->position->X += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 1)//moves left
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 2)//moves up
	{
		ghost->position->Y += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 3)// moves down
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
	}


	if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth())//hits right edge
	{
		ghost->direction = 1; // change direction
	}
	else if (ghost->position->X <= 0)//hits left edge
	{
		ghost->direction = 0;//change direction
	}
	for (int g = 0; g < GHOSTCOUNT; g++)
	{
		_ghost[g]->frameTime += elapsedTime;
		if (_ghost[g]->frameTime > _cGhostFrameTime)
		{
			_ghost[g]->frame++;
			if (_ghost[g]->frame >= 2)
				_ghost[g]->frame = 0;
			_ghost[g]->frameTime = 0;
		}
		_ghost[g]->sourceRect->X = _ghost[g]->sourceRect->Width* _ghost[g]->frame;
		
		_ghost[g]->sourceRect->Y = _ghost[g]->sourceRect->Height * _ghost[g]->direction;
	}
}

//
void Pacman::UpdateMunchies(Enemy*, int elapsedTime)
{
	int i;
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[1]->eFrame += elapsedTime;
		if (_munchie[1]->eFrame > cMunchieFrameTime)
		{
			_munchie[i]->eFrameCount++;
			if (_munchie[i]->eFrameCount >= 2)
				_munchie[i]->eFrameCount = 0;
			_munchie[1]->eFrame = 0;

		}
		_cherry->eRect->X = _cherry->eRect->Width * _munchie[i]->eFrameCount;
		
		_munchie[i]->eRect->X = _munchie[i]->eRect->Width * _munchie[i]->eFrameCount;
	}

}



void Pacman::Update(int elapsedTime)
{

	{
		// Gets the current state of the keyboard
		Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
		Input::MouseState* mouseState = Input::Mouse::GetState();
		if (!_gameStart)
		{
		
			if (!_paused)
			{
				//to enable movement of pacman
				Input(elapsedTime, keyboardState, mouseState);
				
				//to animate the collectables
				int i;
				for (i = 0; i < MUNCHIECOUNT; i++)
				{
					UpdateMunchies(_munchie[i], elapsedTime);
				}

				for (int g = 0; g < GHOSTCOUNT; g++)
				{
					UpdateGhosts(_ghost[g], elapsedTime);
				}
			
			}
		}
		//to animate pacman
		UpdatePacman(elapsedTime);
		
		// to start the game
		CheckStart(keyboardState, (Input::Keys::SPACE));
		
		//to pause the game
		CheckPaused(keyboardState, (Input::Keys::P));
		
		// this is to loop the edges
		CheckViewportCollision();

		CheckGhostCollision();
		CheckMunchieCollision();
		CheckWallCollision();
		
		
		
	}

}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y << "   score " << _pacman->score;


	SpriteBatch::BeginDraw(); // Starts Drawing
	if (_gameStart)
	{
		std::stringstream menuStream;
		menuStream << "Press space to start the game";

		SpriteBatch::Draw(_menu->background, _menu->rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->stringPosition, Color::Red);



	}
	SpriteBatch::Draw(_cherry->eTexture, _cherry->ePosition, _cherry->eRect); //draws cherry

	int i;
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		if (!_munchie[i]->eaten)
		{
			SpriteBatch::Draw(_munchie[i]->eTexture, _munchie[i]->ePosition, _munchie[i]->eRect);
		}
	}

	for (int g = 0; g < GHOSTCOUNT; g++)
	{
		SpriteBatch::Draw(_ghost[g]->texture, _ghost[g]->position, _ghost[g]->sourceRect);

	}
	for (int w = 0; w < 100; w++)
	{
		SpriteBatch::Draw(_wall[w]->texture, _wall[w]->position, _wall[w]->sourceRect);
	}
	//SpriteBatch::Draw(_munchieTexture, _munchiePosition, _munchieRect);
	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	}
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!!!!";

		SpriteBatch::Draw(_menu->background, _menu->rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->stringPosition, Color::Red);



	}

	SpriteBatch::EndDraw(); // Ends Drawing
}