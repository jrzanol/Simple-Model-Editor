// by jrzanol
//

#pragma once
#include "CEvent.h"

class CPickItem : CEvent
{
public:
	CPickItem();

	void ProcessMouseButtonEvent(GLFWwindow*, int, int, int);
};

