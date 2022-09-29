// by jrzanol
//

#pragma once

class CEvent
{
public:
	CEvent();

	virtual void ProcessSecTimer() { }
	virtual void ProcessMinTimer() { }

	virtual void ProcessMouseButtonEvent(GLFWwindow*, int, int, int) { }
	virtual void ProcessMouseEvent(GLFWwindow*, double, double) { }

	static void Initialize();
};

extern std::vector<CEvent*> g_EventList;

