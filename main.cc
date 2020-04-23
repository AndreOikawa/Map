#include <algorithm>
#include <iostream>		/* cout */
#include <queue>
#include <string>		
#include <sstream>		
#include <vector>

#include <math.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

// 0 = floor
// 1 = end
// 2 = start
// 3 = wall


class Cell {
public:
	int _x;
	int _y;
	Cell() {}
	Cell(int x, int y) : _x(x), _y(y) {}
	Cell operator+(const Cell& c) {
		return Cell(c._x + _x, c._y + _y);
	}
};

void print(const vector<vector<int>>& grid) {
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[0].size(); j++) {
			cout << grid[i][j];
		}
		cout << endl;
	}
}

Cell getEmptyCell(const vector<vector<int>>& grid) {
	int x = grid.size();
	int y = grid[0].size();

	Cell cell;
	do {
		cell = Cell(rand() % x, rand() % y);	
	} while(grid[cell._x][cell._y] != 0);
	return cell;
}

bool isValidWall(vector<vector<int>>& grid, Cell cell) {
	int x = grid.size();
	int y = grid[0].size();

	// is inside grid
	if (cell._x >= 0 && cell._x < x && cell._y >= 0 && cell._y < y) {
		// is floor
		if (grid[cell._x][cell._y] == 0) {
			int adjFloor = 0;
			for (int i = -1; i <= 1; i += 2) {
				for (int j = -1; j <= 1; j+= 2) {
					int ax = cell._x + i;
					int ay = cell._y + j;
					// has valid adjacent floor
					if (ax >= 0 && ax < x && ay >= 0 && ay < y && grid[ax][ay] == 0) {
						adjFloor++;
					}
				}
			}
			if (adjFloor > 2) {
				return true;
			} else if ((cell._x == 0 || cell._y == 0 || cell._x == x - 1 || cell._y == y - 1)
						&& adjFloor > 0) {
				return true;
			}

		}
	}

	return false;
}

void generateMap(vector<vector<int>>& grid) {
	int x = grid.size();
	int y = grid[0].size();

	// start
	{
		Cell start = getEmptyCell(grid);
		grid[start._x][start._y] = 2;
	}

	// end
	{
		Cell end = getEmptyCell(grid);
		grid[end._x][end._y] = 1;
	}

	// walls
	{
		int totalTiles = x * y;
		int numWallSeeds = sqrt(totalTiles) - 1;

		queue<Cell> walls;

		for (int i = 0; i <= numWallSeeds; i++) {
			Cell wall = getEmptyCell(grid);
			grid[wall._x][wall._y] = 3;
			walls.push(wall);
		}

		vector<Cell> directions;
		directions.push_back(Cell(-1,-1));
		directions.push_back(Cell(-1,1));
		directions.push_back(Cell(1,-1));
		directions.push_back(Cell(1,1));
		random_shuffle(directions.begin(), directions.end());

		while (!walls.empty()) {
			Cell wall = walls.front();
			walls.pop();
			for (auto it = directions.begin(); it != directions.end(); it++) {
				Cell tryWall = wall + *it;
				if (isValidWall(grid, tryWall)) {
					grid[tryWall._x][tryWall._y] = 3;
					walls.push(tryWall);
				}
			}
			random_shuffle(directions.begin(), directions.end());
		}
	}

}

int main(int argc, char *argv[]) {
	if (argc != 3 && argc != 1) {
		cout << "Require grid size:  x y" << endl << "Default: 10x10" << endl;
		return 1;
	}

	int x = 10;
	int y = 10;

	if (argc == 3) {
		istringstream(argv[1]) >> x;
		istringstream(argv[2]) >> y;
		if (x == 0) x = 10;
		if (y == 0) y = 10;
		cout << "Grid size " << x << "x" << y << endl;
	}

	vector<vector<int>> grid(x,vector<int>(y));
	srand (time(NULL));

	generateMap(grid);

	print(grid);
}