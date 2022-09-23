// by jrzanol
//

#include "stdafx.h"
#include "CObject.h"

int CObject::g_ListCounter = 0;
ObjectData CObject::g_List[MAX_OBJECT];

ObjectData* CObject::LoadModel(std::string file)
{
    ObjectData* it = std::find(g_List, &g_List[g_ListCounter], file);

    if (it == &g_List[g_ListCounter])
    { // Load .obj file.
        rapidobj::Result result = rapidobj::ParseFile(file);
        if (result.error)
        {
            std::cout << result.error.code.message() << '\n';
            return NULL;
        }

        bool success = rapidobj::Triangulate(result);
        if (!success)
        {
            std::cout << result.error.code.message() << '\n';
            return NULL;
        }

        /*g_List[g_ListCounter] = */std::make_tuple(file, result);
        it = &g_List[g_ListCounter];

        g_ListCounter++;
    }
    
    return it;
}

CDrawableObject::CDrawableObject()
{
}

CDrawableObject::~CDrawableObject()
{
}

void CDrawableObject::CreateBuffers()
{
}

void CDrawableObject::Draw(GLuint)
{
}

