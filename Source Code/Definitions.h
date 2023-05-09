//Leo Croft

// Definitions.h
// =============
//
// Type definitions to support path finding 
//

#pragma once

#include <vector>
#include <deque>
#include <memory>
#include <algorithm> 

using namespace std;

enum ECompass
{
	North,
	East,
	South,
	West
};

// Terrain costs for map squares

enum ENodeType
{
	wall = 0,
	clear = 1,
	wood = 2,
	water = 3,
	start,
	end
}; //The type of node also determines the cost of crossing it.

// Maps of any size are implemented as 2D vectors
using TerrainMap = vector<vector<ENodeType>>;

// Represents a node in the search tree.
struct SNode
{
	int x;            // x coordinate
	int y;            // y coordinate
	int mScore = 0;        // used in more complex algorithms
	SNode* mpParent = 0; // note use of raw pointer here
  
	bool NodesMatch(const SNode* comparison); //Test if the two nodes have matching coordinates.
  	
	//Difference in X + Difference in Y.
	int CalculateManhattanDistance(int newX, int newY); //Calculate the minimum number of spaces that would be required to reach the goal from the coordinates.
};

const int NODE_NOT_FOUND = -1; //Used to identify if the CheckListForBetter function didn't find the node on the list
const int BETTER_FOUND = -2; //Used to identify if the CheckListForBetter function found a node on the list which was better than the new node.

enum EStepPathResults
{
	NO_PATH = 0, //Openlist was empty at the end of the step, meaning that all reachable nodes have been checked.
	PATH_FOUND = 1, //The goal was reached this step.
	STEP_SUCCESS = 2, //The goal was not found, but openlist still has nodes on it.
};

// Lists of nodes (e.g Open, Closed and Paths) are implemented as double-ended queues
using NodeList = deque<unique_ptr<SNode>>;
