

class Button : public olc::PGEX
{
public:
	Button(olc::vi2d pos, olc::vi2d size, std::string text, olc::Pixel textColor=olc::WHITE, int textSize=1)
	{
		this->pos = pos;
		this->size = size;
		this->text = text;
		this->textColor = textColor;
		this->textSize = textSize;
	}

public:
olc::vi2d pos;
olc::vi2d size;
std::string text;
olc::Pixel textColor;
int textSize;

	void draw()
	{
		pge->DrawRect(this->pos, this->size);
		olc::vi2d realTextSize = pge->GetTextSize(this->text);
		pge->DrawString(this->pos.x+this->size.x/2-realTextSize.x/2*this->textSize,\
						this->pos.y+this->size.y/2-realTextSize.y/2*this->textSize,\
						this->text, this->textColor, this->textSize);
	}

	bool pressed()
	{
		bool withinX = (pge->GetMouseX() >= this->pos.x && pge->GetMouseX() <= this->pos.x+this->size.x);
		bool withinY = (pge->GetMouseY() >= this->pos.y && pge->GetMouseY() <= this->pos.y+this->size.y);
		bool isPressed = pge->GetMouse(0).bPressed;
		return withinX && withinY && isPressed;
	}
		
};
