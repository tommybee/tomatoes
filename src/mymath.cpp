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

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mymath.h"

// Look-up tables
float sintable[3601];
float costable[3601];
float tantable[3601];

// Return a random floating point number
float RANDF(float min, float max) {
	float range = max - min;
	float num = range * rand() / RAND_MAX;
	return(num+min);
}

// Initialise math tables
void init_math() {

	for(int f=0; f<3601; f++) {
		float a = (float)f;
		sintable[f] = (float)sin(((a/10)/360)*2*M_PI);
		costable[f] = (float)cos(((a/10)/360)*2*M_PI);
		tantable[f] = (float)tan(degree_to_radian(a/10));
	}

	srand(time(NULL));
}


float add_angle(float angle, float add) {

	angle += add;
	if(angle > 360)
		angle -= 360;
	else if(angle < 0)
		angle += 360;

	return angle;
}


// Distance between two points
float distance(float x1, float y1, float x2, float y2) {

	float dx = x2-x1;
	float dy = y2-y1;

	return (float)sqrt((dx*dx)+(dy*dy));
}


// Move x and y along angle at given speed
void move_xy(float speed, float angle, float *x, float *y) {

	float ans;
	if(angle <= 90) {
		ans = COS(angle);
		ans *= speed;
		*y = -ans;
		ans = SIN(angle);
		ans *= speed;
		*x = ans;
	}
	else if(angle <= 180) {
		angle -= 90;
		ans = SIN(angle);
		ans *= speed;
		*y = ans;
		ans = COS(angle);
		ans *= speed;
		*x = ans;
	}
	else if(angle <= 270) {
		angle -= 180;
		ans = COS(angle);
		ans *= speed;
		*y = ans;
		ans = SIN(angle);
		ans *= speed;
		*x = -ans;
	}
	else {
		angle -= 270;
		ans = SIN(angle);
		ans *= speed;
		*y = -ans;
		ans = COS(angle);
		ans *= speed;
		*x = -ans;
	}
}


// Compute angle between two points
float compute_angle(float x1, float y1, float x2, float y2) {

	double dx,dy;
	double ans = 0.0;

	dx = x1 - x2;
	dy = y1 - y2;

	if(dx >= 0 && dy >= 0) {
		ans = atan2(dy, dx);
		ans = 90 - ans;
	}
	else if(dx >= 0 && dy < 0) {
		dy = -dy;
		ans = atan2(dy, dx);
		ans += 90;
	}
	else if(dx < 0 && dy < 0) {
		dx = -dx;
		dy = -dy;
		ans = atan2(dx, dy);
		ans += 180;
	}
	else if(dx < 0 && dy >= 0) {
		dx = -dx;
		ans = atan2(dy, dx);
		ans += 270;
	}

	return radian_to_degree(ans);
}


// Compute distance between two angles
float compute_angle_distance(float angle1, float angle2) {

	float ans1,ans2;

	if(angle1 == angle2)
		return 0;
	else if(angle1 > angle2) {
		ans1 = angle1 - angle2;
		angle2 += 360;
		ans2 = angle2 - angle1;
		if(ans1 < ans2)
			return ans1;
		else
			return ans2;
	}
	else {
		ans1 = angle2 - angle1;
		angle1 += 360;
		ans2 = angle1 - angle2;
		if(ans1 < ans2)
			return ans1;
		else
			return ans2;
	}
}


// Conversions
float radian_to_degree(float r) {
	return r * (360.0f/(2.0f*M_PI));
}

float degree_to_radian(float d) {
	return d * ((2.0f*M_PI)/360.0f);
}


