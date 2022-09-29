// by jrzanol
// 

#include "stdafx.h"

#include "CEvent.h"
#include "CWindowGL.h"

int main(int argc, const char* argv[])
{
    CWindowGL window;

    if (!window.Initialize())
        return EXIT_FAILURE;

    while (!window.Render())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        CUtil::m_DeltaTime = currentFrame - CUtil::m_LastTime;
        CUtil::m_LastTime = currentFrame;

        static int lastSecTimer = -1;
        static int lastMinTimer = -1;

        int currentSecTimer = static_cast<int>(currentFrame);
        int currentMinTimer = static_cast<int>(currentFrame / 60.f);

        if (currentSecTimer != lastSecTimer)
        {
            for (const auto& it : g_EventList)
                it->ProcessSecTimer();
            
            lastSecTimer = currentSecTimer;
        }

        if (currentMinTimer != lastMinTimer)
        {
            for (const auto& it : g_EventList)
                it->ProcessMinTimer();

            lastMinTimer = currentMinTimer;
        }
    }

    window.Cleanup();
    return EXIT_SUCCESS;
}

