"I Have No Tomatoes" Source code
Copyright (c) 2004, Mika Halttunen

    http://tomatoes.sourceforge.net


-----------------------------------------------------------------------
 LICENSE AGREEMENT
-----------------------------------------------------------------------
 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute
 it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must
    not claim that you wrote the original software. If you use this
    software in a product, an acknowledgment in the product documentation
    would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must
    not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.



-----------------------------------------------------------------------
 COMPILING THE SOURCES
-----------------------------------------------------------------------
You should be able to compile this code with zero (or few) modifications
using GNU GCC compilers (or compatible). For the Windows version, you can
use use MinGW (www.mingw.org).

First, make sure you have installed these libraries and their dependancies:
* SDL (v1.2.7)
* SDL_Image (v1.2.2)
* FMOD (v3.7.2)

I used the beforementioned versions, but you can build the game using other versions
too. FMOD must be 3.7 or newer to compile without modifications.

When you're done installing the libs, open the 'makefile' and change the

# SDL include directory
SDL_INCLUDES = c:\mingw\include\SDL

lines to point to your SDL include file directory. Then you can type 'make' and
the game should build all right.




If you have any questions about this, feel free to contact me at lsoft@mbnet.fi
When you make something cool contributions or bug fixes to the code, contact me
please. And be sure to visit http://tomatoes.sourceforge.net for the latest stuff!
