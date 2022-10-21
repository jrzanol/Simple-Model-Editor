// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

GLFWwindow* g_Window = NULL;

float g_DeltaTime = 0;
float g_LastTime = 0;

SSliderInfo CUtil::m_SliderInfo(0.f, 0.f, 0, 1.f, 1.f);

const char* CUtil::m_VertexShader = R"glsl(
    #version 430

    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec3 a_normals;
    layout(location = 2) in vec2 a_texcoords;
    layout(location = 3) in vec3 a_tangent;
    layout(location = 4) in vec3 a_bitangent;
    
    out vec2 TexCoords;

    uniform mat4 u_mvp;

    void main() {
        TexCoords = a_texcoords;

        gl_Position = u_mvp * vec4(a_position, 1.0);
    }
)glsl";

const char* CUtil::m_PickingVertexShader = R"glsl(
    #version 430

    layout(location = 0) in vec3 a_position;
    
    out vec2 TexCoords;

    uniform mat4 u_view;
    uniform mat4 u_proj;
    uniform mat4 u_model;

    void main() {
        gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
    }
)glsl";

const char* CUtil::m_FragmentShader = R"glsl(
    #version 430
    
    out vec4 FragColor;
    in vec2 TexCoords;
    
    uniform sampler2D texture_diffuse1;
    uniform sampler2D texture_specular1;
    uniform sampler2D texture_normal1;
    uniform int u_wireframe;
    
    void main() {
        if (u_wireframe != 0) {
            FragColor = vec4(1, 0, 0, 0);
        }
        else {
            FragColor = mix(texture(texture_diffuse1, TexCoords), texture(texture_specular1, TexCoords), texture(texture_normal1, TexCoords));
        }
    }
)glsl";

const char* CUtil::m_PickingFragmentShader = R"glsl(
    #version 430

    // Ouput data
    out vec4 color;

    // Values that stay constant for the whole mesh.
    uniform vec4 PickingColor;

    void main() {
	    color = PickingColor;
    }
)glsl";

//ray at position p with direction d intersects sphere at (0,0,0) with radius r. returns intersection times along ray t1 and t2
bool CUtil::IntersectSphere(const glm::vec3& p, const glm::vec3& d, float r, float& t1, float& t2)
{
    float A = glm::dot(d, d);// d.dot(d);
    float B = 2.0f * glm::dot(d, p);// d.dot(p);
    float C = glm::dot(p, p) /* p.dot(p)*/ - r * r;

    float dis = B * B - 4.0f * A * C;

    if (dis < 0.0f)
        return false;

    float S = sqrt(dis);

    t1 = (-B - S) / (2.0f * A);
    t2 = (-B + S) / (2.0f * A);
    return true;
}
