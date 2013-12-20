#include "stdafx.h"
#include "Manager.h"
#include "Sprite.h"


CManager* CManager::m_pInstance = nullptr;
CManager::CManager(void)
{
	m_SampleObject = nullptr;
}


CManager::~CManager(void)
{
}

CManager* CManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CManager();
	}

	return m_pInstance;
}

void CManager::Render()
{
	m_SampleObject->Render();
}

bool CManager::Init()
{
	m_SampleObject = new CSampleObject();
	m_SampleObject ->CreateSources();

	return true;
}



