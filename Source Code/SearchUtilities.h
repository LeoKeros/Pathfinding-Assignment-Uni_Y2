//Leo Croft

#pragma once

#include "Definitions.h"  // Type definitions

//Follows the path backwards from the goal (current) to build the path from nodes on the closedlist.
void BuildPath(NodeList &path, NodeList &closedList, unique_ptr<SNode> current);

//Compare the node against each of the nodes on the list. Return true if found, false otherwise.
bool IsNodeOnList(deque<unique_ptr<SNode>> &list, int nodeX, int nodeY);

//Same as IsNodeOnList, but also compares the scores, and returns an int.
//The int is either the index of the node if it exists and is worse, -1 if the node isn't found, or -2 if the node found was better.
int CheckListForBetter(deque<unique_ptr<SNode>> &list, int nodeX, int nodeY, int nodeScore);

bool CompareScores(unique_ptr<SNode> &i, unique_ptr<SNode> &j); //Returns true is the score of I is smaller than the score of J.