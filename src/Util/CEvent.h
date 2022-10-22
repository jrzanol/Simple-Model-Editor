// by jrzanol
//

#pragma once

class CEvent
{
public:
	CEvent();

	virtual void ProcessSecTimer() { }
	virtual void ProcessMinTimer() { }

	virtual void ProcessInput(GLFWwindow*) { }

	virtual void ProcessMouseButtonEvent(GLFWwindow*, int, int, int) { }
	virtual void ProcessMouseEvent(GLFWwindow*, double, double) { }
	virtual void ProcessMouseDragEvent(GLFWwindow*, float, float) { }
	virtual void ProcessMouseScroll(GLFWwindow*, double, double) { }

	static void Initialize();
};

extern std::vector<CEvent*>* g_EventList;

