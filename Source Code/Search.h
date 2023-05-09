//Leo Croft

// Search.h
// ========
//
// Interface for all search classes
//

#pragma once

#include "Definitions.h" // type definitions
#include "SearchUtilities.h" //Functions shared between search solutions

// ISearch interface class - cannot be instantiated
// Implementation classes for specific search algorithms should inherit from this interface
class ISearch
{
public:
  // Interfaces don't need a constructor - makes sense really, given they cannot be instantiated.

  // They DO need a virtual destructor if there are other virtual functions in the interface.
  // This compels derived objects destructors to be called as appropriate.
  // However, at the interface level, there is nothing to do, so the body is empty.
  virtual ~ISearch() {}

  // PATH FINDING FUNCTIONS
  // ----------------------

  // Constructs the path from start to goal for the given terrain
  // Pure Virtual function to be implemented in derived class.
  virtual bool FindPath(TerrainMap& terrain, unique_ptr<SNode> start, unique_ptr<SNode> goal, NodeList& path) = 0;

  // Performs a single step of the FindPath function.
  // Performs the function of the loop in FindPath. Start should be the current node the first time StepPath is called.
  // Takes the openlist and closedlist as additionally reference parameters; These are used to set textures and create models.
  // Goal is passed as a reference parameter because it is used for comparison; It is not added onto the openlist until it is found by the search.
  virtual EStepPathResults StepPath(TerrainMap& terrain, NodeList& mOpenList, NodeList& mClosedList, unique_ptr<SNode>& goal, NodeList& path) = 0;
  /* TODO - Only for high marks
     Add a pure virtual function declaration to perform one iteration of the path-finding loop.
     This is in support of showing the search in real time.
     The main program will need to display somehow:
     - the open list
     - the closed list
     - the path to the current node */
};