/*
Open War: an open-source Total War clone, written in C++ using Ogre3D.
Copyright (C) 2012 William James Dyce

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
