
#include "STcpClient.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <utility>
using namespace std;
#define ITERS 5000
#define TO 5800
int total_n = 0, component = 0;
const double exploration_constant = 1.414;
/*
	選擇起始位置
	選擇範圍僅限場地邊緣(至少一個方向為牆)

	return: init_pos
	init_pos=<x,y>,代表你要選擇的起始位置

*/
std::vector<int> InitPos(int mapStat[12][12])
{
	std::vector<int> init_pos;
	init_pos.resize(2);
	int ava[12][12];
	// 1, check if the space is available 只能放邊緣
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			if (mapStat[x][y] == -1) // 障礙
			{
				ava[x][y] = 0;
				continue;
			}
			if (x == 0 || x == 11 || y == 0 || y == 11)
			{
				ava[x][y] = 1;
			}
			else
			{
				if (mapStat[x][y - 1] == -1 || mapStat[x - 1][y] == -1 || mapStat[x + 1][y] == -1 || mapStat[x][y + 1] == -1)
				{
					ava[x][y] = 1;
				}
				else
				{
					ava[x][y] = 0;
				}
			}
		}
	}
	
	vector<int> step_size[12][12]; // size = 8, for 8 directions
	//  calculate the step size for each direction
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			if (mapStat[x][y] == 0 && ava[x][y] == 1)
			{
				int x_step = x;
				int y_step = y;
				for (int dir = 1; dir <= 9; dir++)
				{
					x_step = x;
					y_step = y;
					if (dir == 5)
						continue;
					int step_length = 0;
					while (true)
					{
						if (dir == 1)
						{
							if (x_step > 0 && y_step > 0 && mapStat[x_step - 1][y_step - 1] == 0)
							{
								x_step--;
								y_step--;
								step_length++;
							}
							else
								break;
						}
						else if (dir == 2)
						{
							if (y_step > 0 && mapStat[x_step][y_step - 1] == 0)
							{
								y_step--;
								step_length++;
							}
							else
								break;
						}
						else if (dir == 3)
						{
							if (x_step < 11 && y_step > 0 && mapStat[x_step + 1][y_step - 1] == 0)
							{
								x_step++;
								y_step--;
								step_length++;
							}
							else
								break;
						}
						else if (dir == 4)
						{
							if (x_step > 0 && mapStat[x_step - 1][y_step] == 0)
							{
								x_step--;
								step_length++;
							}
							else
								break;
						}
						else if (dir == 6)
						{
							if (x_step < 11 && mapStat[x_step + 1][y_step] == 0)
							{
								x_step++;
								step_length++;
							}
							else
								break;
						}
						else if (dir == 7)
						{
							if (x_step > 0 && y_step < 11 && mapStat[x_step - 1][y_step + 1] == 0)
							{
								x_step--;
								y_step++;
								step_length++;
							}
							else
								break;
						}
						else if (dir == 8)
						{
							if (y_step < 11 && mapStat[x_step][y_step + 1] == 0)
							{
								y_step++;
								step_length++;
							}
							else
								break;
						}
						else if (dir == 9)
						{
							if (x_step < 11 && y_step < 11 && mapStat[x_step + 1][y_step + 1] == 0)
							{
								x_step++;
								y_step++;
								step_length++;
							}
							else
								break;
						}
					}
					step_size[x][y].push_back(step_length); // 就算是0 也會push_back 但總共只有8個方向 ，沒有方向5的
				}
			}
		}
	}

	// what is important? step length,
	int max_step_length = 0;
	int max_dir = 0;
	bool over_16 = false;
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			int ava_dir = 0;
			int sum_step_size = 0;
			if (mapStat[x][y] != 0 || ava[x][y] == 0)
			{
				continue; // Skip non-empty or inaccessible fields
			}
			else
			{
				for (int i = 0; i < 8; i++) // step_size存了1-9但沒有方向5的，所以只要8次
				{
					if (step_size[x][y][i] > 0)
					{
						ava_dir++;
						sum_step_size += step_size[x][y][i];
					}
				}
				if (ava_dir < 4)
					continue; // Skip if less than 4 available directions
				if (over_16 == true)
				{
					if (max_dir < ava_dir && sum_step_size >= 16)
					{
						// cout << "max step: " << max_step_length << endl;
						// cout << "max dir: " << max_dir << endl;
						max_step_length = sum_step_size;
						max_dir = ava_dir;
						init_pos[0] = x;
						init_pos[1] = y;
					}
				}
				else
				{
					if (sum_step_size > max_step_length && sum_step_size >= 10)
					{
						// cout << "max step: " << max_step_length << endl;
						// cout << "max dir: " << max_dir << endl;
						max_step_length = sum_step_size;
						max_dir = ava_dir;
						init_pos[0] = x;
						init_pos[1] = y;
						if (sum_step_size > 16)
						{
							over_16 = true;
						}
					}
				}
			}
		}
	}
	return init_pos;
}

/*
	產出指令

	input:
	playerID: 你在此局遊戲中的角色(1~4)
	mapStat : 棋盤狀態, 為 12*12矩陣,
					0=可移動區域, -1=障礙, 1~4為玩家1~4佔領區域
	sheepStat : 羊群分布狀態, 範圍在0~16, 為 12*12矩陣

	return Step
	Step : <x,y,m,dir>
			x, y 表示要進行動作的座標
			m = 要切割成第二群的羊群數量
			dir = 移動方向(1~9),對應方向如下圖所示
			1 2 3
			4 X 6
			7 8 9
*/
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

int selection(vector<struct node> &tree)
{
	bool leaf_node_found = false;
	int leaf_node_id = 0;
	while (!leaf_node_found)
	{
		int node_id = leaf_node_id;
		int child_n = tree[node_id].child.size();
		if (child_n == 0)
		{
			leaf_node_id = node_id;
			leaf_node_found = true;
		}
		else
		{
			float max_uct = -100.0;
			for (int i = 0; i < tree[node_id].child.size(); i++)
			{
				int child_id = tree[node_id].child[i];
				double w = tree[child_id].w;
				double n = tree[child_id].n;
				if (n == 0)
				{
					n = 1e-4;
				}
				double exploitation_value = w / n;
				double exploration_value = sqrt(log(total_n) / n);
				double uct_value = exploitation_value + exploration_constant * exploration_value;
				if (uct_value > max_uct)
				{
					max_uct = uct_value;
					leaf_node_id = child_id;
				}
			}
		}
	}
	return leaf_node_id;
}

int expansion(int sel_node, vector<struct node>& tree, int playerID)
{
	// push all the avaliable steps to tree[sel_node].step
	int canMove = false;
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			if (tree[sel_node].board[x][y] == playerID && tree[sel_node].sheep[x][y] > 1)
			{	// probably canMove
				int ava_dir = 0;
            	vector<int> possible_dirs;
				
				if (x > 0 && y > 0 && tree[sel_node].board[x - 1][y - 1] == 0)
				{
                	ava_dir++;
					possible_dirs.push_back(1);
				} // dir 1
				if (y > 0 && tree[sel_node].board[x][y - 1] == 0)
				{
					ava_dir++;
					possible_dirs.push_back(2);
					
				} // dir 2
				if (x < 11 && y > 0 && tree[sel_node].board[x + 1][y - 1] == 0)
				{
					ava_dir++;
					possible_dirs.push_back(3);
				} // dir 3
				if (x > 0 && tree[sel_node].board[x - 1][y] == 0)
				{
					ava_dir++;
					possible_dirs.push_back(4);
				} // dir 4
				if (x < 11 && tree[sel_node].board[x + 1][y] == 0)
				{
					ava_dir++;
					possible_dirs.push_back(6);
				} // dir 6
				if (x > 0 && y < 11 && tree[sel_node].board[x - 1][y + 1] == 0)
				{
					ava_dir++;
					possible_dirs.push_back(7);
				} // dir 7
				if (y < 11 && tree[sel_node].board[x][y + 1] == 0)
				{
					ava_dir++;
					possible_dirs.push_back(8);
				} // dir 8
				if (x < 11 && y < 11 && tree[sel_node].board[x + 1][y + 1] == 0)
				{
					ava_dir++;
					possible_dirs.push_back(9);
				} // dir 9

				// 太多了 改成for
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
						for (int split = 1; split < tree[sel_node].sheep[x][y]; split++)
						{ 
							temp_step[2] = split;
							tree[sel_node].step.push_back(temp_step);
						}
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
			struct node newNode;
			int x = tree[sel_node].step[k][0];
			int y = tree[sel_node].step[k][1];
			int m = tree[sel_node].step[k][2];
			int dir = tree[sel_node].step[k][3];
			int x_step = x;
			int y_step = y;
			// x_step, y_step: the space after move toward direction dir
			while (1)
			{
				if (dir == 1)
				{
					if (x_step > 0 && y_step > 0 && (tree[sel_node].board[x_step - 1][y_step - 1] == 0))
					{
						x_step--;
						y_step--;
					}
					else
						break;
				}
				else if (dir == 2)
				{
					if (y_step > 0 && tree[sel_node].board[x_step][y_step - 1] == 0)
					{
						y_step--;
					}
					else
						break;
				}
				else if (dir == 3)
				{
					if (x_step < 11 && y_step > 0 && tree[sel_node].board[x_step + 1][y_step - 1] == 0)
					{
						x_step++;
						y_step--;						
					}
					else
						break;
				}
				else if (dir == 4)
				{
					if (x_step > 0 && tree[sel_node].board[x_step - 1][y_step] == 0)
					{
						x_step--;
					}
					else
						break;
				}
				else if (dir == 6)
				{
					if (x_step < 11 && tree[sel_node].board[x_step + 1][y_step] == 0)
					{
						x_step++;
					}
					else
						break;
				}
				else if (dir == 7)
				{
					if (x_step > 0 && y_step < 11 && tree[sel_node].board[x_step - 1][y_step + 1] == 0)
					{
						x_step--;
						y_step++;
					}
					else
						break;
				}
				else if (dir == 8)
				{
					if (y_step < 11 && tree[sel_node].board[x_step][y_step + 1] == 0)
					{
						y_step++;
					}
					else
						break;
				}
				else if (dir == 9)
				{
					if (x_step < 11 && y_step < 11 && tree[sel_node].board[x_step + 1][y_step + 1] == 0)
					{	
						x_step++;
						y_step++;
					}
					else
						break;
				}				
			}

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
		srand(time(NULL));
		int rand_child = rand() % tree[sel_node].child.size(); // random child
		return tree[sel_node].child[rand_child];
	}
	else
	{
		return sel_node;
	}
}

void dfs(int (&gameBoard)[12][12], int x, int y, int playerID)
{
	gameBoard[x][y] = 0;
	if (y < 11 && gameBoard[x][y + 1] == playerID)
	{
		component++;
		dfs(gameBoard, x, y + 1, playerID);
	}
	if (x < 11 && gameBoard[x + 1][y] == playerID)
	{
		component++;
		dfs(gameBoard, x + 1, y, playerID);
	}
}

int simulation(vector<struct node> &tree, int node_id, int player_ID)
{ // player's order
	total_n++;
	int currentPlayer = tree[node_id].player;
	int gameBoard[12][12];
	int sheep[12][12];
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			gameBoard[x][y] = tree[node_id].board[x][y];
			sheep[x][y] = tree[node_id].sheep[x][y];
		}
	}
	int jump_count_down = 4;
	bool canMove[4] = {true, true, true, true}; // player 1 2 3 4 (idx 0 1 2 3)
	bool terminate = false;
	while (!terminate)
	{
		bool move = false;
		if (canMove[currentPlayer - 1] == false)
		{
			if (currentPlayer == player_ID && (canMove[0] + canMove[1] + canMove[2] + canMove[3]) == 3)
			{
				jump_count_down--;
				if (jump_count_down == 0)
				{
					return 4; // rank 4
				}
			}
			currentPlayer = (currentPlayer % 4) + 1; // next player
			continue;
		}
		vector<vector<int>> step;
		// current player puah all possible dir/sheep into vec step
		for (int y = 0; y < 12; y++)
		{
			for (int x = 0; x < 12; x++)
			{
				if (gameBoard[x][y] == currentPlayer && sheep[x][y] > 1)
				{ // probably canMove
					int ava_dir = 0;
					if (y > 0 && x > 0 && gameBoard[x - 1][y - 1] == 0)
					{
						vector<int> temp_step = {x, y, 0, 1};
						for (int split = 1; split < sheep[x][y]; split++)
						{ // 1~k-1
							temp_step[2] = split;
							step.push_back(temp_step);
						}
						ava_dir++;
					} // dir 1

					if (y > 0 && gameBoard[x][y - 1] == 0)
					{
						vector<int> temp_step = {x, y, 0, 2};
						for (int split = 1; split < sheep[x][y]; split++)
						{ // 1~k-1
							temp_step[2] = split;
							step.push_back(temp_step);
						}
						ava_dir++;
					} // dir 2

					if (x < 11 && y > 0 && gameBoard[x + 1][y - 1] == 0)
					{
						vector<int> temp_step = {x, y, 0, 3};
						for (int split = 1; split < sheep[x][y]; split++)
						{ // 1~k-1
							temp_step[2] = split;
							step.push_back(temp_step);
						}
						ava_dir++;
					} // dir 3

					if (x > 0 && gameBoard[x - 1][y] == 0)
					{
						vector<int> temp_step = {x, y, 0, 4};
						for (int split = 1; split < sheep[x][y]; split++)
						{ // 1~k-1
							temp_step[2] = split;
							step.push_back(temp_step);
						}
						ava_dir++;
					} // dir 4

					if (x < 11 && gameBoard[x + 1][y] == 0)
					{
						vector<int> temp_step = {x, y, 0, 6};
						for (int split = 1; split < sheep[x][y]; split++)
						{ // 1~k-1
							temp_step[2] = split;
							step.push_back(temp_step);
						}
						ava_dir++;
					} // dir 6

					if (x > 0 && y < 11 && gameBoard[x - 1][y + 1] == 0)
					{
						vector<int> temp_step = {x, y, 0, 7};
						for (int split = 1; split < sheep[x][y]; split++)
						{ // 1~k-1
							temp_step[2] = split;
							step.push_back(temp_step);
						}
						ava_dir++;
					} // dir 7

					if (y < 11 && gameBoard[x][y + 1] == 0)
					{
						vector<int> temp_step = {x, y, 0, 8};
						for (int split = 1; split < sheep[x][y]; split++)
						{ // 1~k-1
							temp_step[2] = split;
							step.push_back(temp_step);
						}
						ava_dir++;
					} // dir 8

					if (x < 11 && y < 11 && gameBoard[x + 1][y + 1] == 0)
					{
						vector<int> temp_step = {x, y, 0, 9};
						for (int split = 1; split < sheep[x][y]; split++)
						{ // 1~k-1
							temp_step[2] = split;
							step.push_back(temp_step);
						}
						ava_dir++;
					} // dir 9

					if (ava_dir > 0)
						move = true;
				}
			}
		}
		if (move == false)
		{
			canMove[currentPlayer - 1] = false;
			currentPlayer = (currentPlayer % 4) + 1;
		}
		else
		{
			srand(time(NULL));
			int rand_action = rand() % step.size();
			// randomly select step for currentPlayer
			int x = step[rand_action][0];
			int y = step[rand_action][1];
			int m = step[rand_action][2];
			int dir = step[rand_action][3];
			int x_step = x;
			int y_step = y;
			while (1)
			{ // terminate condition: border, occupied
				if (dir == 1)
				{
					if (x_step > 0 && y_step > 0 && (gameBoard[x_step - 1][y_step - 1] == 0))
					{
						x_step--;
						y_step--;
					}
					else
						break;
				}
				else if (dir == 2)
				{
					if (y_step > 0 && gameBoard[x_step][y_step - 1] == 0)
					{
						y_step--;
					}
					else
						break;
				}
				else if (dir == 3)
				{
					if (x_step < 11 && y_step > 0 && gameBoard[x_step + 1][y_step - 1] == 0)
					{
						x_step++;
						y_step--;
					}
					else
						break;
				}
				else if (dir == 4)
				{
					if (x_step > 0 && gameBoard[x_step - 1][y_step] == 0)
					{
						x_step--;
					}
					else
						break;
				}
				else if (dir == 6)
				{
					if (x_step < 11 && gameBoard[x_step + 1][y_step] == 0)
					{
						x_step++;
					}
					else
						break;
				}
				else if (dir == 7)
				{
					if (x_step > 0 && y_step < 11 && gameBoard[x_step - 1][y_step + 1] == 0)
					{
						x_step--;
						y_step++;
					}
					else
						break;
				}
				else if (dir == 8)
				{
					if (y_step < 11 && gameBoard[x_step][y_step + 1] == 0)
					{
						y_step++;
					}
					else
						break;
				}
				else if (dir == 9)
				{
					if (x_step < 11 && y_step < 11 && gameBoard[x_step + 1][y_step + 1] == 0)
					{
						x_step++;
						y_step++;
					}
					else
						break;
				}
			}
			// x, y to x_step, y_step with m split
			gameBoard[x_step][y_step] = currentPlayer;
			sheep[x][y] = sheep[x][y] - m;
			sheep[x_step][y_step] = m;
			currentPlayer = (currentPlayer % 4) + 1;
		}
		if (canMove[0] == false && canMove[1] == false && canMove[2] == false && canMove[3] == false)
		{
			terminate = true;
		}
	}
	// end game, calculate points
	vector<vector<int>> connect(4);
	// find connect component area size
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			if (gameBoard[x][y] == 0 || gameBoard[x][y] == -1)
			{
				continue;
			}
			else
			{
				int playerID = gameBoard[x][y];
				// gameBoard[x][y] = 0;
				dfs(gameBoard, x, y, playerID);
				int componentSize = component;
				connect[playerID - 1].push_back(componentSize);
				component = 0;
			}
		}
	}
	int rank = 1;
	double our_team = 0, other_team = 0;
	for (int i = 0; i < connect.size(); ++i)
	{
		double score = 0.0;
		for (int j = 0; j < connect[i].size(); ++j)
		{
			score += pow(connect[i][j], 1.25);
		}
		if(i == player_ID || i == (player_ID % 4) + 2)
			our_team += score;
		else
			other_team += score;		
	}
	if (our_team > other_team)
			rank++;

	return rank;
}

void backprop(vector<struct node> &tree, int order, int playerID, int leaf_node_id)
{ // update the tree
	double reward;
	if (order == 1)
		reward = 0.7;
	else if (order == 2)
		reward = 0.3;
	// else if (order == 3)
	// 	reward = 0.2;
	// else
	// 	reward = 0.1;

	bool finish_bp = false;
	int node_id = leaf_node_id;
	while (!finish_bp)
	{
		tree[node_id].n += 1;
		tree[node_id].w += reward;
		tree[node_id].q = tree[node_id].w / tree[node_id].n;
		int parent_id = tree[node_id].parent;
		if (parent_id == 0)
			finish_bp = true;
		else
			node_id = parent_id;
	}
	return;
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
		// if(sel_node == expand_child){cout<<"same, order: "<<order<<endl;}
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
