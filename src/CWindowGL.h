// by jrzanol
//

#pragma once

#include "CObject.h"

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

class CWindowGL
{
public:
	CWindowGL();
	~CWindowGL();

	bool Initialize();
	void Cleanup();
	bool Render();

	static SliderInfo g_SliderInfo;
	static const int g_MaxX = 1024;
	static const int g_MaxY = 768;

private:
	std::list<CDrawableObject*> m_DrawObject;
	
	GLuint CompileShader(const char*, GLenum);
	GLuint LinkProgram(GLuint, GLuint);

	GLFWwindow* m_Window;
	GLuint m_ProgramId;

	static const char* g_VertexShader;
	static const char* g_FragmentShader;
};

