
#include "STcpClient.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <utility>
using namespace std;
struct node
{
public:
	int player;
	int board[12][12];
	int sheep[12][12];
	vector<int> child;
	int parent;
	vector<vector<int>> step; // <x,y,m,dir>
	int n;
	double q;
	double w;
};

int expansion(int sel_node, vector<struct node>& tree, int playerID)
{
	// push all the avaliable steps to tree[sel_node].step
	int canMove = false;
	//記每格的可能方向數
	vector<vector<int>> ava_dirs(12, vector<int>(12, 0));
	int delta_x[] = { -1, 0, 1, -1, 0, +1, -1, 0, +1 };
	int delta_y[] = { -1, -1, -1, 0, 0, 0, +1, +1, +1 };
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			if (tree[sel_node].board[x][y] == playerID && tree[sel_node].sheep[x][y] > 1)
			{	// probably canMove
				int ava_dir = 0;
            	vector<int> possible_dirs;
				
				 for (int dir = 0; dir < 9; dir++)
				{
					int temp_x = x + delta_x[dir];
					int temp_y = y + delta_y[dir];
					if (temp_x >= 0 && temp_x < 12 && temp_y >= 0 && temp_y < 12 && tree[sel_node].board[temp_x][temp_y] == 0)
					{
						ava_dir++;
						possible_dirs.push_back(dir + 1);
					}
				}

				// add
				ava_dirs[x][y] = ava_dir;				
				for (int dir : possible_dirs)
				{
					vector<int> temp_step = {x, y, 0, dir};
					if (ava_dir == 1)
					{
						temp_step[2] = tree[sel_node].sheep[x][y] - 1;
						tree[sel_node].step.push_back(temp_step);
					}	
					else 
					{												
						// temp_step[2] = 1;
						tree[sel_node].step.push_back(temp_step);
					}				
					
				}
				if (ava_dir > 0)
					canMove = true;
			}
		}
	}

	if (canMove)
	{
		// loop through avaliable steps and construct new node
		for (int k = 0; k < tree[sel_node].step.size(); k++)
		{			
			int x = tree[sel_node].step[k][0];
			int y = tree[sel_node].step[k][1];
			int m = tree[sel_node].step[k][2];
			int dir = tree[sel_node].step[k][3];
			int x_step = x;
			int y_step = y;									
			int step_length = 0;
			vector<int> split_sheeps;
			// 找該方向步長、新座標
			while (x_step + delta_x[dir - 1] >= 0 && x_step + delta_x[dir - 1] < 12 &&
				y_step + delta_y[dir - 1] >= 0 && y_step + delta_y[dir - 1] < 12 &&
				tree[sel_node].board[x_step + delta_x[dir - 1]][y_step + delta_y[dir - 1]] == 0)
			{
				x_step += delta_x[dir - 1];
				y_step += delta_y[dir - 1];
				step_length++;
				
			}	

			if (ava_dirs[x][y] == 1)
			{
				// split_sheeps.push_back(tree[sel_node].sheep[x][y] - 1);
				split_sheeps.push_back(m);
			}	
			else 
			{											
				int empty = 0;
				if (step_length == 1) 
				{
					empty = ava_dirs[x_step][y_step] ;
					if (empty > tree[sel_node].sheep[x][y] - 2)
						empty = tree[sel_node].sheep[x][y] - 2;

					for (int split = 1; split < empty + 2; split++)
					{
						split_sheeps.push_back(split);
					}
					
				}
				else {
					for (int split = 1; split < tree[sel_node].sheep[x][y]; split++)
					{
						split_sheeps.push_back(split);
					}
				}

			}	
				
			for(int sp = 0; sp<split_sheeps.size(); sp++)
			{	m = split_sheeps[sp];
				struct node newNode;

				for (int i = 0; i < 12; i++)
				{
					for (int j = 0; j < 12; j++)
					{
						if (i == x && j == y)
						{
							newNode.board[i][j] = tree[sel_node].board[i][j]; // alreade have sheep flock before movement
							newNode.sheep[i][j] = tree[sel_node].sheep[i][j] - m;
						}
						else if (i == x_step && j == y_step)
						{
							newNode.board[i][j] = playerID; // movement
							newNode.sheep[i][j] = m;
						}
						else
						{
							newNode.board[i][j] = tree[sel_node].board[i][j];
							newNode.sheep[i][j] = tree[sel_node].sheep[i][j];
						}
					}
				}
				newNode.player = (tree[sel_node].player) % 4 + 1; // next player
				newNode.n = 0;
				newNode.w = 0;
				newNode.q = 0;
				newNode.parent = sel_node;
				tree[sel_node].child.push_back(tree.size()); // the last index as tree[sel_node].child
				tree.push_back(newNode);
			}
		}		
		srand(time(NULL));
		int rand_child = rand() % tree[sel_node].child.size(); // random child
		return tree[sel_node].child[rand_child];
	}
	else
	{
		return sel_node;
	}
}



std::vector<int> GetStep(int playerID, int mapStat[12][12], int sheepStat[12][12])
{
	vector<struct node> tree;
	std::vector<int> step;
	step.resize(4);
	// init_tree
	total_n = 0;
	struct node rootNode;
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			rootNode.board[i][j] = mapStat[i][j];
			rootNode.sheep[i][j] = sheepStat[i][j];
		}
	}
	rootNode.player = (playerID + 3) % 5 + 1; // previous player
	tree.push_back(rootNode);

	// no child, ava step, n, w, q, training MCTS
	clock_t start, finish, check;
	start = clock();
	int loop;
	for (int i = 0; i < ITERS; i++)
	{
		int sel_node = selection(tree);
		// cout <<"sel_node :"<< sel_node << endl;
		int expand_child = expansion(sel_node, tree, playerID);
		// cout <<"expand :"<< expand_child << endl;
		int order = simulation(tree, expand_child, playerID);
		// cout <<"order :"<< order << endl;
		backprop(tree, order, playerID, expand_child);
		// cout <<"backprop !"<< endl;
		check = clock();
		loop = i;
		if (check - start > TO)
		{
			break;
		}
	}
	finish = clock();


	double maxq = -100.0;
	int best_action_id = 0;
	for (int i = 0; i < tree[0].child.size(); i++)
	{
		if (tree[tree[0].child[i]].q > maxq)
		{
			maxq = tree[tree[0].child[i]].q;
			best_action_id = i;
		}
	}

	return tree[0].step[best_action_id];
}

int main()
{
	int id_package;
	int playerID;
	int mapStat[12][12];
	int sheepStat[12][12];

	// player initial
	GetMap(id_package, playerID, mapStat);
	std::vector<int> init_pos = InitPos(mapStat);
	SendInitPos(id_package, init_pos);

	while (true)
	{
		if (GetBoard(id_package, mapStat, sheepStat))
		{
			break;
		}
		std::vector<int> step = GetStep(playerID, mapStat, sheepStat);		
		SendStep(id_package, step);
	}
}