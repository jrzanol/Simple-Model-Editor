// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CEvent.h"

std::vector<CEvent*> g_EventList;

CEvent::CEvent()
{
	g_EventList.push_back(this);
}

void MouseEvent_Button_Callback(GLFWwindow* window, int button, int action, int mods)
{
	std::cout << "Mouse Button Event: " << button << ", " << action << std::endl;

	for (const auto& it : g_EventList)
		it->ProcessMouseButtonEvent(window, button, action, mods);
}

void MouseEvent_Callback(GLFWwindow* window, double xpos, double ypos)
{
	for (const auto& it : g_EventList)
		it->ProcessMouseEvent(window, xpos, ypos);
}

void CEvent::Initialize()
{
	glfwSetMouseButtonCallback(m_Window, MouseEvent_Button_Callback);
	glfwSetCursorPosCallback(m_Window, MouseEvent_Callback);
}

