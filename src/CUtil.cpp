// by jrzanol
//

#include "stdafx.h"
#include "CWindowGL.h"

GLFWwindow* m_Window = NULL;

float CUtil::m_DeltaTime = 0;
float CUtil::m_LastTime = 0;

SliderInfo CUtil::g_SliderInfo(0.f, 0.f, 0, 1.f, 1.f);

