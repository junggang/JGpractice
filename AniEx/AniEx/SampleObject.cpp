#include "stdafx.h"
#include "SampleObject.h"


CSampleObject::CSampleObject(void)
{
}


CSampleObject::~CSampleObject(void)
{
}

void CSampleObject::CreateSources()
{
	m_sprite = new CSprite(L"sample.png");
	if ( m_sprite->LoadAnimationImage())
	{
		m_sprite->CutFrames(118.3f,118.3f);
		m_sprite->SetFrameSpeed(0.1f);
		//m_sprite->SetLoop(S_LT_ONCE);
		m_sprite->SetLoop(S_LT_INFINITE);
	}

	dest.left = 0.0f;
	dest.right = 100.0f;
	dest.top = 0.0f;
	dest.bottom = 100.0f;
}

void CSampleObject::Render()
{
	m_sprite->BeginDraw();
	m_sprite->ClearDraw();
	m_sprite->StartAnimation(dest);
	//m_sprite->StartRotateAnimation(dest);
	//m_sprite->MoveAnimation(dest,D2D1::RectF(500.0f,0.0f,600.0f,100.0f));
	m_sprite->EndDraw();
}
