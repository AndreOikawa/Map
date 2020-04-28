#include <algorithm>
#include <fstream>
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

enum Tile { WALL, END, START, FLOOR};

class Cell {
public:
	int _x;
	int _y;
	Tile _source;
	Cell() {}
	Cell(int x, int y) : _x(x), _y(y) {}
	Cell operator+(const Cell& c) {
		return Cell(c._x + _x, c._y + _y);
	}
	Cell operator-(const Cell& c) {
		return (Cell(_x - c._x, _y - c._y));
	}
};

void save(const vector<vector<int>>& grid, string name) {
	ofstream file;
	file.open(name, ios::trunc | ios::out);
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size() - 1; j++) {
			file << grid[i][j] << ",";
		}
		file << grid[i][grid[i].size() - 1] << endl;
	}
	file.close();
}

void print(const vector<vector<int>>& grid) {
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size() - 1; j++) {
			cout << grid[i][j] << ",";
		}
		cout << grid[i][grid[i].size() - 1] << endl;
	}
}

Cell getEmptyCell(const vector<vector<int>>& grid) {
	int x = grid.size();
	int y = grid[0].size();

	Cell cell;
	do {
		cell = Cell(rand() % x, rand() % y);	
	} while(grid[cell._x][cell._y] != WALL);
	return cell;
}

bool isInside(vector<vector<int>>& grid, Cell cell) {
	int x = grid.size();
	int y = grid[0].size();
	return cell._x >= 0 && cell._x < x && cell._y >= 0 && cell._y < y;
}

int getDistance(Cell a, Cell b) {
	return abs(a._x - b._x) + abs(a._y - b._y);
}

bool isValidFloor(vector<vector<int>>& grid, Cell cell) {
	int x = grid.size();
	int y = grid[0].size();

	// is inside grid
	if (isInside(grid, cell)) {
		// is wall
		if (grid[cell._x][cell._y] == WALL) {
			int adjFloor = 0;
			for (int i = -1; i <= 1; i += 2) {
				// for (int j = -1; j <= 1; j+= 2) {
				int ax = cell._x + i;
				// has valid adjacent floor
				if (ax >= 0 && ax < x && grid[ax][cell._y] != WALL) {
					adjFloor++;
				}
				int ay = cell._y + i;
				if (ay >= 0 && ay < y && grid[cell._x][ay] != WALL) {
					adjFloor++;
				}
				// }
			}
			if (adjFloor < 2) {
				return true;
			} 
			// else if ((cell._x == 0 || cell._y == 0 || cell._x == x - 1 || cell._y == y - 1)
			// 			&& adjFloor > 0) {
			// 	return true;
			// }

		}
	}

	return false;
}

void generateMap(vector<vector<int>>& grid, bool easy) {
	int x = grid.size();
	int y = grid[0].size();
	queue<Cell> floors;

	// start
	
		Cell start = getEmptyCell(grid);
		Cell end = getEmptyCell(grid);

		while(getDistance(start, end) < max(x, y)) {
			start = getEmptyCell(grid);
			end = getEmptyCell(grid);
		}

		start._source = START;
		floors.push(start);
		grid[start._x][start._y] = START;

	// end
	
		end._source = END;
		floors.push(end);
		grid[end._x][end._y] = END;
	

	// floor
	{
		vector<Cell> directions;
		directions.push_back(Cell(0,-1));
		directions.push_back(Cell(0,1));
		directions.push_back(Cell(1,0));
		directions.push_back(Cell(-1,0));
		random_shuffle(directions.begin(), directions.end());

		while(!floors.empty()) {
			Cell floor = floors.front();
			floors.pop();
			for (auto it = directions.begin(); it != directions.end(); it++) {
				Cell tryFloor = floor + *it;
				if (isValidFloor(grid, tryFloor)) {
					grid[tryFloor._x][tryFloor._y] = floor._source;//FLOOR;
					tryFloor._source = floor._source;
					// cout << (floor._source == START ? "start" : "end") << endl;
					floors.push(tryFloor);
					floors.push(floor);
					break;
				}
			}
			random_shuffle(directions.begin(), directions.end());
		}

		{
			vector<Cell> walls;
			vector<Cell> floorTile;
			for (int i = 0; i < x; i++) {
				for (int j = 0; j < y; j++) {
					if (grid[i][j] == WALL) {
						walls.push_back(Cell(i,j));
					} else {
						floorTile.push_back(Cell(i,j));
					}
				}
			}

			random_shuffle(walls.begin(), walls.end());
			int numWalls = walls.size() / 2;
			bool singleSoln = false;

			for (int i = 0; i < walls.size(); i++) {
				if (numWalls <= 0 && singleSoln) {
					break;
				}
				numWalls--;
				Cell wall = walls[i];

				vector<Cell> s;
				vector<Cell> e;

				// bool s = false;
				// bool e = false;

				// cout << "from: " << wall._x << " " << wall._y << endl;
				for (auto it = directions.begin(); it != directions.end(); it++) {
					Cell tryWall = wall + *it;
					if (isInside(grid, tryWall)) {
						if (grid[tryWall._x][tryWall._y] == END) e.push_back(tryWall);
						if (grid[tryWall._x][tryWall._y] == START) s.push_back(tryWall);
					}
					// if (s.size() > 0 && e.size() > 0) {
					// 	// cout << "found" << endl;
					// 	// grid[wall._x][wall._y] = START;
					// 	// break;
					// }
				}
				if (s.size() == 1 && e.size() == 1 && !singleSoln) {
					grid[wall._x][wall._y] = FLOOR;
					singleSoln = true;
					// break;
				} else if (s.size() + e.size() == 2 && numWalls > 0 && easy) {
					Cell a;
					Cell b;
					if (s.size() > 0) {
						a = s[0];
						b = s[1];
					} else if (e.size() > 0) {
						a = e[0];
						b = e[1];
					}
					Cell c = wall + (a - wall + b - wall);
					// cout << "a: " << a._x << " " << a._y << endl;
					// cout << "b: " << b._x << " " << b._y << endl;
					// cout << "c: " << c._x << " " << c._y << endl;
					if (grid[c._x][c._y] == WALL) {
						grid[wall._x][wall._y] = FLOOR;
					}
					
				}
			}

			for (int i = 0; i < floorTile.size(); i++) {
				grid[floorTile[i]._x][floorTile[i]._y] = FLOOR;
			}
		}

		grid[start._x][start._y] = START;
		grid[end._x][end._y] = END;

	}

}

void backtrack(vector<vector<int>>& grid, vector<vector<int>>& copy, Cell end, int step) {
	vector<Cell> directions;
	directions.push_back(Cell(0,-1));
	directions.push_back(Cell(0,1));
	directions.push_back(Cell(1,0));
	directions.push_back(Cell(-1,0));

	Cell curr = end;
	while (step != -1) {
		bool changed = false;
		// cout << "back" << endl;
		for (auto it = directions.begin(); it != directions.end(); it++) {
			Cell adjacent = curr + *it;
			// cout << adjacent._x << " " << adjacent._y << endl;
			if (isInside(grid, adjacent) && copy[adjacent._x][adjacent._y] == step) {
				grid[adjacent._x][adjacent._y] = step;
				step++;
				curr = adjacent;
				// cout << "found" << endl;
				changed = true;
				break;
			}
		}
		if (!changed) {
			cout << "bad tile" << endl;
			break;
		}
	}

	cout << "Solved" << endl;
	print(grid);
	save(grid, "solution.txt");


}

void solveMap(vector<vector<int>>& grid) {
	int x = grid.size();
	int y = grid[0].size();
	vector<vector<int>> copy = grid;

	Cell start = Cell(-1,-1);

	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			if (copy[i][j] == START) {
				start._x = i;
				start._y = j;
				copy[i][j] = -1;
				i = x;
				j = y;
			}
		}
	}

	queue<Cell> path;
	path.push(start);

	vector<Cell> directions;
	directions.push_back(Cell(0,-1));
	directions.push_back(Cell(0,1));
	directions.push_back(Cell(1,0));
	directions.push_back(Cell(-1,0));

	while(!path.empty()) {
		Cell curr = path.front();
		path.pop();

		for (auto it = directions.begin(); it != directions.end(); it++) {
			Cell adjacent = curr + *it;
			if (!isInside(grid, adjacent)) continue;

			if (copy[adjacent._x][adjacent._y] == FLOOR) {
				copy[adjacent._x][adjacent._y] = copy[curr._x][curr._y] - 1;
				path.push(adjacent);
			} else if (copy[adjacent._x][adjacent._y] == END) {
				backtrack(grid, copy, adjacent, copy[curr._x][curr._y]);
				cout << "Solved Map" << endl;
				print(copy);
				save(copy, "solving.txt");
			}
		}
	}

}

vector<vector<int>> readMap(string path) {
	vector<vector<int>> grid;
	ifstream file(path);
	if (file.is_open()) {
		for (string line; getline(file,line);) {
  			vector<int> row;
  			
  			size_t pos = 0;

  			while ((pos = line.find(",") != string::npos)) {
  				row.push_back(stoi(line.substr(0, pos)));
  				line.erase(0, pos + 1);
  			}

  			row.push_back(stoi(line));
  			grid.push_back(row);
		}
		file.close();
	}
	return grid;
}

int main(int argc, char *argv[]) {
	// cout << argc << endl;
	if (argc != 3 && argc != 1) {
		cout << "Require grid size:  x y" << endl << "Default: 10x10" << endl;
		return 1;
	}

	int x = 10;
	int y = 10;
	bool easy = false;
	if (argc >= 3) {
		istringstream(argv[1]) >> x;
		istringstream(argv[2]) >> y;
		if (x == 0) x = 10;
		if (y == 0) y = 10;
		cout << "Grid size " << x << "x" << y << endl;
	}
	if (argc == 4) {
		easy = true;
	}

	vector<vector<int>> grid(x,vector<int>(y));
	srand (time(NULL));

	// grid = readMap("file.txt");
	generateMap(grid, easy);

	print(grid);
	save(grid, "unsolved.txt");
	solveMap(grid);
}