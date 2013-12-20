#pragma once
#include <string>
#include <d2d1.h>
#include <wincodec.h>
#include <array>
enum LoopType
{
	S_LT_ONCE,
	S_LT_INFINITE,
};
enum AnimationState
{
	S_PAUSE,
	S_PLAY,
	S_STOP
};
class CSprite
{
public:
	CSprite(void);
	~CSprite(void);

public : CSprite(std::wstring fileName)
	{
		m_ipD2DFactory = nullptr;
		m_ipRenderTarget = nullptr;

		m_pImagingFactory = nullptr;
		m_pDecoder = nullptr;
		m_pFrame = nullptr;	
		m_pConvertedSourceBitmap = nullptr;
		m_ipBitampTraget = nullptr;
		m_LoadedBitmap = nullptr;

		m_TotalFrameNumber = -1;
		m_CurrentFrame = 0;

		m_fileName = fileName;
	}

	ID2D1Factory*			GetD2DFactory() const { return m_ipD2DFactory; }
	ID2D1HwndRenderTarget*	GetHwndRenderTarget() const { return m_ipRenderTarget; }

	bool SetRenderTarget();

	//������->�׸��� �غ��Ѵ�.
	bool LoadAnimationImage();
	
	//������ �ӵ��� ���Ѵ�. (�����Ӵ� �ʸ� �Է�)
	void SetFrameSpeed(float second) { m_FrameSpeed = (DWORD)second*1000;}

	//���� �̹����� ����, ���� ���̸� ���� �ڸ���.-> ������ ������ �˾Ƽ� ����
	void CutFrames(float width, float height);

	//�����Ӱ����� ���� ���� ���(�ɼ�)
	void SetFrameNumber(int frameNumber) { m_TotalFrameNumber = frameNumber; }

	//�ִϸ��̼� ��� Ÿ���� ���Ѵ� 
	//	S_LT_ONCE : �� ���� ��� S_LT_INFINITE: �ݺ� ���
	void SetLoop(LoopType looptype) { m_LoopType = looptype;}

	//imagePosition��ġ���� ���ϸ��̼��� ����Ѵ�.
	void StartAnimation(D2D1_RECT_F imagePosition);

	void PauseAnimation() { m_AnimationState = S_PAUSE; }
	
	void StopAnimation() { m_AnimationState = S_STOP; }

	//��������
	void BeginDraw() {m_ipRenderTarget->BeginDraw();}
	void ClearDraw() {m_ipRenderTarget->Clear();}
	void EndDraw() {m_ipRenderTarget->EndDraw();}

private:
	CSprite* m_Sprite;
	std::wstring m_fileName;

	LoopType m_LoopType;
	AnimationState m_AnimationState;

	std::array <D2D1_RECT_F,20> m_Frame;
	int m_TotalFrameNumber;

	ID2D1Factory*			m_ipD2DFactory;
	ID2D1HwndRenderTarget*	m_ipRenderTarget;

	IWICImagingFactory * m_pImagingFactory;
	IWICBitmapDecoder *m_pDecoder;
	IWICBitmapFrameDecode *m_pFrame;	
	IWICFormatConverter* m_pConvertedSourceBitmap;
	ID2D1BitmapRenderTarget* m_ipBitampTraget;
	ID2D1Bitmap *m_LoadedBitmap;

	//�ð�
	DWORD m_StartTime;
	DWORD m_CurrentTime;
	DWORD m_CheckedTime;
	DWORD m_FrameSpeed;

	int m_CurrentFrame;
	D2D1_RECT_F destRect;

};
