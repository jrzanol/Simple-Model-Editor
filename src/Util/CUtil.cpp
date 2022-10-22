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
    float A = glm::dot(d, d);
    float B = 2.0f * glm::dot(d, p);
    float C = glm::dot(p, p) - r * r;
    float dis = B * B - 4.0f * A * C;

    if (dis < 0.0f)
        return false;

    float S = sqrt(dis);

    t1 = (-B - S) / (2.0f * A);
    t2 = (-B + S) / (2.0f * A);
    return true;
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool CUtil::RayIntersectsTriangle(glm::vec3 rayOrigin, glm::vec3 rayVector, glm::vec3* inTriangle, glm::vec3& outIntersectionPoint)
{
    const float EPSILON = 0.0000001f;

    glm::vec3 vertex0 = inTriangle[0];
    glm::vec3 vertex1 = inTriangle[1];
    glm::vec3 vertex2 = inTriangle[2];
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;

    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;

    h = glm::cross(rayVector, edge2);// rayVector.crossProduct(edge2);
    a = glm::dot(edge1, h);// edge1.dotProduct(h);

    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.

    f = 1.0f / a;
    s = rayOrigin - vertex0;
    u = f * glm::dot(s, h);// s.dotProduct(h);

    if (u < 0.0f || u > 1.0f)
        return false;

    q = glm::cross(s, edge1);// s.crossProduct(edge1);
    v = f * glm::dot(rayVector, q);// rayVector.dotProduct(q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);// edge2.dotProduct(q);
    if (t > EPSILON) // ray intersection
    {
        outIntersectionPoint = rayOrigin + rayVector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}
