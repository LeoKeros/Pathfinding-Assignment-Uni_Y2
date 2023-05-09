//Leo Croft

// SearchAStar.h
// ======^^^^^^==
//
// Declaration of Search class for Breadth First algorithm
//

#pragma once

#include "Definitions.h"  // Type definitions
#include "Search.h"       // Base (=interface) class definition

// Breadth First search class definition

// Inherit from interface and provide implementation for 0* algorithm
class CSearchAStar: public ISearch
{
	// I have not implemented any constructors or destructors.
	// Whether you need some is up to how you choose to do your implementation.

	// Constructs the path from start to goal for the given terrain
	bool FindPath(TerrainMap& terrain, unique_ptr<SNode> start, unique_ptr<SNode> goal, NodeList& path);

	// Performs a single step of the FindPath function.
	// Performs the function of the loop in FindPath. Start should be the first node in openlist the first time StepPath is called.
	// Takes the openlist and closedlist as additionally reference parameters; These are used to set textures and create models.
	// Goal is passed as a reference parameter because it is used for comparison; It is not added onto the openlist until it is found by the search.
	EStepPathResults StepPath(TerrainMap& terrain, NodeList& openList, NodeList& closedList, unique_ptr<SNode>& goal, NodeList& path);
};