// by jrzanol
//

#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

struct SliderInfo
{
	int m_X;
	int m_Y;
	int m_Angle;
	float m_ScaleX;
	float m_ScaleY;

	SliderInfo(int x, int y, int angle, float scalex, float scaley)
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
	GLuint CompileShader(const char*, GLenum);
	GLuint LinkProgram(GLuint, GLuint);
	GLuint LoadDataInBuffers();

	GLFWwindow* m_Window;

	static const char* g_VertexShader;
	static const char* g_FragmentShader;
};

