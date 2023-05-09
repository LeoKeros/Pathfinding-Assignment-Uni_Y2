//Leo Croft
// Pathfinding.cpp: A program using the TL-Engine

#include "DisplayClasses.h"

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./Media");

	/**** Set up your scene here ****/

	unique_ptr<CMapHandler> map(new CMapHandler(myEngine));
	unique_ptr<CBallHandler> ball(new CBallHandler(myEngine->LoadMesh(BALL_MESH)));
	//ball->mpTheMap = map.get(); //Give the ball a reference to the map, so that it can see the world positions of the grid spaces.
	ISearch* pathFinder = nullptr;

	//UI creation
	//A backdrop with a white text-display and arrows on either side, positioned at the bottom center of the screen.
	float uiPlacementX = (myEngine->GetWidth() / 2.0f) - (UI_WIDTH / 2.0f);
	float uiPlacementY = myEngine->GetHeight() - UI_HEIGHT;

	IFont* uiSelectionText = myEngine->LoadFont(SELECTION_TEXT_FONT, SELECTION_TEXT_FONTSIZE);

	ISprite* selectionAreaBackdrop = myEngine->CreateSprite(UI_BACKDROP_TEXTURE,
															uiPlacementX, 
															uiPlacementY, 
															UI_BACKDROP_LAYER);
	ISprite* prevSelectionArrow = myEngine->CreateSprite(LEFT_ARROW_TEXTURE,
														 uiPlacementX + UI_HOR_GAP, 
														 uiPlacementY + UI_VERT_GAP, 
														 UI_ELEMENTS_LAYER);
	ISprite* selectionTextArea = myEngine->CreateSprite(TEXT_DISPLAY_TEXTURE, 
														uiPlacementX + (UI_HOR_GAP * 2.0f) + UI_ARROW_WIDTH, 
														uiPlacementY + UI_VERT_GAP, 
														UI_ELEMENTS_LAYER);
	ISprite* nextSelectionArrow = myEngine->CreateSprite(RIGHT_ARROW_TEXTURE,		
														 uiPlacementX + (UI_WIDTH - UI_HOR_GAP - UI_ARROW_WIDTH), 
														 uiPlacementY + UI_VERT_GAP, 
														 UI_ELEMENTS_LAYER);

	short int optionSelected = 0; //Tracks which option is currently being displayed to the user.
	EGameState state = EGameState::Setup;
	string textSelectionOutput; //To be output in the user input section.
	SIntVector nodeSelectionPos; //Used to allow the user to select start / end positions.
	IModel* nodeSelectionModel = myEngine->LoadMesh(GRID_SPACE_MESH)->CreateModel(0.0f, SPAWN_Y, 0.0f);
	float delayTimer = 0.0f; //Track the passage of time between particular calls. EG, steps in single-step pathing.
	NodeList path;

	/*Declaring and instantiating variables that affect how objects and the camera
	in the program move.*/
	float frameTime;
	myEngine->Timer();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		//The text displayed is based on the previous frame. Placed here 
		uiSelectionText->Draw(textSelectionOutput, uiPlacementX + (UI_WIDTH / 2.0f), uiPlacementY + (UI_HEIGHT / 2.0f), kBlack, kCentre, kVCentre);
		textSelectionOutput = ""; //The text to be displayed is reset after it's drawn.

		myEngine->DrawScene();
		frameTime = myEngine->Timer();
		
		if (state == EGameState::ChoosingMap)
		{
			map->ReadMap();
			state = EGameState::Setup;
		}
		else if (state == EGameState::Finding)
		{
			//These unique pointers are defined locally because, when they are moved, they are removed from their original position.
			unique_ptr<SNode> start(new SNode{ map->GetStartNode().x, map->GetStartNode().y, 0 });
			map->mpGoal.reset(new SNode{ map->GetEndNode().x, map->GetEndNode().y, 1 });
			path.clear(); //Empty the path of existing nodes to prevent multiple-inclusion of nodes.

			if (pathFinder->FindPath(map->mMapData, move(start), move(map->mpGoal), path))
			{
				//cout << "Testing if the unique pointers for start and goal are empty after FindPath call."; //They were
				state = EGameState::Pathing;
				map->SaveResultsToFile(path);
				if (map->GetSearchSelection() == ESearchType::AStar) cout << ASTAR_SEARCH_COUNT_OUTPUT << path.back()->mScore << endl;
				ball->SetPath(path, map.get());
				ball->SpawnBall();
				ball->SetModelMatrix();
			}
			else
			{
				state = EGameState::SearchFail;
			}
		}
		else if (state == EGameState::StepFind)
		{
			//There is a delay between each step call.
			if (delayTimer <= 0)
			{
				delayTimer = STEP_TIMER;
				//These unique pointers are defined locally because, when they are moved, they are removed from their original position.
				switch (pathFinder->StepPath(map->mMapData, map->mOpenList, map->mClosedList, map->mpGoal, path))
				{
					// After each step, 
				case EStepPathResults::STEP_SUCCESS: //If the search was successful, but the goal wasn't found, display the closed and open lists.
					map->SetupSearchDemo();
					break;
				case EStepPathResults::PATH_FOUND: //If the goal was found, demonstrate the pathing.
					state = EGameState::Pathing;
					map->SaveResultsToFile(path);
					if (map->GetSearchSelection() == ESearchType::AStar) cout << ASTAR_SEARCH_COUNT_OUTPUT << path.back()->mScore << endl;
					ball->SetPath(path, map.get());
					ball->SpawnBall();
					ball->SetModelMatrix();
					break;
				case EStepPathResults::NO_PATH: //If the path doesn't exist, alert the user.
					state = EGameState::SearchFail;
					break;
				}
			}
			else
			{
				delayTimer -= frameTime;
			}
			
		}
		else if (state == EGameState::Pathing)
		{
			//Set output text
			textSelectionOutput = END_DEMO_OUTPUT;
			ball->MoveBall(frameTime);

			//Press the select button to stop pathing.
			//Changes the game state and moves the ball out of sight.
			if (myEngine->KeyHit(SELECT))
			{
				state = EGameState::Setup;
				ball->HideBall();
				map->HideSearchDemo();
				ball->HidePathDemo();
			}
			ball->SetModelMatrix();
		}
		else if (state == EGameState::SearchFail) //Output a message and wait for the user to press the select button to continue.
		{
			//Set the output
			textSelectionOutput = SEARCH_FAIL;

			//Waits until the user presses the select button to resume operations.
			if (myEngine->KeyHit(SELECT))
			{
				state = EGameState::Setup;
				map->HideSearchDemo();
			}
		}

		
		//Cycle through the available options, using the options enums to avoid going  out of bounds.
		//Options are only available in setup and Search Selection.
		//When a map has not been loaded, the only option available is to load the map.
		//When a map has been loaded, the options available additionally include selecting start and end points, and choosing a search.
		//When a search has been selected, the options available additionally include starting the search.
		//When a search is active, the only option will be to quit the search.
		//A key is used to confirm the selection.
		if (state == EGameState::Setup)
		{
			textSelectionOutput = OPTIONS[optionSelected];
			if (optionSelected == EOptions::FindPath || optionSelected == EOptions::StepPath)
			{
				textSelectionOutput = textSelectionOutput + SEARCH_TYPES[map->GetSearchSelection()]; //Add the selected search onto the "Find Path" option.
			}

			//The options available during the setup change depending on criteria.
			if (myEngine->KeyHit(NEXT_SELECT)) //The last available option is the upper limit.
			{
				optionSelected++;
				if (map->MapSelected())
				{
					optionSelected = EOptions::ChooseMap; //Only map selection is available at this point.
				}
				else if (map->GetSearchSelection() == ESearchType::NumOfSearches)
				{
					optionSelected = optionSelected % (EOptions::ChooseSearch + 1); //Only options up to ChooseSearch are available at this point.
				}
				else
				{
					optionSelected = optionSelected % (EOptions::NumOfOptions); //All options are unlocked at this point.
				}
			}
			if (myEngine->KeyHit(PREV_SELECT)) //The current option is set to the last available option it goes below 0.
			{
				optionSelected--;
				if (optionSelected < 0)
				{
					if (map->MapSelected())
					{
						optionSelected = EOptions::ChooseMap; //Only ChooseMap is available.
					}
					else if (map->GetSearchSelection() == ESearchType::NumOfSearches)
					{
						optionSelected = EOptions::ChooseSearch; //Options up to choosing a search are available.
					}
					else
					{
						optionSelected = EOptions::NumOfOptions - 1; //All options are unlocked
					}
				}
			}
			if (myEngine->KeyHit(SELECT)) //The current option is checked against the options Enum and performs the appropriate action.
			{
				switch (optionSelected)
				{
				case EOptions::ChooseMap: //If the user selects to choose a map, call in the ReadMap function
					state = EGameState::ChoosingMap;
					textSelectionOutput = MAP_INPUT_PROMPT;
					break;
				case EOptions::ChooseSearch: //If the user selects to choose a search, set the gamestate to "ChoosingSearch". In this state, choose like option.
					state = EGameState::ChoosingSearch;
					break;
				case EOptions::ChooseStart: //If the user selects to choose a start point, change the control scheme to allow selection.
					state = EGameState::ChoosingStart;
					nodeSelectionPos = { 0,0 };
					nodeSelectionModel->SetPosition(0, MAP_LEVEL_Y + OVERLAY_Y, 0);
					nodeSelectionModel->SetSkin(START_DOT_DEMO);
					break;
				case EOptions::ChooseEnd: //If the user selects to choose an end point, change the control scheme to allow selection.
					state = EGameState::ChoosingEnd;
					nodeSelectionPos = { 0,0 };
					nodeSelectionModel->SetPosition(0, MAP_LEVEL_Y + OVERLAY_Y, 0);
					nodeSelectionModel->SetSkin(END_DOT_DEMO);
					break;
				case EOptions::FindPath: //If the user selects to find the path, call the selected pathfinding algorithm.
					state = EGameState::Finding;
					break;
				case EOptions::StepPath: //If the user selects to find the path step by step, 
					state = EGameState::StepFind;
					
					//The following steps are done here because the StepFind gamestate is used to track the step-by-step progress.
					path.clear(); //Empty the path of existing nodes to prevent multiple-inclusion of nodes.
					
					if(map->GetSearchSelection() == ESearchType::AStar)	path.push_back(unique_ptr<SNode>(new SNode)); //The score value on this node will be used to track the count.

					//Reset the search lists when the user selects to search step-by-step, to ensure they are empty.
					map->mOpenList.clear();
					map->mClosedList.clear();
					
					//Add the start node to the openlist for the first call.
					unique_ptr<SNode> tmp(new SNode{ map->GetStartNode().x, map->GetStartNode().y, 0 });
					SNode tmpGoal = { map->GetEndNode().x, map->GetEndNode().y };
					tmp->mScore = tmpGoal.CalculateManhattanDistance(tmp->x, tmp->y);
					map->mOpenList.push_back(move(tmp));

					//Set the goal.
					map->mpGoal.reset(new SNode{ map->GetEndNode().x, map->GetEndNode().y, 1 });					
					break;
				}
				optionSelected = 0;
			}
		}
		else if (state == EGameState::ChoosingSearch)
		{
			textSelectionOutput = SEARCH_TYPES[optionSelected];

			if (myEngine->KeyHit(NEXT_SELECT)) //The last available option is the upper limit.
			{
				optionSelected++;
				optionSelected = optionSelected % ESearchType::NumOfSearches;
			}
			if (myEngine->KeyHit(PREV_SELECT)) //The current option is set to the last available option it goes below 0.
			{
				optionSelected--;
				if (optionSelected < 0)
				{
					optionSelected = ESearchType::NumOfSearches - 1;
				}
			}
			if (myEngine->KeyHit(SELECT)) //The current option is checked against the options Enum and performs the appropriate action.
			{
				map->SetSearchSelection(optionSelected);
				if (pathFinder != nullptr)
				{
					delete(pathFinder);
				}
				pathFinder = NewSearch(map->GetSearchSelection()); //Get the pathfinding object.
				state = EGameState::Setup; //Set back to setup.
				optionSelected = 0; //Reset to the first option after an option has been selected.
			}
		}

		//Use WASD to move the start/end node graphic around the map and enter to place it.
		else if (state == EGameState::ChoosingStart || state == EGameState::ChoosingEnd)
		{
			textSelectionOutput = NODE_SELECTION_INFO;

			//As long as the x position isn't 0, reduce it by 1 when the control is pressed.
			if (myEngine->KeyHit(NODE_SELECT_X_NEG))
			{
				if (nodeSelectionPos.x > 0)
				{
					nodeSelectionPos.x--;
					nodeSelectionModel->SetX(nodeSelectionPos.x);
				}
			}
			//As long as the x position is less than the horizontal limit of the map, increase it by 1 when the control is pressed.
			if (myEngine->KeyHit(NODE_SELECT_X_POS))
			{
				if (nodeSelectionPos.x < (map->GetDimensions().x - 1))
				{
					nodeSelectionPos.x++;
					nodeSelectionModel->SetX(nodeSelectionPos.x);
				}
			}
			//As long as the z position isn't 0, reduce it by 1 when the control is pressed.
			if (myEngine->KeyHit(NODE_SELECT_Z_NEG))
			{
				if (nodeSelectionPos.y != 0)
				{
					nodeSelectionPos.y--;
					nodeSelectionModel->SetZ(nodeSelectionPos.y);
				}
			}
			//As long as the z position is less than the vertical limit of the map, increase it by 1 when the control is pressed.
			if (myEngine->KeyHit(NODE_SELECT_Z_POS))
			{
				if (nodeSelectionPos.y < (map->GetDimensions().y - 1))
				{
					nodeSelectionPos.y++;
					nodeSelectionModel->SetZ(nodeSelectionPos.y);
				}
			}
			if (myEngine->KeyHit(SELECT)) //Set the selected grid space to the chosen option (Start/End)
			{
				if (state == EGameState::ChoosingStart)
				{
					map->SetStartNode(nodeSelectionPos);
				}
				else
				{
					map->SetEndNode(nodeSelectionPos);
				}
				state = EGameState::Setup;
				nodeSelectionModel->SetY(SPAWN_Y);
			}
		}
		
		/***** FRAMETIME SHOULD ONLY BE USED AFTER THIS POINT *****/

		//Camera Controls
		if (myEngine->KeyHeld(CAM_ZOOM_OUT))
		{
			map->ZoomCamOut(frameTime);
		}
		if (myEngine->KeyHeld(CAM_ZOOM_IN))
		{
			map->ZoomCamIn(frameTime);
		}
		
		//Close the program
		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}
	}

	delete(pathFinder);
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}