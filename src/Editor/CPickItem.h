// by jrzanol
//

#pragma once
#include "CEvent.h"

class CPickItem : CEvent
{
public:
	CPickItem();

	void ProcessMouseDragEvent(GLFWwindow*, float, float);
	void ProcessMouseButtonEvent(GLFWwindow*, int, int, int);

	static std::vector<std::tuple<CMesh*, Vertex*>> g_ClickedObject;

private:
	
};

