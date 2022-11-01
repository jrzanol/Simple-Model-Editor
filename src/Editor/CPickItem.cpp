// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CWindow.h"
#include "CPickItem.h"

std::vector<std::tuple<CMesh*, Vertex*>> CPickItem::g_ClickedObject;

CPickItem g_PickItem;

CPickItem::CPickItem() : CEvent()
{
}

void CPickItem::ProcessMouseDragEvent(GLFWwindow* window, float xoffset, float yoffset)
{
    if (g_ClickedObject.size() > 0)
    {
        xoffset *= 0.01f;
        yoffset *= 0.01f;

        for (auto& it : g_ClickedObject)
        {
            CMesh* mesh = std::get<0>(it);
            Vertex* vertex = std::get<1>(it);

            vertex->Position.x += xoffset;
            vertex->Position.y += yoffset;
            mesh->AllocBuffer();
        }
    }
}

void CPickItem::ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            if (CUtil::m_EditorType == 1)
            { // Create Vertices.

            }

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            glm::vec2 fC(xpos, ypos);
            glm::vec2 ndc = ((glm::vec2(fC.x / static_cast<float>(g_WindowMaxX), 1.0 - fC.y / static_cast<float>(g_WindowMaxY)) * 2.0f) - 1.0f);

            bool findedSphere = false;
            bool findedTriangle = false;

            if (CUtil::m_EditorType == 0 || CUtil::m_EditorType == 2)
            {
                for (CModel* it : CWindow::GetModels())
                {
                    glm::mat4 mvp = (CWindow::GetVP() * it->GetModelPos());
                    glm::mat4 worldPosition = glm::inverse(mvp);

                    glm::vec4 from = (worldPosition * glm::vec4(ndc, -1.0f, 1.0f));
                    glm::vec4 to = (worldPosition * glm::vec4(ndc, 1.0f, 1.0f));

                    from /= from.w; //perspective divide ("normalize" homogeneous coordinates)
                    to /= to.w;

                    glm::vec3 direction = (glm::vec3(to) - glm::vec3(from));
                    float minDistOfSphere = 9999999.f;

                    for (CMesh& mesh : it->m_Meshes)
                    {
                        for (Vertex& v : mesh.m_Vertex)
                        {
                            float t1, t2;

                            if (CUtil::IntersectSphere((glm::vec3(from) - v.Position), direction, 0.10f, t1, t2))
                            {
                                if (t1 < minDistOfSphere)
                                {
                                    g_ClickedObject.push_back(std::make_tuple(&mesh, &v));

                                    findedSphere = true;
                                    minDistOfSphere = t1;
                                }
                            }
                        }
                    }
                }
            }

            if (findedSphere && CUtil::m_EditorType == 2)
            { // Remove vertices.
                for (auto& it : g_ClickedObject)
                {
                    CMesh* mesh = std::get<0>(it);
                    Vertex* vertex = std::get<1>(it);

                    const auto itr = std::find(mesh->m_Vertex.begin(), mesh->m_Vertex.end(), *vertex);
                    int vertexId = std::distance(mesh->m_Vertex.begin(), itr);

                    std::vector<unsigned int> newIndices;

                    for (unsigned int id = 0; id < mesh->m_Indices.size(); id += 3)
                    {
                        if (mesh->m_Indices[id] != vertexId && mesh->m_Indices[id + 1] != vertexId && mesh->m_Indices[id + 2] != vertexId)
                        {
                            newIndices.push_back(mesh->m_Indices[id]);
                            newIndices.push_back(mesh->m_Indices[id + 1]);
                            newIndices.push_back(mesh->m_Indices[id + 2]);
                        }
                    }

                    mesh->m_Indices = newIndices;
                    //mesh->m_Vertex.erase(itr);
                    mesh->AllocBuffer();
                }
            }
            else if (!findedSphere || CUtil::m_EditorType == 1)
            {
                glm::vec3 outIntersectionPoint;
                int minDistOfSurface = 9999999;

                CMesh* mesh = NULL;
                unsigned int* indices = NULL;

                for (CModel* it : CWindow::GetModels())
                {
                    glm::mat4 mvp = (CWindow::GetVP() * it->GetModelPos());
                    glm::mat4 worldPosition = glm::inverse(mvp);

                    glm::vec4 from = (worldPosition * glm::vec4(ndc, -1.0f, 1.0f));
                    glm::vec4 to = (worldPosition * glm::vec4(ndc, 1.0f, 1.0f));

                    from /= from.w; //perspective divide ("normalize" homogeneous coordinates)
                    to /= to.w;

                    glm::vec3 direction = (glm::vec3(to) - glm::vec3(from));

                    for (CMesh& itmesh : it->m_Meshes)
                    {
                        for (unsigned int id = 0; id < itmesh.m_Indices.size(); id += 3)
                        {
                            glm::vec3 triangule[3];
                            triangule[0] = itmesh.m_Vertex[itmesh.m_Indices[id]].Position;
                            triangule[1] = itmesh.m_Vertex[itmesh.m_Indices[id + 1]].Position;
                            triangule[2] = itmesh.m_Vertex[itmesh.m_Indices[id + 2]].Position;

                            if (CUtil::RayIntersectsTriangle(glm::vec3(from), glm::vec3(to), triangule, outIntersectionPoint))
                            {
                                glm::vec3 diff = (glm::vec3(from) - outIntersectionPoint);

                                int dist = (int)sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                                if (dist < minDistOfSurface)
                                {
                                    mesh = &itmesh;
                                    indices = &itmesh.m_Indices[id];

                                    minDistOfSurface = dist;
                                    findedTriangle = true;
                                }
                            }
                        }
                    }
                }

                if (mesh && indices)
                {
                    if (CUtil::m_EditorType == 1)
                    {
                        const Vertex& A = mesh->m_Vertex[indices[0]];
                        const Vertex& B = mesh->m_Vertex[indices[1]];
                        const Vertex& C = mesh->m_Vertex[indices[2]];

                        Vertex centerOfTriangle; centerOfTriangle.Clear();
                        centerOfTriangle.Position = (glm::vec3((A.Position + B.Position + C.Position) / 3.f));
                        centerOfTriangle.TexCoords = (glm::vec2((A.TexCoords + B.TexCoords + C.TexCoords) / 3.f));

                        int vertexId = mesh->m_Vertex.size();
                        mesh->m_Vertex.push_back(centerOfTriangle);

                        unsigned int prevA = indices[0];
                        unsigned int prevB = indices[1];
                        unsigned int prevC = indices[2];

                        // A, B, G
                        indices[2] = vertexId;

                        // A, G, C
                        mesh->m_Indices.push_back(prevA);
                        mesh->m_Indices.push_back(vertexId);
                        mesh->m_Indices.push_back(prevC);

                        // B, C, G
                        mesh->m_Indices.push_back(prevB);
                        mesh->m_Indices.push_back(prevC);
                        mesh->m_Indices.push_back(vertexId);

                        mesh->AllocBuffer();
                    }
                    else
                    {
                        Vertex* A = &mesh->m_Vertex[indices[0]];
                        Vertex* B = &mesh->m_Vertex[indices[1]];
                        Vertex* C = &mesh->m_Vertex[indices[2]];

                        auto addVertexInClickedObj = [](CMesh* m, Vertex* v) {
                            bool finded = false;

                            for (const std::tuple<CMesh*, Vertex*> it : g_ClickedObject)
                            {
                                if (v == std::get<1>(it))
                                {
                                    finded = true;
                                    break;
                                }
                            }

                            if (!finded)
                                g_ClickedObject.push_back(std::make_tuple(m, v));
                        };

                        addVertexInClickedObj(mesh, A);
                        addVertexInClickedObj(mesh, B);
                        addVertexInClickedObj(mesh, C);
                    }
                }
            }

            if (!findedSphere && !findedTriangle)
                g_ClickedObject.clear();
        }
        else if (action == GLFW_RELEASE)
        {
        }
    }
}

bool CPickItem::IntersectSphere(stIntersect& out, bool onlySelected)
{
    bool finded = false;

    double xpos, ypos;
    glfwGetCursorPos(g_Window, &xpos, &ypos);

    glm::vec2 fC(xpos, ypos);
    glm::vec2 ndc = ((glm::vec2(fC.x / static_cast<float>(g_WindowMaxX), 1.0 - fC.y / static_cast<float>(g_WindowMaxY)) * 2.0f) - 1.0f);

    for (CModel* it : CWindow::GetModels())
    {
        if (onlySelected)
            it = CModel::g_SelectedModel;

        glm::mat4 mvp = (CWindow::GetVP() * it->GetModelPos());
        glm::mat4 worldPosition = glm::inverse(mvp);

        glm::vec4 from = (worldPosition * glm::vec4(ndc, -1.0f, 1.0f));
        glm::vec4 to = (worldPosition * glm::vec4(ndc, 1.0f, 1.0f));

        from /= from.w; //perspective divide ("normalize" homogeneous coordinates)
        to /= to.w;

        glm::vec3 direction = (glm::vec3(to) - glm::vec3(from));
        float minDistOfSphere = 9999999.f;

        for (CMesh& mesh : it->m_Meshes)
        {
            for (Vertex& v : mesh.m_Vertex)
            {
                float t1, t2;

                if (CUtil::IntersectSphere((glm::vec3(from) - v.Position), direction, 0.10f, t1, t2))
                {
                    if (t1 < minDistOfSphere)
                    {
                        out.m_Model = it;
                        out.m_Mesh = &mesh;
                        out.m_Vertex = &v;
                        out.m_Indices = NULL;

                        finded = true;
                        minDistOfSphere = t1;
                    }
                }
            }
        }

        if (onlySelected)
            break;
    }

    return finded;
}

bool CPickItem::IntersectSurface(stIntersect& out, bool onlySelected)
{
    bool finded = false;

    double xpos, ypos;
    glfwGetCursorPos(g_Window, &xpos, &ypos);

    glm::vec2 fC(xpos, ypos);
    glm::vec2 ndc = ((glm::vec2(fC.x / static_cast<float>(g_WindowMaxX), 1.0 - fC.y / static_cast<float>(g_WindowMaxY)) * 2.0f) - 1.0f);

    glm::vec3 outIntersectionPoint;
    int minDistOfSurface = 9999999;

    for (CModel* it : CWindow::GetModels())
    {
        if (onlySelected)
            it = CModel::g_SelectedModel;

        glm::mat4 mvp = (CWindow::GetVP() * it->GetModelPos());
        glm::mat4 worldPosition = glm::inverse(mvp);

        glm::vec4 from = (worldPosition * glm::vec4(ndc, -1.0f, 1.0f));
        glm::vec4 to = (worldPosition * glm::vec4(ndc, 1.0f, 1.0f));

        from /= from.w; //perspective divide ("normalize" homogeneous coordinates)
        to /= to.w;

        glm::vec3 direction = (glm::vec3(to) - glm::vec3(from));

        for (CMesh& itmesh : it->m_Meshes)
        {
            for (unsigned int id = 0; id < itmesh.m_Indices.size(); id += 3)
            {
                glm::vec3 triangule[3];
                triangule[0] = itmesh.m_Vertex[itmesh.m_Indices[id]].Position;
                triangule[1] = itmesh.m_Vertex[itmesh.m_Indices[id + 1]].Position;
                triangule[2] = itmesh.m_Vertex[itmesh.m_Indices[id + 2]].Position;

                if (CUtil::RayIntersectsTriangle(glm::vec3(from), glm::vec3(to), triangule, outIntersectionPoint))
                {
                    glm::vec3 diff = (glm::vec3(from) - outIntersectionPoint);

                    int dist = (int)sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                    if (dist < minDistOfSurface)
                    {
                        out.m_Model = it;
                        out.m_Mesh = &itmesh;
                        out.m_Vertex = NULL;
                        out.m_Indices = &itmesh.m_Indices[id];

                        finded = true;
                        minDistOfSurface = dist;
                    }
                }
            }
        }

        if (onlySelected)
            break;
    }

    return finded;
}

