// by jrzanol
//

#pragma once

struct SSliderInfo
{
	float m_X;
	float m_Y;
	int m_Angle;
	float m_ScaleX;
	float m_ScaleY;

	SSliderInfo(float x, float y, int angle, float scalex, float scaley)
	{
		m_X = x;
		m_Y = y;
		m_Angle = angle;
		m_ScaleX = scalex;
		m_ScaleY = scaley;
	}
};

class CUtil
{
public:
	static SSliderInfo m_SliderInfo;

	static const char* m_VertexShader;
	static const char* m_FragmentShader;
};

extern float g_DeltaTime;
extern float g_LastTime;

const int g_WindowMaxX = 1024;
const int g_WindowMaxY = 768;

extern GLFWwindow* g_Window;
