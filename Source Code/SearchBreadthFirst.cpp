//Leo Croft

// SearchBreadthFirst.cpp
// ======^^^^^^====
//
// Implementation of Search class for Breadth First algorithm
//

#include "SearchBreadthFirst.h" // Declaration of this class

// This function takes ownership of the start and goal pointers that are passed in from the calling code.
// Ownership is not returned at the end, so the start and goal nodes are consumed.
// The Path is returned through the reference parameter.
bool CSearchBreadthFirst::FindPath(TerrainMap& terrain, unique_ptr<SNode> start, unique_ptr<SNode> goal, NodeList& path)
{
	NodeList openList;
	NodeList closedList;

	//Create the first node in the path- the origin. 
	openList.push_back(move(start));
	
	EStepPathResults result;
	do
	{
		result = StepPath(terrain, openList, closedList, goal, path);
		if (result == EStepPathResults::PATH_FOUND)
		{
			return true;
		}
	} while (result != EStepPathResults::NO_PATH);

	return false;
}

// Performs a single step of the FindPath function.
// Performs the function of the loop in FindPath. Start should be the first node in openlist the first time StepPath is called.
// Takes the openlist and closedlist as additionally reference parameters; These are used to set textures and create models.
// Goal is passed as a reference parameter because it is used for comparison; It is not added onto the openlist until it is found by the search.
EStepPathResults CSearchBreadthFirst::StepPath(TerrainMap& terrain, NodeList& openList, NodeList& closedList, unique_ptr<SNode>& goal, NodeList& path)
{

	//Pop the first element from OpenList.
	unique_ptr<SNode> current = move(openList.front());
	openList.pop_front();

	//The end is found.
	//Follow the reverse path from the end node to build the path for the ball.
	if (current->NodesMatch(goal.get()))
	{
		BuildPath(path, closedList, move(current));
		return EStepPathResults::PATH_FOUND;
	}

	unique_ptr<SNode> tmp; // So new unique_ptrs don't need to be defined each time a new node is created

	//NORTH. Test if in open, closed or wall.
	if (current->y + 1 < terrain[0].size()) //Ensure that it is within the bounds of the map. EG, in a 10 by 10 map, if y + 1 = 10, not valid.
	{
		if (terrain[current->x][current->y + 1] != ENodeType::wall //Is not a wall
			&& !IsNodeOnList(openList, current->x, current->y + 1) //Is not on the open list
			&& !IsNodeOnList(closedList, current->x, current->y + 1)) //Is not on the closed list
		{
			//Set up the node data, then move onto the open list.
			tmp.reset(new SNode);
			tmp->x = current->x;
			tmp->y = current->y + 1;
			tmp->mpParent = current.get();

			openList.push_back(move(tmp));
		}
	}
	//East
	if (current->x + 1 < terrain.size())
	{
		if (terrain[current->x + 1][current->y] != ENodeType::wall //Is not a wall
			&& !IsNodeOnList(openList, current->x + 1, current->y) //Is not on the open list
			&& !IsNodeOnList(closedList, current->x + 1, current->y)) //Is not on the closed list
		{
			//Set up the node data, then move onto the open list.
			tmp.reset(new SNode);
			tmp->x = current->x + 1;
			tmp->y = current->y;
			tmp->mpParent = current.get();

			openList.push_back(move(tmp));
		}
	}
	//SOUTH. Test if in open, closed or wall.
	if (current->y - 1 >= 0) //Ensure that it is within the bounds of the map. EG, in a 10 by 10 map, if y + 1 = 10, not valid.
	{
		if (terrain[current->x][current->y - 1] != ENodeType::wall //Is not a wall
			&& !IsNodeOnList(openList, current->x, current->y - 1) //Is not on the open list
			&& !IsNodeOnList(closedList, current->x, current->y - 1)) //Is not on the closed list
		{
			//Set up the node data, then move onto the open list.
			tmp.reset(new SNode);
			tmp->x = current->x;
			tmp->y = current->y - 1;
			tmp->mpParent = current.get();

			openList.push_back(move(tmp));
		}
	}
	//West
	if (current->x - 1 >= 0)
	{
		if (terrain[current->x - 1][current->y] != ENodeType::wall //Is not a wall
			&& !IsNodeOnList(openList, current->x - 1, current->y) //Is not on the open list
			&& !IsNodeOnList(closedList, current->x - 1, current->y)) //Is not on the closed list
		{
			//Set up the node data, then move onto the open list.
			tmp.reset(new SNode);
			tmp->x = current->x - 1;
			tmp->y = current->y;
			tmp->mpParent = current.get();

			openList.push_back(move(tmp));
		}
	}

	closedList.push_back(move(current));

	//If the open list is empty, no path exists. If 
	if (openList.empty())
	{
		return EStepPathResults::NO_PATH;
	}
	else
	{
		return EStepPathResults::STEP_SUCCESS;
	}
}