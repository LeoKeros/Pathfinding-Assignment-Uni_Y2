//Leo Croft
#pragma once
#ifndef __MAPDEF
#define __MAPDEF

#include "CMatrix4x4.h"
#include "CVector3.h"
#include <string>
#include <TL-Engine.h>
#include <iostream>
#include <fstream>
//This definitions header is only used by Pathfinding.cpp, so it's safe to include these here. Also allows access to Enums and other stuff.
#include "Search.h"
#include "SearchFactory.h"
#include <math.h>

using namespace tle;
using namespace std;

struct SIntVector
{
	int x;
	int y;

};

const unsigned short BASE_BLOCK_NUMBER = 100; //The number of models created for the expected size of grid (10x10)

const float SPAWN_Y = -50.0f; //This is the y coordinate where world objects are placed when not in use. It is hidden above the camera, out of view.
const float MAP_LEVEL_Y = 0.0f; //This is the Y coordinate of the map, so blocks in use are placed here.

//Avoiding magic numbers.
const float HALF_UNIT = 0.5f; //A unit is 1, this is to avoid a magic number.
const float BALL_SPEED = 10.0f;
const float OVERLAY_Y = 0.001f; //This value is added onto the position of the grid to get the overlay's y position.
const float STEP_TIMER = 0.2f; //The time delay between single steps in the path.
const float PATH_COLLISION_DIST = 0.05f; //The collision distance for the ball moving between path points.
const float RIGHT_ANGLE = 90.0f;

/** UI element constants **/
/* Font */
const string SELECTION_TEXT_FONT = "Comic Sans MS";
const short int SELECTION_TEXT_FONTSIZE = 16;

/* Textures */
const string LEFT_ARROW_TEXTURE = "LeftArrow.png";
const string RIGHT_ARROW_TEXTURE = "RightArrow.png";
const string TEXT_DISPLAY_TEXTURE = "uifront.png";
const string UI_BACKDROP_TEXTURE = "uibackground.png";

/* Placement */
const float UI_WIDTH = 300.0f;
const float UI_HEIGHT = 50.0f;
const float UI_VERT_GAP = 10.0f;
const float UI_HOR_GAP = 5.0f;
const float UI_ARROW_WIDTH = 50.0f;
const float UI_BACKDROP_LAYER = 0.2f;
const float UI_ELEMENTS_LAYER = 0.1f;
const float UI_TEXT_LAYER = 0;

//Text input/outputs
const string EXIT_COMMAND = "cancel";

const string MAP_CMD_PROMPT = "Please input the name of the map you wish to use in full (EG: for xMap.txt and xCoords.txt, input \"x\"): ";
const string MAP_INPUT_PROMPT = "See CMD";
const string NODE_SELECTION_INFO = "WASD to select"; //WASD to select, Enter to confirm.
const string END_DEMO_OUTPUT = "Stop Path"; //Press enter to stop pathing.
const string SEARCH_FAIL = "Search failed";
const string ASTAR_SEARCH_COUNT_OUTPUT = "Number of Searches during A* Search: ";

const string MAP_FILE_EXTENSION = "Map.txt";
const string MAP_FILE_SUCCESS = "Map file confirmed.";
const string MAP_FILE_ERROR = "Map file not found; Try again.";

const string COORD_FILE_EXTENSION = "Coords.txt";
const string COORD_FILE_SUCCESS = "Cooord file confirmed.";
const string COORD_FILE_ERROR = "Coord file not found; Try again.";

const string PATH_OUTPUT_FILE = "output.txt"; //The name of the file to output the path to.

//UI output for selecting options
enum EOptions { ChooseMap, ChooseStart, ChooseEnd, ChooseSearch, FindPath, StepPath, NumOfOptions }; //NumOfOptions should always be last
const string OPTIONS[EOptions::NumOfOptions] = { "Choose Map", "Choose Start", "Choose End",
												 "Choose Search", "Use ", "Step " }; // "Use <Algorithm>" and "Step <Algorithm>"
const string SEARCH_TYPES[ESearchType::NumOfSearches] = { "Breadth First", "AStar" }; //The text outputs so users can pick their search.

const string PATH_TEXTURE = "PathArrow.png"; //This texture is used to show the nodes on the path.
const string OPENLIST_TEXTURE = "openListDisplay.png"; //This texture is used to show nodes in the openlist.
const string CLOSEDLIST_TEXTURE = "closedListDisplay.png"; //This texture is used to show nodes in the closedlist.
const string END_DOT_DEMO = "endDot.png"; //This texture is used to show where the end node will be positioned after selection.
const string START_DOT_DEMO = "startDot.png"; //This texture is used to show where the start node will be positioned after selection.

//Controls
const EKeyCode NODE_SELECT_X_NEG = Key_A; //Press the A key to move negatively along the X axis 
const EKeyCode NODE_SELECT_X_POS = Key_D; //Press the D key to move positively along the X axis 
const EKeyCode NODE_SELECT_Z_NEG = Key_S; //Press the W key to move negatively along the Z axis 
const EKeyCode NODE_SELECT_Z_POS = Key_W; //Press the S key to move positively along the Z axis 
const EKeyCode PREV_SELECT = Key_A; //A key selects previous option.
const EKeyCode NEXT_SELECT = Key_D; //D key selects the next option.
const EKeyCode SELECT = Key_Return; //Enter key selects the current option.
const EKeyCode CAM_ZOOM_IN = Key_Up; //Up arrow key zooms camera in.
const EKeyCode CAM_ZOOM_OUT = Key_Down; //Down arrow key zooms camera out.

const string NODE_SKINS[6] = { "wall.png", "clear.png", "wood.png", "water.png", "start.png", "end.png" };

const string GRID_SPACE_MESH = "quad.x"; //Quad mesh edited to be 1 unit in size and facing along the Y axis.
const string BALL_MESH = "Bullet.x";


/** These states are used in conjunction to allow flexibility in the program **/
/** Gamestates are used to tell the program the current operation.
	EG, when choosing a search, the program will set the state to ChooseSearch, then at the start of the following search it will make it possible to choose.**/
	//This is used to track the state of the program.
enum EGameState
{
	Setup, //During this state, the user is able to make selections of options.
	ChoosingMap, //During this state, the user is picking the map.
	ChoosingSearch, //During this state, the pathfinding algorithm is being selected.
	ChoosingStart, //During this state, the user is selecting a start point.
	ChoosingEnd, //During this state, the user isa selecting an end point.
	Finding, //During this state, the search is being used to find the path.
	StepFind, //Performs a single step in a search.
	Pathing, //During this state, the ball is demonstating the path. The ball's state will be used at this point.
	SearchFail //During this state, output a message to alert the user to the failure.
};

//This is used to track the behaviour of the ball while pathing.
enum EBallState
{
	Spawning, //During this state, the ball is being placed at the start node.
	Rolling, //During this state, the ball is following the path,
	Ending, //During this state, the ball is leaving the user's view so the path can restart.
};


//Data about the visual side of the map.
struct SGridSpace
{
	IModel* mpBody;
	CVector3 mPos;
};

// The grid of models is 2D, but the vector used to handle it is 1D.
// Handling is done manually based upon X and Y coordinates and the dimensions in use.
// This is to avoid creating extra models for, for example, a 10x10 grid vs a 5x20 grid.
using GridVisual = vector<SGridSpace>;

#endif