//Leo Croft

// SearchBreadthFirst.cpp
// ======^^^^^^====
//
// Implementation of Search class for Breadth First algorithm
//

#include "SearchAStar.h" // Declaration of this class
#include <iostream>

// This function takes ownership of the start and goal pointers that are passed in from the calling code.
// Ownership is not returned at the end, so the start and goal nodes are consumed.
// The Path is returned through the reference parameter.
bool CSearchAStar::FindPath(TerrainMap& terrain, unique_ptr<SNode> start, unique_ptr<SNode> goal, NodeList& path)
{
	NodeList openList;
	NodeList closedList;

	path.push_back(unique_ptr<SNode>(new SNode)); //The score value on this node will be used to track the count.

	start->mScore = goal->CalculateManhattanDistance(start->x, start->y);

	//Create the first node in the path- the origin of the search.
	//Both the parent and the cost are 0.
	openList.push_back(move(start));

	//Until goal is found or OpenList is empty.
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

EStepPathResults CSearchAStar::StepPath(TerrainMap& terrain, NodeList& openList, NodeList& closedList, unique_ptr<SNode>& goal, NodeList& path)
{
	//Pop the first element from OpenList and make it the current node.
	unique_ptr<SNode> current = move(openList.front());
	openList.pop_front();

	//std::cout << "x = " << current->x << "   y = " << current->y << "   Score = " << current->mScore << endl;

	//The end is found.
	//Follow the reverse path from the end node to build the path for the ball.
	//Then exit the search using return.
	if (current->NodesMatch(goal.get()))
	{
		//The first node in closedList is used to track the number of searches. While building the path, the goal node is the only node
		//that isn't deep-copied. Set the score of the goal to the number of searches to ensure it is returned to the main program.
		current->mScore = path[0]->mScore;
		path.clear();
		BuildPath(path, closedList, move(current));
		return EStepPathResults::PATH_FOUND;
	}

	//Keep track of the results of list checking and score calculations. Declared here so it doesn't need to be declared multiple times per call.
	int newCost; //The cost of the current node.
	int onListResult; //The result of the "CheckListForBetter" function.


	//NORTH. Test if in open, closed or wall.
	if (current->y + 1 < terrain[0].size() ) //Ensure that it is within the bounds of the map. EG, in a 10 by 10 map, if y + 1 = 10, not valid.
	{
		//If the next node is not a wall, nor on the open or closed lists, add it to openlist.
		if (terrain[current->x][current->y + 1] != ENodeType::wall) //Is not a wall
		{
			//If the next node is not found on the openlist, check the closedlist. If it's not found on closedlist, generate and push onto openlist.
			//If the next node is found on the openlist and is better than the one on the openlist, edit the data on the openlist.
			//If the next node is found on the closedlist and is better than the one on the closedlist, move it to the openlist and edit the data.
			//New cost = base cost + terrain cost
			newCost = current->mScore - goal->CalculateManhattanDistance(current->x, current->y) + 
					  terrain[current->x][current->y + 1] + goal->CalculateManhattanDistance(current->x, current->y + 1);
			onListResult = CheckListForBetter(openList, current->x, current->y + 1, newCost);
			if (onListResult == NODE_NOT_FOUND) //Is not on the open list
			{
				onListResult = CheckListForBetter(closedList, current->x, current->y + 1, newCost);
				if (onListResult == NODE_NOT_FOUND) //Is not on the closed list
				{
					//Set up the node data, then move onto the open list.
					openList.push_back(move(unique_ptr<SNode>(new SNode)));
					openList.back()->x = current->x;
					openList.back()->y = current->y + 1;
					openList.back()->mpParent = current.get();
					//New score = new cost + heuristic
					openList.back()->mScore = newCost;
				}
				else if (onListResult != BETTER_FOUND) //A node was found but it was not better
				{
					closedList[onListResult]->mpParent = current.get(); //Set current as the node's new parent
					closedList[onListResult]->mScore = newCost; //Set the nodes score to the new score.
					openList.push_back(move(closedList[onListResult])); //Move the node from the closed list to the open list.
					closedList.erase(closedList.begin() + onListResult); //Erase the now empty spot.
				}
			}
			else if (onListResult != BETTER_FOUND) //A node was found but it was not better
			{
				openList[onListResult]->mpParent = current.get(); //Set current as the node's new parent
				openList[onListResult]->mScore = newCost; //Set the nodes score to the new score.
			}
		}
	}
	//East
	if (current->x + 1 < terrain.size()) //Ensure that it is within the bounds of the map. EG, in a 10 by 10 map, if y + 1 = 10, not valid.
	{
		//If the next node is not a wall, nor on the open or closed lists, add it to openlist.
		if (terrain[current->x + 1][current->y] != ENodeType::wall) //Is not a wall
		{
			//If the next node is not found on the openlist, check the closedlist. If it's not found on closedlist, generate and push onto openlist.
			//If the next node is found on the openlist and is better than the one on the openlist, edit the data on the openlist.
			//If the next node is found on the closedlist and is better than the one on the closedlist, move it to the openlist and edit the data.
			//New cost = base cost + terrain cost
			newCost = current->mScore - goal->CalculateManhattanDistance(current->x, current->y) + 
				      terrain[current->x + 1][current->y] + goal->CalculateManhattanDistance(current->x + 1, current->y);
			onListResult = CheckListForBetter(openList, current->x + 1, current->y, newCost);
			if (onListResult == NODE_NOT_FOUND) //Is not on the open list
			{
				onListResult = CheckListForBetter(closedList, current->x + 1, current->y, newCost);
				if (onListResult == NODE_NOT_FOUND) //Is not on the closed list
				{
					//Set up the node data, then move onto the open list.
					openList.push_back(move(unique_ptr<SNode>(new SNode)));
					openList.back()->x = current->x + 1;
					openList.back()->y = current->y;
					openList.back()->mpParent = current.get();
					//New score = new cost + heuristic
					openList.back()->mScore = newCost;
				}
				else if (onListResult != BETTER_FOUND) //A node was found but it was not better
				{
					closedList[onListResult]->mpParent = current.get(); //Set current as the node's new parent
					closedList[onListResult]->mScore = newCost; //Set the nodes score to the new score.
					openList.push_back(move(closedList[onListResult])); //Move the node from the closed list to the open list.
					closedList.erase(closedList.begin() + onListResult); //Erase the now empty spot.
				}
			}
			else if (onListResult != BETTER_FOUND) //A node was found but it was not better
			{
				openList[onListResult]->mpParent = current.get(); //Set current as the node's new parent
				openList[onListResult]->mScore = newCost; //Set the nodes score to the new score.
			}
		}
	}

	//SOUTH. Test if in open, closed or wall.
	if (current->y - 1 >= 0) //Ensure that it is within the bounds of the map. EG, in a 10 by 10 map, if y + 1 = 10, not valid.
	{
		//If the next node is not a wall, nor on the open or closed lists, add it to openlist.
		if (terrain[current->x][current->y - 1] != ENodeType::wall) //Is not a wall
		{
			//If the next node is not found on the openlist, check the closedlist. If it's not found on closedlist, generate and push onto openlist.
			//If the next node is found on the openlist and is better than the one on the openlist, edit the data on the openlist.
			//If the next node is found on the closedlist and is better than the one on the closedlist, move it to the openlist and edit the data.
			//New cost = base cost + terrain cost
			newCost = current->mScore - goal->CalculateManhattanDistance(current->x, current->y) + 
					  terrain[current->x][current->y - 1] + goal->CalculateManhattanDistance(current->x, current->y - 1);
			onListResult = CheckListForBetter(openList, current->x, current->y - 1, newCost);
			if (onListResult == NODE_NOT_FOUND) //Is not on the open list
			{
				onListResult = CheckListForBetter(closedList, current->x, current->y - 1, newCost);
				if (onListResult == NODE_NOT_FOUND) //Is not on the closed list
				{
					//Set up the node data, then move onto the open list.
					openList.push_back(move(unique_ptr<SNode>(new SNode)));
					openList.back()->x = current->x;
					openList.back()->y = current->y - 1;
					openList.back()->mpParent = current.get();
					//New score = new cost + heuristic
					openList.back()->mScore = newCost;
				}
				else if (onListResult != BETTER_FOUND) //A node was found but it was not better
				{
					closedList[onListResult]->mpParent = current.get(); //Set current as the node's new parent
					closedList[onListResult]->mScore = newCost; //Set the nodes score to the new score.
					openList.push_back(move(closedList[onListResult])); //Move the node from the closed list to the open list.
					closedList.erase(closedList.begin() + onListResult); //Erase the now empty spot.
				}
			}
			else if (onListResult != BETTER_FOUND) //A node was found but it was not better
			{
				openList[onListResult]->mpParent = current.get(); //Set current as the node's new parent
				openList[onListResult]->mScore = newCost; //Set the nodes score to the new score.
			}
		}
	}
	//West
	if (current->x - 1 >= 0) //Ensure that it is within the bounds of the map. EG, in a 10 by 10 map, if y + 1 = 10, not valid.
	{
		//If the next node is not a wall, nor on the open or closed lists, add it to openlist.
		if (terrain[current->x - 1][current->y] != ENodeType::wall) //Is not a wall
		{
			//If the next node is not found on the openlist, check the closedlist. If it's not found on closedlist, generate and push onto openlist.
			//If the next node is found on the openlist and is better than the one on the openlist, edit the data on the openlist.
			//If the next node is found on the closedlist and is better than the one on the closedlist, move it to the openlist and edit the data.
			//New cost = base cost + terrain cost
			newCost = current->mScore - goal->CalculateManhattanDistance(current->x, current->y) + 
					  terrain[current->x - 1][current->y] + goal->CalculateManhattanDistance(current->x - 1, current->y);
			onListResult = CheckListForBetter(openList, current->x - 1, current->y, newCost);
			if (onListResult == NODE_NOT_FOUND) //Is not on the open list
			{
				onListResult = CheckListForBetter(closedList, current->x - 1, current->y, newCost);
				if (onListResult == NODE_NOT_FOUND) //Is not on the closed list
				{
					//Set up the node data, then move onto the open list.
					openList.push_back(move(unique_ptr<SNode>(new SNode)));
					openList.back()->x = current->x - 1;
					openList.back()->y = current->y;
					openList.back()->mpParent = current.get();
					//New score = new cost + heuristic
					openList.back()->mScore = newCost;
				}
				else if (onListResult != BETTER_FOUND) //A node was found but it was not better
				{
					closedList[onListResult]->mpParent = current.get(); //Set current as the node's new parent
					closedList[onListResult]->mScore = newCost; //Set the nodes score to the new score.
					openList.push_back(move(closedList[onListResult])); //Move the node from the closed list to the open list.
					closedList.erase(closedList.begin() + onListResult); //Erase the now empty spot.
				}
			}
			else if (onListResult != BETTER_FOUND) //A node was found but it was not better
			{
				openList[onListResult]->mpParent = current.get(); //Set current as the node's new parent
				openList[onListResult]->mScore = newCost; //Set the nodes score to the new score.
			}
		}
	}

	sort(openList.begin(), openList.end(), CompareScores);
	closedList.push_back(move(current));
	path[0]->mScore++;

	if (openList.empty())
	{
		return EStepPathResults::NO_PATH;
	}
	else
	{
		return EStepPathResults::STEP_SUCCESS;
	}
}