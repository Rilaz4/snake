#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <deque>

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
};

class Snake
{
public:
	Snake(int x, int y, int rotation, int length)
	{
		for (int b=0; b<length;b++)
		{
			if (rotation == Rotations::Up)
				this->blocks.push_back(Block(x, y-b));
			else if (rotation == Rotations::Right)
				this->blocks.push_back(Block(x+b, y));
			else if (rotation == Rotations::Down)
				this->blocks.push_back(Block(x, y-b));
			else if (rotation == Rotations::Left)
				this->blocks.push_back(Block(x-b, y));
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
	
	void move()
	{
		this->blocks.pop_back();
		this->blocks.push_front(Block(blocks[0].getX()+1, blocks[0].getY()));
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
std::vector<Block> blocks;
const int BLOCK_WIDTH = ScreenWidth()/10;
const int BLOCK_HEIGHT = ScreenHeight()/10;
float elapsedTotal = 2;

public:
	bool OnUserCreate() override
	{
		blocks.push_back(Block(3, 3));
		blocks.push_back(Block(4, 3));
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (elapsedTotal >= 1)
		{
			elapsedTotal = 0;
			Clear(olc::BLACK);
			drawSnake();
			updateSnake();
		}
		else
			elapsedTotal += fElapsedTime;
		return true;
	}

	void drawSnake()
	{
		for (Block block : snake.getBlocks())
		{
			FillRect({block.getX()*BLOCK_WIDTH, block.getY()*BLOCK_HEIGHT}, {BLOCK_WIDTH, BLOCK_HEIGHT});
		}
	}

	void updateSnake()
	{
		snake.move();
	}
};

int main()
{
	SnakeGame game;
	if (game.Construct(400, 400, 1, 1))
		game.Start();
	return 0;
}

