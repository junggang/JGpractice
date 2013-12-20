#pragma once
#include "Sprite.h"
class CSampleObject
{
public:
	CSampleObject(void);
	~CSampleObject(void);

	void CreateSources();
	void Render();

private:
	CSprite* m_sprite;
	D2D1_RECT_F dest;
};

