// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CWindow.h"
#include "CPickItem.h"

CPickItem g_PickItem;

CPickItem::CPickItem() : CEvent()
{
}

void CPickItem::ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        glm::vec2 fC(xpos, ypos);
        glm::vec2 ndc = ((glm::vec2(fC.x / static_cast<float>(g_WindowMaxX), 1.0 - fC.y / static_cast<float>(g_WindowMaxY)) * 2.0f) - 1.0f);
        glm::mat4 worldPosition = glm::inverse(CWindow::GetMVP());

        glm::vec4 from = (worldPosition * glm::vec4(ndc, -1.0f, 1.0f));
        glm::vec4 to = (worldPosition * glm::vec4(ndc, 1.0f, 1.0f));

        from /= from.w; //perspective divide ("normalize" homogeneous coordinates)
        to /= to.w;

        glm::vec3 direction = (glm::vec3(to) - glm::vec3(from));

        const CModel* clickedObject = NULL;
        const Vertex* clickedVertex = NULL;
        float minDist = 99999.0f;

        for (const CModel* it : CWindow::GetModels())
        {
            float t1, t2;
            
            for (const CMesh& mesh : it->m_Meshes)
            {
                for (const Vertex& v : mesh.m_Vertex)
                {
                    if (CUtil::IntersectSphere((glm::vec3(from) - v.Position), direction, 0.10f, t1, t2))
                    {
                        //object i has been clicked. probably best to find the minimum t1 (front-most object)
                        if (t1 < minDist)
                        {
                            minDist = t1;
                            clickedVertex = &v;
                            clickedObject = it;
                        }
                    }
                }
            }
        }

        if (clickedObject)
            printf("Clicked on %s (vertex: %.2f, %.2f, %.2f).\n", clickedObject->m_ObjName.c_str(), clickedVertex->Position.x, clickedVertex->Position.y, clickedVertex->Position.z);
    }
}

