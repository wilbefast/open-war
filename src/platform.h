#ifndef PLATFORM_HPP_INCLUDED
#define PLATFORM_HPP_INCLUDED

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    #define WIN32_LEAN_AND_MEAN
    #include "windows.h"
    #define MAIN    \
        INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
    #define ERROR(msg)   \
        MessageBoxA(NULL, msg, "An exception has occurred!", \
            MB_OK | MB_ICONERROR | MB_TASKMODAL)
#else
    #define MAIN    \
        int main(int argc, char **argv, char** envp)
    #define ERROR(msg)   \
        fprintf(stderr, "An exception has occurred: %s\n", msg)
#endif

#endif // PLATFORM_HPP_INCLUDED
