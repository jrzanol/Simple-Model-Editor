// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CCamera.h"
#include "CModel.h"

class CWindow : CCamera
{
public:
	CWindow();
	~CWindow();

	bool Initialize();
	void Cleanup();
	bool Render();

	static const glm::mat4& GetMVP();
	static const std::list<CModel*>& GetModels();

private:
	GLuint m_ProgramId;
	GLuint m_PickingProgramId;

	GLuint CompileShader(const char*, GLenum);
	GLuint LinkProgram(GLuint, GLuint);

	static glm::mat4 m_MVP;
	static std::list<CModel*> m_DrawModel;
};

