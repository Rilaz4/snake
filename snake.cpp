#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <deque>
#include <random>

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
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(1, 9);
		sAppName = "Snake";
	}
public:
Snake snake =  Snake(3, 3, Rotations::Right, 3);
Block fruit = Block(6, 7);
const int BLOCK_WIDTH = ScreenWidth()/10;
const int BLOCK_HEIGHT = ScreenHeight()/10;
float elapsedTotal = 2;
bool isDead = false;

std::random_device rd;
std::mt19937 mt;
std::uniform_int_distribution<int> dist;

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	
		if (isDead)
		{
			Clear(olc::BLACK);
			DrawString({ScreenWidth()/2-80, ScreenHeight()/2}, "you lost", olc::RED, 3);
			return true;
		}
		if (elapsedTotal >= 1)
		{
			elapsedTotal = 0;
			Clear(olc::BLACK);
			updateSnake();
			drawSnake();
			FillCircle({fruit.getX()*BLOCK_WIDTH+BLOCK_WIDTH/2, fruit.getY()*BLOCK_HEIGHT+BLOCK_HEIGHT/2}, BLOCK_WIDTH/2, olc::RED);
		}
		else
			elapsedTotal += fElapsedTime;
		return true;
	}

	void drawSnake()
	{
		for (int blockAt=0; blockAt < snake.getLength(); blockAt++)
		{
			Block block = snake.getBlocks().at(blockAt);
			olc::Pixel color = (blockAt % 2 == 0)? olc::BLUE : olc::GREEN;
			FillRect({block.getX()*BLOCK_WIDTH, block.getY()*BLOCK_HEIGHT}, {BLOCK_WIDTH, BLOCK_HEIGHT}, color);
		}
	}

	void updateSnake()
	{
		if(GetKey(olc::A).bHeld) snake.changeRotation(Rotations::Left);
		if(GetKey(olc::W).bHeld) snake.changeRotation(Rotations::Up);
		if(GetKey(olc::D).bHeld) snake.changeRotation(Rotations::Right);
		if(GetKey(olc::S).bHeld) snake.changeRotation(Rotations::Down);
		if(GetKey(olc::SPACE).bHeld) snake.grow(2);

		snake.move();
		for (int blockAt=1; blockAt < snake.getLength(); blockAt++)
		{
			Block block = snake.getBlocks().at(blockAt);
			if (snake.getBlocks().front().getCoords() == block.getCoords())
				isDead = true;
		}
		if (snake.getBlocks().front().getCoords() == fruit.getCoords())
		{
			snake.grow(1);
			fruit.setCoords(dist(mt), dist(mt));
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

