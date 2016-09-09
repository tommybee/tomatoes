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
#include "SDL.h"
#include "SDL_opengl.h"
#include "pathfinder.h"
#include "tilemap.h"



// Helper function which returns a pointer to the last node on a list
inline NODE *last_node(list <NODE> &l) {
	return &(*(--l.end()));
}


// Helper function which returns the tile distance between two points on the map
int tile_dist(int px1, int py1, int px2, int py2) {
	int dx = abs(px1 - px2);
	int dy = abs(py1 - py2);

	return ((dx + dy) * 10);
}


// Find the path between (sx,sy) and (dx,dy). Returns PATH_FAILED or PATH_OK.
int PATHFINDER::find_path(int sx_, int sy_, int dx_, int dy_) {
	// Check that the target tile is reachable
	if(map_solid(dx_, dy_))
		return PATH_FAILED;


	// Helper array which tells us if a tile is on the open nodes list, or
	// on the closed nodes list.
	// 0 == It isn't on the lists
	// 1 == It is on the closed list
	// 2 == It is on the open list
	int map_nodes[MAP_W][MAP_H];

	// Initialize the pathfinder
	dx = dx_;
	dy = dy_;
	sx = sx_;
	sy = sy_;
	nodes_open.clear();
	nodes_closed.clear();
	path.clear();
	for(int fyy = 0; fyy < MAP_H; fyy++)
		for(int fxx = 0; fxx < MAP_W; fxx++)
			map_nodes[fxx][fyy] = 0;


	// Add the starting point to the open nodes
	NODE st;
	st.clear();
	st.x = sx;
	st.y = sy;
	nodes_open.push_back(st);
	map_nodes[sx][sy] = 2;

	// Current node
	NODE current;
	current.clear();

	// State of the search
	int search_state = -1;

	// Search the path
	while(search_state == -1) {
		// Sort the open list by F value
		nodes_open.sort();

		// Current tile shall be the one with the lowest F (the first after sorting)
		current = *(nodes_open.begin());

		// Move it to the closed list
		nodes_open.pop_front();
		nodes_closed.push_back(current);
		map_nodes[current.x][current.y] = 1;

		// Look for adjacent reachable tiles
		for(int f=0; f < 4; f++) {
			int mx,my;
			switch(f) {
				default:
				case 0: mx = current.x; my = current.y - 1; break;
				case 1: mx = current.x + 1; my = current.y; break;
				case 2: mx = current.x; my = current.y + 1; break;
				case 3: mx = current.x - 1; my = current.y; break;
			}

			// Check the tile
			if(map_solid(mx, my))
				continue;			// Solid tile, skip it

			// Check if the tile is on the closed nodes list
			if(map_nodes[mx][my] == 1)
				continue;
/*			bool is_closed = false;
			list<NODE>::iterator i;
			for(i = nodes_open.begin(); i != nodes_open.end(); ++i) {
				if((*i).x == mx && (*i).y == my) {
					is_closed = true;
					break;
				}
			}

			if(is_closed)
				continue;			// It was on the closed nodes list, skip it
*/

			// Check if the tile isn't on the open nodes list
			if(map_nodes[mx][my] == 0) {
				// Add a new node and make the current tile it's parent
				NODE n;
				n.clear();
				n.x = mx;
				n.y = my;
				n.g = current.g + 10;				// No diagonal movement
				n.h = tile_dist(mx, my, dx, dy);	// Manhattan distance
				n.f = n.g + n.h;					// F = G + H
				n.parent = last_node(nodes_closed);
				nodes_open.push_back(n);
				map_nodes[mx][my] = 2;

				// If we just added our target tile (dx, dy), we've found the path.
				// Bail out.
				if(mx == dx && my == dy) {
					search_state = PATH_OK;
					break;
				}
			}

		}

		// If the list of open nodes is empty, we can't find the path
		if(nodes_open.empty()) {
			search_state = PATH_FAILED;
			break;
		}
	}

	// At this point, the path is either found or failed
	if(search_state == PATH_OK) {
		// Path was found

		// The destination node is the last node added to the open list
		NODE *dest = last_node(nodes_open);

		// Make sure it is so
		if(dest->x != dx || dest->y != dy)
			printf("find_path() error:\nThe path destination is (%d,%d) while it should be (%d,%d)!\n", dest->x, dest->y, dx, dy);

		// Add the destination to the path
		PATHPOINT point;
		point.x = dx;
		point.y = dy;
		path.insert(path.begin(), point);

		// Traverse from the destination along the path, and save the path points
		NODE *p = dest->parent;
		while(p) {
			PATHPOINT point;
			point.x = p->x;
			point.y = p->y;
			path.insert(path.begin(), point);
			p = p->parent;
		}

		// Remove the start point from the path as it isn't needed.
		path.erase(path.begin());

		// Clear the nodes list as we don't need them any more
		nodes_open.clear();
		nodes_closed.clear();
		return PATH_OK;
	}
	else {
		// Path was not found
		return PATH_FAILED;
	}
}


// Operators for STL list
NODE &NODE::operator=(const NODE &other) {
	this->x = other.x;
	this->y = other.y;
	this->f = other.f;
	this->g = other.g;
	this->h = other.h;
	this->parent = other.parent;
	return *this;
}

NODE *NODE::operator=(const NODE *other) {
	this->x = other->x;
	this->y = other->y;
	this->f = other->f;
	this->g = other->g;
	this->h = other->h;
	this->parent = other->parent;
	return this;
}



int NODE::operator==(const NODE &other) const {
	if(this->x != other.x || this->y != other.y) return 0;
	if(this->f != other.f || this->g != other.g || this->h != other.h) return 0;
	if(this->parent != other.parent) return 0;
	return 1;
}


// For sorting
int NODE::operator<(const NODE &other) const {
	if(this->f < other.f) return 1;
	return 0;
}

