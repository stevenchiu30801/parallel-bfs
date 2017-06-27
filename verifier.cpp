// BFS path verifier
// Only 2017/06/22
//

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <queue>          // std::priority_queue
using namespace std;

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define diff(a,b) ((a) > (b) ? ((a)-(b)) : ((b)-(a)))

#define RIGHT   0
#define UP      1
#define LEFT    2
#define DOWN    3
#define DIR     4

/******************* Structure *******************/
class Node{
	private:
		int x, y;
		int step;
		int estimate;
	public:
		Node(){
			x=0;    y=0;    step=0;    estimate=0;
		}
		Node(int X, int Y, int STEP, int ESTIMATE){
			x=X;    y=Y;    step=STEP;    estimate=ESTIMATE;
		}
		int getX() const    { return x; };
		int getY() const    { return y; };
		int getStep() const { return step; };
		int getEST() const  { return estimate; };
		bool operator<(const Node&) const;

};

bool Node::operator<(const Node& right) const
{
  return estimate > right.estimate;
}

/******************* Global Variables *******************/
int** map;
int mapSizeX, mapSizeY;
int sourceX, sourceY;
int targetX, targetY;

vector<int> pathX, pathY;
int path_distance;

/******************* Functions *******************/
void parse_test_data( char* filename );
void parse_path_result( char* filename );
void Check_path( char* filename );
void Destruct();
int BFS();

/******************* Functions *******************/
int main( int argc, char** argv )
{
	if( argc != 3 )
	{
		cout << "Error arg number. CMD: " << argv[0] << " [test_data filename] [path_result filename]" << endl;
		return false;
	}

	cout << "Reading test_data file..." << endl;
	parse_test_data( argv[1] );

	cout << "Reading path_result file..." << endl;
	parse_path_result( argv[2] );

	cout << "Start to check path_result..." << endl;
	Check_path( argv[1] );

	Destruct();
	return 0;
}

void parse_test_data( char* filename )
{
	ifstream fin( filename );

	fin >> mapSizeX >> mapSizeY;
	fin >> sourceX >> sourceY;
	fin >> targetX >> targetY;

	// create map
	map = new int* [mapSizeX+1];
	for(int i=0; i<=mapSizeX; i++)
	{
		map[i] = new int [mapSizeY+1];
		for(int j=0; j<=mapSizeY; j++)
			map[i][j] = -2;
	}

	// read blocks
	int Ax, Ay, Bx, By;
	while( fin >> Ax >> Ay >> Bx >> By )
	{
		int LDx = min(Ax,Bx);
		int LDy = min(Ay,By);
		int RTx = max(Ax,Bx);
		int RTy = max(Ay,By);
		for(int x=LDx; x<=RTx; x++)
			for(int y=LDy; y<=RTy; y++)
				map[x][y] = -1;
	}

	fin.close();
	return;
}

void parse_path_result( char* filename )
{
	ifstream fin( filename );

	fin >> path_distance;

	int path_x, path_y;
	while(fin >> path_x >> path_y)
	{
		pathX.push_back(path_x);
		pathY.push_back(path_y);
	}

	fin.close();
	return;
}

void Check_path( char* filename )
{
	bool all_pass = true;
	cout << "==========================================================" << endl;
	cout << "Check shortest distance...                   ";
	int answer = -1;
	if(!strcmp(filename,"map1.txt")) answer = 8;
	if(!strcmp(filename,"map4.txt")) answer = 20002;
	if(!strcmp(filename,"map5.txt")) answer = 39917;
	if(answer==-1) { answer = BFS();}

	if(path_distance==answer) 	{ cout << "pass" << endl;}
	else 						{ cout << "fail" << endl; all_pass = false;}

	cout << "Check vertex number...                       ";
	bool same_size = (pathX.size()==pathY.size()) && (pathX.size()-1==path_distance);
	if(same_size) 	{ cout << "pass" << endl;}
	else 			{ cout << "fail" << endl; all_pass = false;}

	cout << "Check connection...                          ";
	bool is_connected = true;
	if(pathX.front()!=sourceX || pathY.front()!=sourceY) is_connected = false;
	if(pathX.back()!=targetX || pathY.back()!=targetY) is_connected = false;

	for(int i_path=0; i_path<pathX.size()-1; i_path++)
	{
		bool ABconnective = false;
		int Ax=pathX[i_path];    int Ay=pathY[i_path];
		int Bx=pathX[i_path+1];    int By=pathY[i_path+1];
		if(Ax==Bx && diff(Ay,By)==1) ABconnective = true;
		if(Ay==By && diff(Ax,Bx)==1) ABconnective = true;
		is_connected = is_connected & ABconnective;
	}
	if(is_connected) 	{ cout << "pass" << endl;}
	else 				{ cout << "fail" << endl; all_pass = false;}

	cout << "Check overlap with blocks...                 ";
	bool overlap = false;
	for(int i_path=0; i_path<pathX.size(); i_path++)
	{
		int x=pathX[i_path];    int y=pathY[i_path];
		if(x>mapSizeX || y>mapSizeY) {overlap=true; continue;}
		if(map[x][y]==-1) {
            cout << "overlap " << x << " " << y << endl;
            overlap = true;
		}
	}
	if(!overlap) 	{ cout << "pass" << endl;}
	else 			{ cout << "fail" << endl; all_pass = false;}

	cout << "----------------------------------------------------------" << endl;
	if(all_pass)
	{
		cout << "The path is legal. BFS success!" << endl;
		cout << "==========================================================" << endl;
		cout << "*************      !!! PASS !!!        *******************" << endl;
	}
	else
		cout << "The path is illegal. Please check with the message above." << endl;
	cout << "==========================================================" << endl;
	return;
}


int BFS()
{
	priority_queue<Node> pQueue;
	Node Source( sourceX, sourceY, 0, diff(sourceX,targetX)+diff(sourceY,targetY) );
	pQueue.push(Source);
	while(!pQueue.empty())
	{
		int nowX = pQueue.top().getX();
		int nowY = pQueue.top().getY();
		int nowSTEP = pQueue.top().getStep();
		pQueue.pop();
		for(int i=0; i<DIR; i++)
		{
			int nextX = nowX;
			int nextY = nowY;
			int nextSTEP = nowSTEP;

			if(i==RIGHT)      nextX += 1;
			else if(i==LEFT)  nextX -= 1;
			else if(i==UP)    nextY += 1;
			else if(i==DOWN)  nextY -= 1;
			else continue;
			nextSTEP++;

			if(nextX==targetX && nextY==targetY) return nextSTEP;
			if(nextX<0 || nextX>mapSizeX || nextY<0 || nextY>mapSizeY) continue;
			if(map[nextX][nextY]==-1) continue;
			if(map[nextX][nextY]!=-2 && map[nextX][nextY]<=nextSTEP) continue;

			map[nextX][nextY] = nextSTEP;
			Node nextNode( nextX, nextY, nextSTEP, nextSTEP+diff(nextX,targetX)+diff(nextY,targetY) );
			pQueue.push(nextNode);
		}
	}
	return -1;
}

void Destruct()
{
	for(int i=0; i<=mapSizeX; i++)
		delete [] map[i];
	delete map;

	return;
}
