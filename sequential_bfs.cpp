#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <queue>
#include <stack>
#define MAX_LINE 25

using namespace std;

class Node{
public:
	int x, y;	// x, y coordinate
	Node(int x, int y);
	Node(const Node &c);
};

Node::Node(int x, int y){
	this->x = x;
	this->y = y;
}

Node::Node(const Node &c){
	this->x = c.x;
	this->y = c.y;
}

int main(int argc, char **argv){
	if(argc != 2){
		cerr << "Incorrect input. Usage: " << argv[0] << " <map>\n";
	}

	int x_size, y_size;	// map size
	int x_start, y_start, x_end, y_end;	// start and end point
	int **map;

	char buf[MAX_LINE];
	char *pch;

	/***** reading file *****/
	fstream file;
	file.open(argv[1], ios::in);

	/***** map initialization *****/
	file.getline(buf, MAX_LINE, '\n');
	pch = strtok(buf, " ");
	x_size = atoi(pch);
	pch = strtok(NULL, " ");
	y_size = atoi(pch);

	x_size++;
	y_size++;

	map = new int*[x_size];
	for(int i = 0; i < x_size; i++){
		map[i] = new int[y_size];
		for(int j = 0; j < y_size; j++)
			map[i][j] = -1;
	}

	file.getline(buf, MAX_LINE, '\n');
	pch = strtok(buf, " ");
	x_start = atoi(pch);
	pch = strtok(NULL, " ");
	y_start = atoi(pch);

	file.getline(buf, MAX_LINE, '\n');
	pch = strtok(buf, " ");
	x_end = atoi(pch);
	pch = strtok(NULL, " ");
	y_end = atoi(pch);

	/***** blockages initialization *****/
	while(file.getline(buf, MAX_LINE, '\n') != NULL){
		int x, y, _x, _y;
		pch = strtok(buf, " ");
		x = atoi(pch);
		pch = strtok(NULL, " ");
		y = atoi(pch);
		pch = strtok(NULL, " ");
		_x = atoi(pch);
		pch = strtok(NULL, " ");
		_y = atoi(pch);

		if(x <= _x && y <= _y)
			for(int i = x; i <= _x; i++)
				for(int j = y; j <= _y; j++)
					map[i][j] = -2;
		else if(x <= _x && y >= _y)
			for(int i = x; i <= _x; i++)
				for(int j = y; j >= _y; j--)
					map[i][j] = -2;
		else if(x >= _x && y <= _y)
			for(int i = _x; i >= x; i--)
				for(int j = y; j <= _y; j++)
					map[i][j] = -2;
		else
			for(int i = _x; i >= x; i--)
				for(int j = _y; j >= y; j--)
					map[i][j] = -2;
	}

	file.close();
	/***** end of reading file and initializing *****/

	/***** breadth-first search *****/
	queue<Node> q;
	Node u(x_start, y_start);
	q.push(u);
	map[u.x][u.y] = 0;

	while(!q.empty()){
		Node t = q.front();
		q.pop();
		int d = map[t.x][t.y] + 1;

		if((t.x == x_end && abs(t.y - y_end) == 1) || (abs(t.x - x_end) == 1 && t.y == y_end)){
			map[x_end][y_end] = d;
			break;
		}

		if(t.x != 0 && map[t.x - 1][t.y] == -1){
			Node v(t.x - 1, t.y);
			q.push(v);
			map[t.x - 1][t.y] = d;
		}
		if(t.x != x_size - 1 && map[t.x + 1][t.y] == -1){
			Node v(t.x + 1, t.y);
			q.push(v);
			map[t.x + 1][t.y] = d;
		}
		if(t.y != 0 && map[t.x][t.y - 1] == -1){
			Node v(t.x, t.y - 1);
			q.push(v);
			map[t.x][t.y - 1] = d;
		}
		if(t.y != y_size - 1 && map[t.x][t.y + 1] == -1){
			Node v(t.x, t.y + 1);
			q.push(v);
			map[t.x][t.y + 1] = d;
		}
	}
	/***** end of breadth-first search *****/

	/***** output file *****/
	if(map[x_end][y_end] == -1){	// no path from start point to end point
		cout << "There is no path from (" << x_start << ", " << y_start
			<< ") to (" << x_end << ", " << y_end << ")\n";

		char *outfile = new char[strlen(argv[1] + 6)];
		sprintf(outfile, "path_%s", argv[1]);
		fstream fout;
		fout.open(outfile, ios::out);

		strcpy(buf, "No Path");
		fout.write(buf, strlen(buf));

		fout.close();
	}
	else{
		cout << "Shortest distance from (" << x_start << ", " << y_start
			<< ") to (" << x_end << ", " << y_end << ") : " << map[x_end][y_end] << endl;

		/***** backtrack the path *****/
		stack<Node> path;
		int dist = map[x_end][y_end] - 1;
		u.x = x_end;
		u.y = y_end;
		path.push(u);

		while(dist >= 0){
			int xtmp = u.x, ytmp = u.y;
			if(xtmp != 0 && map[xtmp - 1][ytmp] == dist){
				u.x--;
				path.push(u);
			}
			else if(xtmp != x_size - 1 && map[xtmp + 1][ytmp] == dist){
				u.x++;
				path.push(u);
			}
			else if(ytmp != 0 && map[xtmp][ytmp - 1] == dist){
				u.y--;
				path.push(u);
			}
			else{
				u.y++;
				path.push(u);
			}

			dist--;
		}

		/***** output shortest distance and path to file *****/
		char *outfile = new char[strlen(argv[1] + 6)];
		sprintf(outfile, "path_%s", argv[1]);
		fstream fout;
		fout.open(outfile, ios::out);

		sprintf(buf, "%d\n", map[x_end][y_end]);
		fout.write(buf, strlen(buf));

		for(int i = path.size(); i > 0; i--){
			sprintf(buf, "%d %d\n", path.top().x, path.top().y);
			fout.write(buf, strlen(buf));

			path.pop();
		}

		fout.close();

		delete [] outfile;
	}
	/***** end of output file *****/

	/***** deallocate memory *****/
	for(int i = 0; i < x_size; i++)
		delete [] map[i];
	delete [] map;

	return 0;
}
