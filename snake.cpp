#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

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
		
		this->rotation = rotation;
		this->length = length;
	}

private:
	std::vector<Block> blocks;
	int rotation;
	int length;
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

public:
	bool OnUserCreate() override
	{
		blocks.push_back(Block(3, 3));
		blocks.push_back(Block(4, 3));
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		drawBlocks();
		return true;
	}

	void drawBlocks()
	{
		for (Block block : blocks)
		{
			FillRect({block.getX()*BLOCK_WIDTH, block.getY()*BLOCK_HEIGHT}, {BLOCK_WIDTH, BLOCK_HEIGHT});
		}
	}
};

int main()
{
	SnakeGame game;
	if (game.Construct(400, 400, 1, 1))
		game.Start();
	return 0;
}

