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

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <list>
#include <vector>
using namespace std;


// Result values
#define			PATH_FAILED		0
#define			PATH_OK			1


// Node class
class NODE {
public:
	int x, y;					// Node position in the map
	int f, g, h;				// F, G and H costs
	NODE *parent;				// Parent node

	// Clear the node
	void clear() {
		x = y = 0;
		f = g = h = 0;
		parent = NULL;
	}

	// Operators for STL list
	NODE &operator=(const NODE &other);
	NODE *operator=(const NODE *other);
	int operator==(const NODE &other) const;
	int operator<(const NODE &other) const;
};



// Structure to save the path points
struct PATHPOINT {
	int x, y;							// Position on the map
};


// Pathfinder class
class PATHFINDER {
public:
	int dx, dy;							// Destination point
	int sx, sy;							// Starting point

	list<NODE> nodes_open;				// Open nodes list
	list<NODE> nodes_closed;			// Closed nodes list

	vector<PATHPOINT> path;				// The finished path


	// Find the path between (sx,sy) and (dx,dy). Returns PATH_FAILED or PATH_OK.
	int find_path(int sx, int sy, int dx, int dy);

};



#endif

