// by jrzanol
// 

#include <stdlib.h>
#include "CWindowGL.h"

int main(int argc, const char* argv[])
{
    CWindowGL window;

    if (!window.Initialize())
        return EXIT_FAILURE;

    while (!window.Render())
    {
    }

    window.Cleanup();
    return EXIT_SUCCESS;
}

