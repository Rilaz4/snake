#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <deque>
#include <random>

enum GameStates
{
	Menu,
	Playing,
	Dead,
};

enum Rotations
{
Up,
Right,
Down,
Left
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
		std::mt19937 mt(rd());
		std::uniform_int_distribution<> dist(0, 9);
		sAppName = "Snake";
	}
public:
Snake snake =  Snake(3, 3, Rotations::Right, 3);
Block fruit = Block(6, 7);

int BLOCKS_HORIZONTAL = 20;
int BLOCKS_COLOUMN = 20;
float BLOCK_WIDTH;
float BLOCK_HEIGHT;

float elapsedTotal = 2;
int gameState = GameStates::Menu;

std::random_device rd;
std::mt19937 mt;
std::uniform_int_distribution<> dist;

public:
	bool OnUserCreate() override
	{
		BLOCK_WIDTH = ScreenWidth()/BLOCKS_HORIZONTAL;
		BLOCK_HEIGHT = ScreenHeight()/BLOCKS_COLOUMN;
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	
		switch (gameState)
		{
			case GameStates::Menu:
				Clear(olc::BLACK);
				DrawString(ScreenWidth()/2-100, ScreenHeight()/2, "play snake\n press space", olc::WHITE, 3);
				gameState = (GetKey(olc::SPACE).bPressed)? GameStates::Playing : GameStates::Menu;
				break;
			case GameStates::Playing:
				if (elapsedTotal >= 0.5)
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
		DrawString(10, 10, std::to_string(gameState));
		/*if (isDead)
		{
			Clear(olc::BLACK);
			DrawString({ScreenWidth()/2-80, ScreenHeight()/2}, "you lost", olc::RED, 3);
			DrawString(1, 1, std::to_string(isDead));
			if(GetKey(olc::SPACE).bPressed) resetSnake();
			return true;
		}
		if (elapsedTotal >= 0.5)
		{
			elapsedTotal = 0;
			Clear(olc::BLACK);
			updateSnake();
			drawSnake();
		}
		else
			elapsedTotal += fElapsedTime;*/
		return true;
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
		fruit.setCoords(dist(mt)%BLOCKS_HORIZONTAL+1, dist(mt)%BLOCKS_COLOUMN+1);
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

