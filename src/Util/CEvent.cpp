// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CEvent.h"

std::vector<CEvent*>* g_EventList;

CEvent::CEvent()
{
	if (g_EventList == NULL)
		g_EventList = new std::vector<CEvent*>();

	g_EventList->push_back(this);
}

void MouseEvent_Button_Callback(GLFWwindow* window, int button, int action, int mods)
{
	for (const auto& it : *g_EventList)
		it->ProcessMouseButtonEvent(window, button, action, mods);
}

void MouseEvent_Callback(GLFWwindow* window, double xpos, double ypos)
{
	for (const auto& it : *g_EventList)
		it->ProcessMouseEvent(window, xpos, ypos);
}

void MouseEvent_Scroll_Callback(GLFWwindow* window, double xpos, double ypos)
{
	for (const auto& it : *g_EventList)
		it->ProcessMouseScroll(window, xpos, ypos);
}

void CEvent::Initialize()
{
	glfwSetMouseButtonCallback(g_Window, MouseEvent_Button_Callback);
	glfwSetCursorPosCallback(g_Window, MouseEvent_Callback);
	glfwSetScrollCallback(g_Window, MouseEvent_Scroll_Callback);
}

