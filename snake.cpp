#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "button.h"
#include <deque>

enum GameStates
{
	Menu,
	Playing,
	Dead,
};

enum MenuStates
{
	Main,
	SpeedSelect,
};

enum Rotations
{
Up,
Right,
Down,
Left
};

enum Speeds
{
Slow = 1,
Medium = 2,
Fast = 4
};

class Block
{
public:
	Block(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

protected:
	int x;
	int y;

public:
	int getX()
	{
		return x;
	}

	int getY()
	{
		return y;
	}
	
	olc::vi2d getCoords()
	{
		return {this->x, this->y};
	}

	void setCoords(int newX, int newY)
	{
		this->x = newX;
		this->y = newY;
	}
};

class Snake
{
public:
	Snake(int x, int y, int rotation, int length)
	{
		for (int b=0; b<length;b++)
		{
			if (rotation == Rotations::Up)
				this->blocks.push_front(Block(x, y-b));
			else if (rotation == Rotations::Right)
				this->blocks.push_front(Block(x+b, y));
			else if (rotation == Rotations::Down)
				this->blocks.push_front(Block(x, y+b));
			else if (rotation == Rotations::Left)
				this->blocks.push_front(Block(x-b, y));
		}
		this->rotation = rotation;
		this->length = length;
	}

private:
	std::deque<Block> blocks;
	int rotation;
	int length;

public:
	std::deque<Block> getBlocks()
	{
		return this->blocks;
	}
	
	int getLength()
	{
		return this->length;
	}
	
	void move()
	{
		this->blocks.pop_back();
		if (rotation == Rotations::Up)
				this->blocks.push_front(Block(this->blocks[0].getX(), this->blocks[0].getY()-1));
			else if (rotation == Rotations::Right)
				this->blocks.push_front(Block(this->blocks[0].getX()+1, this->blocks[0].getY()));
			else if (rotation == Rotations::Down)
				this->blocks.push_front(Block(this->blocks[0].getX(), this->blocks[0].getY()+1));
			else if (rotation == Rotations::Left)
				this->blocks.push_front(Block(this->blocks[0].getX()-1, this->blocks[0].getY()));
	}

	void changeRotation(int newRotation)
	{
		if (this->rotation == Rotations::Up && newRotation == Rotations::Down)
			return;
		else if (this->rotation == Rotations::Left && newRotation == Rotations::Right)
			return;
		else if (this->rotation == Rotations::Down && newRotation == Rotations::Up)
			return;
		else if (this->rotation == Rotations::Right && newRotation == Rotations::Left)
			return;
		else
			this->rotation=newRotation;
	}

	void grow(int amount)
	{
		this->length += amount;
		for (int newOnes=0; newOnes<amount; newOnes++)
		{
			this->blocks.push_back(Block(this->blocks.back().getX(), this->blocks.back().getY()));
		}
	}
};

class SnakeGame : public olc::PixelGameEngine
{
public:
	SnakeGame()
	{
		sAppName = "Snake";
	}
public:
Snake snake =  Snake(3, 3, Rotations::Right, 3);
Block fruit = Block(6, 7);

Button startButton = Button({0, 0}, {80, 24}, "play", olc::WHITE, 2); 
Button speedSelect1 = Button({0, 0}, {80, 24}, "slow", olc::WHITE, 2);
int BLOCKS_HORIZONTAL = 10;
int BLOCKS_COLOUMN = 10;
float BLOCK_WIDTH;
float BLOCK_HEIGHT;

float elapsedTotal = 2;
int gameState = GameStates::Menu;
int menuState = MenuStates::Main;
float snakeSpeed = Speeds::Fast;

public:
	bool OnUserCreate() override
	{
		std::srand(std::time(NULL));
		BLOCK_WIDTH = ScreenWidth()/BLOCKS_HORIZONTAL;
		BLOCK_HEIGHT = ScreenHeight()/BLOCKS_COLOUMN;
		startButton.pos = {ScreenWidth()/2-40, ScreenHeight()/2-12};
		speedSelect1.pos = {ScreenWidth()/2-85, ScreenHeight()/2-12};
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	
		switch (gameState)
		{
			case GameStates::Menu:
				switch (menuState)
				{
					case MenuStates::Main:
						{
							Clear(olc::BLACK);
							olc::vi2d playTextSize = GetTextSize("Play Snake");
							DrawString(ScreenWidth()/2-playTextSize.x/2*3, ScreenHeight()/3-playTextSize.y/2*3, "play snake", olc::WHITE, 3);
							startButton.draw();
							menuState = (startButton.pressed())? MenuStates::SpeedSelect : MenuStates::Main;
							break;
						}
					case MenuStates::SpeedSelect:
						{
							Clear(olc::BLACK);
							olc::vi2d speedTextSize = GetTextSize("Select Speed");
							DrawString(ScreenWidth()/2-speedTextSize.x/2*3, ScreenHeight()/3-speedTextSize.y/2*3, "Select Speed", olc::WHITE, 3);
							speedSelect1.draw();
							if(speedSelect1.pressed()) startGame(Speeds::Slow);
							break;
						}
				}
				break;
			case GameStates::Playing:
				if (elapsedTotal >= 1/snakeSpeed)
				{
					elapsedTotal = 0;
					Clear(olc::BLACK);
					updateSnake();
					drawSnake();
					drawFruit();
				}
				else
					elapsedTotal += fElapsedTime;
				break;
			case GameStates::Dead:
				Clear(olc::BLACK);
				DrawString(ScreenWidth()/2-80, ScreenHeight()/2, "you died", olc::RED, 3);
				if (GetKey(olc::SPACE).bPressed) resetGame();
				break;
		}
		return true;
	}

	void startGame(int selectedSpeed)
	{
		snakeSpeed = selectedSpeed;
		gameState = GameStates::Playing;
	}


	void drawSnake()
	{
		for (int blockAt=0; blockAt < snake.getLength(); blockAt++)
		{
			Block block = snake.getBlocks().at(blockAt);
			olc::Pixel color = (blockAt % 2 == 0)? olc::BLUE : olc::GREEN;
			color = (blockAt == 0)? olc::DARK_BLUE : color;
			FillRect({block.getX()*BLOCK_WIDTH-BLOCK_WIDTH, block.getY()*BLOCK_HEIGHT-BLOCK_HEIGHT}, {BLOCK_WIDTH, BLOCK_HEIGHT}, color);
		}
	}

	void drawFruit()
	{
		FillCircle({fruit.getX()*BLOCK_WIDTH-BLOCK_WIDTH+BLOCK_WIDTH/2, fruit.getY()*BLOCK_HEIGHT-BLOCK_HEIGHT+BLOCK_HEIGHT/2}, BLOCK_WIDTH/2, olc::RED);
	}
		
	void updateSnake()
	{
		if(GetKey(olc::A).bHeld) snake.changeRotation(Rotations::Left);
		if(GetKey(olc::W).bHeld) snake.changeRotation(Rotations::Up);
		if(GetKey(olc::D).bHeld) snake.changeRotation(Rotations::Right);
		if(GetKey(olc::S).bHeld) snake.changeRotation(Rotations::Down);

		snake.move();
		Block snakeHead = snake.getBlocks().front();
		if (snakeHead.getX() > BLOCKS_HORIZONTAL || snakeHead.getX() < 1 || snakeHead.getY() > BLOCKS_COLOUMN || snakeHead.getY() < 1)
			gameState = GameStates::Dead;
		for (int blockAt=1; blockAt < snake.getLength(); blockAt++)
		{
			Block block = snake.getBlocks().at(blockAt);
			if (snakeHead.getCoords() == block.getCoords())
				gameState = GameStates::Dead;
		}
		if (snakeHead.getCoords() == fruit.getCoords())
		{
			snake.grow(4);
			moveFruit();		
		}
	}
	
	void moveFruit()
	{
		fruit.setCoords(std::rand()%BLOCKS_HORIZONTAL+1, std::rand()%BLOCKS_COLOUMN+1);
		for (Block snakeBlock : snake.getBlocks())
		{
			if (fruit.getCoords() == snakeBlock.getCoords())
				moveFruit();
		}
	}

	void resetGame()
	{
		snake =  Snake(3, 3, Rotations::Right, 3);
		fruit = Block(6, 7);
		gameState = GameStates::Menu;
	}
};

int main()

{
	SnakeGame game;
	if (game.Construct(400, 400, 1, 1))
		game.Start();
	return 0;
}

