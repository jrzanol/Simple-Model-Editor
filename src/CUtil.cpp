// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

GLFWwindow* g_Window = NULL;

float g_DeltaTime = 0;
float g_LastTime = 0;

SSliderInfo CUtil::m_SliderInfo(0.f, 0.f, 0, 1.f, 1.f);

#if _MSC_VER >= 1930
const char* CUtil::m_VertexShader = R"glsl(
    #version 430

    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec3 a_normals;
    layout(location = 2) in vec2 a_texcoords;

    uniform mat4 u_view;
    uniform mat4 u_proj;
    uniform mat4 u_model;

    void main() {
        gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
    }
)glsl";
#else
const char* CUtil::m_VertexShader = R"glsl(
    #version 150
    #extension GL_ARB_explicit_attrib_location : require

    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec3 a_normals;
    layout(location = 2) in vec2 a_texcoords;

    uniform mat4 u_view;
    uniform mat4 u_proj;
    uniform mat4 u_model;

    void main() {
        gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
    }
)glsl";
#endif

const char* CUtil::m_FragmentShader = R"glsl(
    #version 130

    void main() {
        gl_FragColor = vec4(1, 0, 0, 1);
    }
)glsl";

