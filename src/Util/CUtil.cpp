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
    layout(location = 5) in vec3 a_barycentric;
    
    out vec2 TexCoords;
    varying vec3 vbc;

    uniform mat4 u_view;
    uniform mat4 u_proj;
    uniform mat4 u_model;

    void main() {
        TexCoords = a_texcoords;
        vbc = a_barycentric;
        
        gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
    }
)glsl";

const char* CUtil::m_FragmentShader = R"glsl(
    #version 430
    #extension GL_OES_standard_derivatives : enable
    
    precision mediump float;
    varying vec3 vbc;
    
    const float lineWidth = 1.0005;
    const vec3 color = vec3(0.7, 0.7, 0.7);
    
    out vec4 FragColor;
    in vec2 TexCoords;
    
    uniform sampler2D texture_diffuse1;
    uniform sampler2D texture_specular1;
    uniform sampler2D texture_normal1;
    
    float edgeFactor() {
        vec3 d = fwidth(vbc);
        vec3 f = step(d * lineWidth, vbc);
        return min(min(f.x, f.y), f.z);
    }
    
    void main() {
        //gl_FragColor = vec4(min(vec3(edgeFactor()), color), 1.0);
        gl_FragColor = min(vec4(min(vec3(edgeFactor()), color), 1.0), mix(texture(texture_diffuse1, TexCoords), texture(texture_specular1, TexCoords), texture(texture_normal1, TexCoords)));

        //gl_FragColor = vec4(min(vec3(edgeFactor()), mix(texture(texture_diffuse1, TexCoords), texture(texture_specular1, TexCoords), texture(texture_normal1, TexCoords)), 1.0);
        //FragColor = mix(texture(texture_diffuse1, TexCoords), texture(texture_specular1, TexCoords), texture(texture_normal1, TexCoords));
    }
)glsl";

