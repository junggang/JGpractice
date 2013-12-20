#include "stdafx.h"
#include "Sprite.h"
#include "Manager.h"
#include "MacroSet.h"
#include <assert.h>
#include <combaseapi.h>

CSprite::CSprite(void)
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
}


CSprite::~CSprite(void)
{
}

bool CSprite::SetRenderTarget()
{
	HRESULT hr;
	hr = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_ipD2DFactory);
	if (hr != S_OK)
	{
		return false;
	}

	HWND hwnd = CManager::GetInstance()->GetWindowHandle();

	RECT rt;
	GetClientRect(hwnd, &rt);
	D2D1_SIZE_U size = D2D1::SizeU(rt.right - rt.left, rt.bottom - rt.top);

	//�ϴ� �����ӷ� �϶� ���� Ȯ���� ���ؼ� vsync�� �� ���·� ����
	hr = m_ipD2DFactory->CreateHwndRenderTarget( D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		&m_ipRenderTarget );

	return hr;
}
bool CSprite::LoadAnimationImage()
{
	if (SetRenderTarget()!=S_OK)
		return false;

	HRESULT hr;

	const WCHAR * myFile = m_fileName.c_str();
	CoInitialize(NULL);
		hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pImagingFactory)
		);

	if ( SUCCEEDED(hr))
	{
		hr = m_pImagingFactory->CreateDecoderFromFilename(
			myFile,                      // Image to be decoded
			NULL,                            // Do not prefer a particular vendor
			GENERIC_READ,                    // Desired read access to the file
			WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
			&m_pDecoder                        // Pointer to the decoder
			);
	}
	// Retrieve the first frame of the image from the decoder
	// gif �̹����� ��� ������ ���� �ٲ� �ָ� ���� �������� �ҷ� �� �� �ִ�.
	// ��������Ʈ�� �̰Ŷ� �ٸ�->�߶� ���°�?
	if (SUCCEEDED(hr))
	{
		hr = m_pDecoder->GetFrame(0, &m_pFrame);
	}

	if (SUCCEEDED(hr))
	{
		SafeRelease(m_pConvertedSourceBitmap);
		hr = m_pImagingFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pConvertedSourceBitmap->Initialize(
			m_pFrame,                          // Input bitmap to convert
			GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
			WICBitmapDitherTypeNone,         // Specified dither pattern
			NULL,                            // Specify a particular palette 
			0.f,                             // Alpha threshold
			WICBitmapPaletteTypeCustom       // Palette translation type
			);
	}

	if (SUCCEEDED(hr))
	{
		//���� �͵��� �����ش�.
		SafeRelease(m_LoadedBitmap);
		hr = m_ipRenderTarget->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_LoadedBitmap);
	}

	assert(hr == S_OK);

	SafeRelease(m_pDecoder);
	SafeRelease(m_pFrame);

	m_AnimationState = S_PAUSE;

	return true;
}

void CSprite::CutFrames(float width, float height)
{
	D2D1_SIZE_F size = m_LoadedBitmap->GetSize();
	float ImageWidth = size.width;
	float ImageHeight = size.height;

	//Frame������ ���� �Է����� ���� ��쿡�� ���.
	if ( m_TotalFrameNumber== -1 )
		m_TotalFrameNumber = (int)(ImageWidth/width)*(ImageHeight/height);

	m_Frame[0].left = 0;
	m_Frame[0].right = width;
	m_Frame[0].top = 0;
	m_Frame[0].bottom = height;

	for ( int i = 1; i < m_TotalFrameNumber ; ++i )
	{
		if ( m_Frame[i-1].right + width < ImageWidth)
		{
			m_Frame[i].left = m_Frame[i-1].left + width;
			m_Frame[i].right = m_Frame[i].left+ width;
			m_Frame[i].top = m_Frame[i-1].top;
			m_Frame[i].bottom = m_Frame[i-1].bottom;
		}
		else
		{
			m_Frame[i].left = 0;
			m_Frame[i].right = width;
			m_Frame[i].top = m_Frame[i-1].top+height;
			m_Frame[i].bottom = m_Frame[i].top+height;
		}	
	}
}


void CSprite::StartAnimation(D2D1_RECT_F imagePosition)
{
	if (m_AnimationState == S_PAUSE)
	{
		m_AnimationState = S_PLAY;
		m_CheckedTime = GetTickCount();
	}

	m_CurrentTime = GetTickCount();
	DWORD interval = m_CurrentTime - m_CheckedTime;

	if (m_LoopType == S_LT_ONCE && m_AnimationState == S_PLAY)
	{
		if (interval > m_FrameSpeed && m_CurrentFrame < m_TotalFrameNumber)
		{
			m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[m_CurrentFrame++%m_TotalFrameNumber]);
			m_CheckedTime = m_CurrentTime;
		}
		else if (m_CurrentFrame == (m_TotalFrameNumber-1))
		{
			m_AnimationState = S_STOP;
		}
		else
		{
			m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[m_CurrentFrame]);
		}
	}

	else if (m_LoopType == S_LT_INFINITE && m_AnimationState == S_PLAY)
	{
		if (interval > m_FrameSpeed)
		{
			m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[m_CurrentFrame++%m_TotalFrameNumber]);
			m_CheckedTime = m_CurrentTime;		
		}
		else
		{
			m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[m_CurrentFrame]);
		}
	}
	
	else
	{
		m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[m_CurrentFrame]);
	}



	//�ϴ� �Ǵ� �ڵ�--------------------------------
// 	if (m_AnimationState == S_PAUSE)
// 	{
// 		m_StartTime = GetTickCount();
// 		m_CheckedTime = m_StartTime;
// 		m_AnimationState = S_PLAY;
// 	}
// 	else
// 	{
// 		m_CurrentTime = GetTickCount();
// 		DWORD interval = m_CurrentTime - m_CheckedTime;
// 		if(interval>m_FrameSpeed)
// 			{
// 				m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[m_CurrentFrame++%m_TotalFrameNumber]);
// 				m_CheckedTime = m_CurrentTime;
// 		}
// 		
// 		else
// 			m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[m_CurrentFrame%m_TotalFrameNumber]);
// 
// 	}
	//----------------------------------------------------------------

	//m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[1]);
// 	if (m_AnimationState == S_PAUSE)
// 	{
// 		m_StartTime = GetTickCount();
// 		m_CheckedTime = m_StartTime;
// 		m_AnimationState = S_PLAY;
// 	}
// 
// 	m_CurrentTime	 = GetTickCount();
// 	DWORD interval = m_CurrentTime - m_CheckedTime;
// 
// 	if ( interval > m_FrameSpeed && m_AnimationState == S_PLAY )
// 	{		
// 		m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,imagePosition,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,m_Frame[m_CurrentFrame++]);
// 		m_CheckedTime = m_CurrentTime;	
// 		//m_CurrentFrame = (m_CurrentFrame+1)%m_TotalFrameNumber;
// 		m_CurrentFrame %= m_TotalFrameNumber;
// 
// 		if (m_CurrentFrame == m_TotalFrameNumber-1 && m_LoopType == S_LT_ONCE )
// 			m_AnimationState = S_STOP;
// 	}
// 	else 
// 	{
// 		m_ipRenderTarget->DrawBitmap(m_LoadedBitmap,destRect,1.0f,D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,m_Frame[m_CurrentFrame]);
// 	}
}