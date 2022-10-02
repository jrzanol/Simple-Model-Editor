// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CCamera.h"
#include "CObject.h"

class CWindow : CCamera
{
public:
	CWindow();
	~CWindow();

	bool Initialize();
	void Cleanup();
	bool Render();

private:
	std::list<CDrawableObject*> m_DrawObject;
	
	GLuint m_ProgramId;

	GLuint CompileShader(const char*, GLenum);
	GLuint LinkProgram(GLuint, GLuint);
};

