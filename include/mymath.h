/*************************************************************************

                         "I Have No Tomatoes"
                  Copyright (c) 2004, Mika Halttunen

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


 Mika Halttunen <lsoft@mbnet.fi>

*************************************************************************/

#ifndef MYMATH_H
#define MYMATH_H

// Macros for table access
#define SIN(x) sintable[(int)(x*10)]
#define COS(x) costable[(int)(x*10)]
#define TAN(x) tantable[(int)(x*10)]

// Look-up tables
extern float sintable[3601];
extern float costable[3601];
extern float tantable[3601];

// PI....
//#ifndef M_PI
//#define M_PI 3.1415926536f
//#endif

// Return a random number
#define RAND(min, max) rand()%((max)+1-(min))+(min)

// Return a random floating point number
float RANDF(float min, float max);

// Initialise math tables
void init_math();

// General math routines
float add_angle(float angle, float add);

// Distance between two points
float distance(float x1, float y1, float x2, float y2);

// Move x and y along angle at given speed
void move_xy(float speed, float angle, float *x, float *y);

// Compute angle between two points
float compute_angle(float x1, float y1, float x2, float y2);

// Compute distance between two angles
float compute_angle_distance(float angle1, float angle2);

// Conversions
inline float radian_to_degree(float r);
inline float degree_to_radian(float d);


#endif


