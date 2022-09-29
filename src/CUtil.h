// by jrzanol
//

#pragma once

struct SliderInfo
{
	float m_X;
	float m_Y;
	int m_Angle;
	float m_ScaleX;
	float m_ScaleY;

	SliderInfo(float x, float y, int angle, float scalex, float scaley)
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
	static float m_DeltaTime;
	static float m_LastTime;

	static SliderInfo g_SliderInfo;
	static const int g_MaxX = 1024;
	static const int g_MaxY = 768;
};

extern GLFWwindow* m_Window;
