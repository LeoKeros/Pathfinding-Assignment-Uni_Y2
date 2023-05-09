#include "DisplayClasses.h"

// When the Map Handler is created, create the camera and models.
//Scale down the models so that it fits a model per unit square.
CMapHandler::CMapHandler(I3DEngine* myEngine)
{
	mpMapCam = myEngine->CreateCamera(kManual);
	mpMapCam->RotateLocalX(RIGHT_ANGLE); //Angle the camera to point down at the map.

	mpGridSpaceMesh = myEngine->LoadMesh(GRID_SPACE_MESH);
	mpBackground = mpGridSpaceMesh->CreateModel(0.0f, -40.0f, 0.0f);
	mpBackground->ScaleX(400.0f);
	mpBackground->ScaleZ(400.0f);
	mpBackground->SetSkin(NODE_SKINS[3]);
	SetupGrid();
}

//Setters and getters
//When setting the start and end nodes, change the texture of the current node before changing the texture of the new node.
void CMapHandler::SetStartNode(const SIntVector &coords)
{
	mMapVisual[NodeIndex(mStartNode)].mpBody->SetSkin(NODE_SKINS[mMapData[mStartNode.x][mStartNode.y]]); // Set the texture of the old start node.

	mStartNode.x = coords.x;
	mStartNode.y = coords.y;

	mMapVisual[NodeIndex(mStartNode)].mpBody->SetSkin(NODE_SKINS[ENodeType::start]); //Set the texture of the start node.
}
SIntVector CMapHandler::GetStartNode()
{
	return mStartNode;
}
void CMapHandler::SetEndNode(const SIntVector &coords)
{
	mMapVisual[NodeIndex(mEndNode)].mpBody->SetSkin(NODE_SKINS[mMapData[mEndNode.x][mEndNode.y]]); // Set the texture of the old start node.

	mEndNode.x = coords.x;
	mEndNode.y = coords.y;

	mMapVisual[NodeIndex(mEndNode)].mpBody->SetSkin(NODE_SKINS[ENodeType::end]); //Set the texture of the start node.
}
SIntVector CMapHandler::GetEndNode()
{
	return mEndNode;
}
SIntVector CMapHandler::GetDimensions()
{
	return mDimensions;
}
CVector3 CMapHandler::GetNodePosition(const int &x, const int &z) //Get the world position of the centre of the chosen grid space.
{
	return mMapVisual[NodeIndex(x, z)].mPos;
}
void CMapHandler::SetSearchSelection(const short &selection)
{
	mSearchSelected = (ESearchType)selection;
}
ESearchType CMapHandler::GetSearchSelection()
{
	return mSearchSelected;
}

IModel* CMapHandler::BuildFromGridspaceMesh(CVector3 position) //Create a model from the grid space mesh and return it. This allows external usage of the mesh without loading it in multiple times, or allowing access to it.
{
	return mpGridSpaceMesh->CreateModel(position.x, position.y, position.z);
}

//Tests if a map has been selected.
bool CMapHandler::MapSelected()
{
	return mMapSelected.empty();
}

//Convert the coordinates for a node into the index for the mMapVisual vector.
int CMapHandler::NodeIndex(SIntVector node)
{
	return ((mDimensions.x * node.y) + node.x);
}
int CMapHandler::NodeIndex(int x, int z)
{
	return ((mDimensions.x * z) + x);
}

/** Camera Controls **/
void CMapHandler::ZoomCamOut(float frameTime)
{
	mpMapCam->MoveY(frameTime);
}
void CMapHandler::ZoomCamIn(float frameTime)
{
	mpMapCam->MoveY(-frameTime);
}

/** Set up the map display (physical and texture) **/
//Create enough models to display the  grid
void CMapHandler::SetupGrid()
{
	//The minimum number of grid spaces generated is for a 10x10 grid.
	float numGridSpaces = mDimensions.x * mDimensions.y;
	if (numGridSpaces < BASE_BLOCK_NUMBER)
	{
		numGridSpaces = BASE_BLOCK_NUMBER;
	}

	/*
	Populates a 1D vector of grid spaces to ensure there are enough to create a grid of the required dimensions.
	If there are already enough spaces, no new spaces need to be generated.
	Moves blocks that won't be used out of sight, and arranges the rest in a grid.
	*/
	float oldSize = mMapVisual.size();

	//If the new dimensions require more gridspaces than are currently in use, resize.
	if (oldSize < numGridSpaces)
	{
		mMapVisual.resize(numGridSpaces);
		//Generate new models so that the number of models matches the number of grid spaces
		for (int count = oldSize; count < numGridSpaces; count++)
		{
			mMapVisual[count].mpBody = mpGridSpaceMesh->CreateModel();
		}
	}

	//Position all of the models.
	for (int count = 0; count < mMapVisual.size(); count++)
	{
		//Move models within the dimensions to line up in the grid.
		//Using count instead of an iterator so that the index can be used to calculate the coordinates.
		if (count < (mDimensions.x * mDimensions.y))
		{
			mMapVisual[count].mPos = { float(count % mDimensions.x), //EG, in a 10x10 grid, count = 47 x = 7
									MAP_LEVEL_Y,
									float(count / mDimensions.x) }; //EG, in a 10x10 grid, count = 44, z = 4
			mMapVisual[count].mpBody->SetPosition(mMapVisual[count].mPos.x, mMapVisual[count].mPos.y, mMapVisual[count].mPos.z);
		}
		else //Move excess models out of view.
		{
			mMapVisual[count].mPos = CVector3(0.0f, SPAWN_Y, 0.0f);
			mMapVisual[count].mpBody->SetPosition(0.0f, SPAWN_Y, 0.0f);
		}
	}

	//Position the camera so it can see the entire grid.
	//Can fit more spaces horizontally than vertically
	mpMapCam->SetPosition((mDimensions.x / 2.0f) - 0.5f, //Half and shift over by half a unit.
		mDimensions.y, //Number of spaces in Z axis + 0.6
		(mDimensions.y / 2.0f) - 0.5f); //Half and shift over by half a unit.
}

//Set the textures of the models used to display the map.
void CMapHandler::SetupTerrain()
{
	//Setting types
	for (int x = 0; x < mDimensions.x; x++)
	{
		for (int z = 0; z < mDimensions.y; z++)
		{
			mMapVisual[NodeIndex(x, z)].mpBody->SetSkin(NODE_SKINS[mMapData[x][z]]);
		}
	}

	mMapVisual[NodeIndex(mStartNode)].mpBody->SetSkin(NODE_SKINS[ENodeType::start]); //Set the texture of the start node.
	mMapVisual[NodeIndex(mEndNode)].mpBody->SetSkin(NODE_SKINS[ENodeType::end]); //Set the texture of the end node.
}

/** Path/Search demonstration manipulation **/
//If there aren't enough models available to demonstrate the search, push more onto the end.
//Loop through openlist and closedlist, place them just above the nodes in the list and apply the relevant textures.
void CMapHandler::SetupSearchDemo()
{
	int overlaysRequired = mOpenList.size() + mClosedList.size(); // The number of overlays required to display the search.
	int oldSize = mSearchOverlay.size();
	//If there are not enough overlays available, resize to make more.
	if (mSearchOverlay.size() < overlaysRequired)
	{
		mSearchOverlay.resize(overlaysRequired);
	}
	for (int i = oldSize; i < mSearchOverlay.size(); i++)
	{
		mSearchOverlay[i] = mpGridSpaceMesh->CreateModel();
	}

	int overlayCount = 0; //Track which of the overlay pieces in mSearchOverlay is being modified.
	CVector3 position;

	//Check each element in each list and put an overlay on it's position.
	//Set the texture of the overlay according to whether it represents a node on the closed list or open list.
	for (auto it = mOpenList.begin(); it != mOpenList.end(); it++)
	{
		position = GetNodePosition((*it)->x, (*it)->y);
		mSearchOverlay[overlayCount]->SetPosition(position.x, position.y + OVERLAY_Y, position.z);
		mSearchOverlay[overlayCount]->SetSkin(OPENLIST_TEXTURE);
		overlayCount++;
	}
	for (auto it = mClosedList.begin(); it != mClosedList.end(); it++)
	{
		position = GetNodePosition((*it)->x, (*it)->y);
		mSearchOverlay[overlayCount]->SetPosition(position.x, position.y + OVERLAY_Y, position.z);
		mSearchOverlay[overlayCount]->SetSkin(CLOSEDLIST_TEXTURE);
		overlayCount++;
	}
}

//If the first overlay isn't at the spawn position, set all overlays to the spawn position.
void CMapHandler::HideSearchDemo()
{
	if (mSearchOverlay.size() > 0)
	{
		if (mSearchOverlay[0]->GetY() != SPAWN_Y)
		{
			for (auto it = mSearchOverlay.begin(); it != mSearchOverlay.end(); it++)
			{
				(*it)->SetY(SPAWN_Y);
			}
		}
	}
}

/** File input and output **/
//Take user input for file name and check for a map file.
void CMapHandler::ReadMap()
{
	string userInput;
	string mapFile;
	ifstream mapReader;
	string coordFile;
	ifstream coordReader;

	//Get the user's input
	cout << MAP_CMD_PROMPT;

	//Loops until a return statement is hit.
	while (true)
	{
		userInput = ""; //Resets the userInput to empty.
		cin >> userInput; //Reads the user's input. 

		//Add the file end to the user's input to get both of the file names.
		mapFile = userInput + string(MAP_FILE_EXTENSION);
		coordFile = userInput + string(COORD_FILE_EXTENSION);

		//Attempt to open the map file. If successful, attempt to open the Coord file.
		//If both are successful, process the data in the files.
		mapReader.open(mapFile);
		if (mapReader) //If the map file was opened successfully
		{
			cout << MAP_FILE_SUCCESS << mapFile << endl;

			coordReader.open(coordFile);
			if (coordReader) //If the coordinates file was opened successfully
			{
				cout << COORD_FILE_SUCCESS << coordFile << endl;

				//Read the map file
				mapReader >> mDimensions.x >> mDimensions.y; //Getting the dimensions from the first line

				mMapData.resize(mDimensions.x); //Resize the data vector to fit every column.

				for (int x = 0; x < mDimensions.x; x++)
				{
					mMapData[x].resize(mDimensions.y); //Resize each of the columns to match the number of rows.
				}

				for (int z = mDimensions.y - 1; z >= 0; z--)
				{
					//Read each of the individual spaces from the file as characters, then convert it into an integer and then into the node type.
					for (int x = 0; x < mDimensions.x; x++)
					{
						char nodeScore;
						mapReader >> nodeScore;

						//Before being parsed into the type, the character has the ascii character for 0 removed to translate it into an int.
						mMapData[x][z] = ENodeType(nodeScore - '0');
					}
				}
				mapReader.close();

				//Read the Coord File. This gives the Start and End positions.
				coordReader >> mStartNode.x >> mStartNode.y;
				coordReader >> mEndNode.x >> mEndNode.y;

				coordReader.close();

				//Setup the visual representation of the grid, then set the skins.
				SetupGrid();
				SetupTerrain();
				mMapSelected = userInput;
				return;
			}
			else
			{
				cout << COORD_FILE_ERROR << endl;
			}
		}
		else
		{
			cout << MAP_FILE_ERROR << endl;
		}
	}
}

//Take the path generated by the search and output it to a file.
void CMapHandler::SaveResultsToFile(NodeList &path)
{
	ofstream outFile(PATH_OUTPUT_FILE);
	if (outFile)
	{
		for (NodeList::iterator it = path.begin(); it != path.end(); it++)
		{
			outFile << (*it)->x << ", " << (*it)->y << endl;
		}
		outFile.close();
	}
}