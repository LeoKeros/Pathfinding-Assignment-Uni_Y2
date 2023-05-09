//Leo Croft

// SearchFactory.h
// ===============
//
// Factory function declaration to create CSearchXXX objects 
//

#pragma once

#include "Search.h" // Search interface class

// List of implemented seach algorithms
enum ESearchType
{
  BreadthFirst,
  //Dijkstra,
  AStar,
  
  /* TODO - Add type elements for each implemented search */

  NumOfSearches //The number of available searches, to remove magic numbers
};

// Factory function to create CSearchXXX object where XXX is the given search type
ISearch* NewSearch(ESearchType search);
