//Leo Croft

#include "SearchUtilities.h"

bool SNode::NodesMatch(const SNode* comparison)
{
	if (x == comparison->x && y == comparison->y) return true;
	else return false;
}

bool IsNodeOnList(deque<unique_ptr<SNode>> &list, int nodeX, int nodeY)
{
	for (auto it = list.begin(); it != list.end(); it++)
	{
		//If this node matches the coordinates of a node on the list, return true.
		if ((*it)->x == nodeX && (*it)->y == nodeY)
		{
			return true;
		}
	}
	return false;
}

int CheckListForBetter(deque<unique_ptr<SNode>> &list, int nodeX, int nodeY, int nodeScore)
{
	for (auto it = list.begin(); it != list.end(); it++)
	{
		//If this node matches the coordinates of a node on the list, then the node has already been seen
		if ((*it)->x == nodeX && (*it)->y == nodeY)
		{
			//If the node on the list has a better score than the one being checked, return the error.
			//Otherwise, return the index.
			//If the score is the same, return the code so that data isn't needlessly updated.
			if ((*it)->mScore <= nodeScore)
			{
				return BETTER_FOUND;
			}
			else
			{
				return (it - list.begin());
			}
		}
	}
	//If the node is not found, return the message.
	return NODE_NOT_FOUND;
}

//Calculates the difference between this node and a destination.
int SNode::CalculateManhattanDistance(int newX, int newY)
{
	int result = abs(newX - x) + abs(newY - y); //Difference in X + Difference in Y
	if (result == 0) result = INT_MIN;
	return result;
}

//Searches the closed list for the nodes on the path and moves them onto the path list.
void BuildPath(NodeList &path, NodeList &closedList, unique_ptr<SNode> current)
{
	//Check the closed list for each of the nodes in the path parentage.
	//Reverse iterate through the list until the node is found.
	//Loops until the first item in closed list, which is always the start.

	//Push the final node onto the front of the path to build it backwards.
	path.push_front(move(current));

	SNode* frontTracker = path.front()->mpParent; //Tracks the node that will be added to the path.

	while (frontTracker  != 0)
	{
		path.push_front(unique_ptr<SNode>(new SNode));
		path[0]->x = frontTracker->x;
		path[0]->y = frontTracker->y;
		frontTracker = frontTracker->mpParent;
	}

	//The old way of bulding the path.
	//for (NodeList::reverse_iterator it = closedList.rbegin(); it != closedList.rend(); it++)
	//{
	//	if ((*it).get() == path.front()->mpParent) //Compare the pointers. If they match, it's the right node.
	//	{
	//		path.push_front(move(*it));
	//	}
	//}
}

bool CompareScores(unique_ptr<SNode> &i, unique_ptr<SNode> &j)
{
	return(i->mScore < j->mScore);
}