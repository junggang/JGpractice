#pragma once
#include <windows.h>
#include "SampleObject.h"
class CManager
{
public:
	CManager(void);
	~CManager(void);

	static CManager* CManager::GetInstance();
	static void ReleaseInstance();

	bool GetHWND(HWND hwnd) { m_hwnd = hwnd; if (m_hwnd == nullptr) return false; return true;}
	bool Init();
	HWND GetWindowHandle() {return m_hwnd;}
	void Render();

private:
	static CManager* m_pInstance;
	HWND m_hwnd;
	CSampleObject* m_SampleObject;
};

