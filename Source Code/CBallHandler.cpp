#include "DisplayClasses.h"

CBallHandler::CBallHandler(IMesh* mesh)
{
	mpBody = mesh->CreateModel(0.0f, SPAWN_Y, 0.0f);
	mpBody->GetMatrix(&mMatrix.e00);
}

/** These functions allow the program to interact with the private member variables. **/
// Set the ball's model matrix to the member matrix.
void CBallHandler::SetModelMatrix()
{
	mpBody->SetMatrix(&mMatrix.e00);
}

// Takes the generated path and a reference to the map and converts it into coordinates.
void CBallHandler::SetPath(NodeList &searchResult, CMapHandler* map)
{
	// Empty the path first.
	mPath.clear();

	// Apply the spline to the search result to build a path.
	// Push the start node onto the path, then apply the spline.
	// Apply the spline to the first 3 nodes using the points (0,0,1,2) and push each generated position except the first one onto the path.
	// (This is because this position is very close to the spawn position due to the use of 2 0 positions.)
	// Apply the spline to every 3 nodes after, using the end of the previous generated spline section as the first position.
	// This allows a smooth spline to be generated.
	mPath.push_back(map->GetNodePosition(searchResult[0]->x, searchResult[0]->y));

	if (searchResult.size() > 2) //If it's only 2 spaces, it can never need a spline.
	{
		CVector3 splinePos1;
		CVector3 splinePos2;
		CVector3 splinePos3;

		//Calculate the spline for the first 3 nodes, but don't use the first position generated.
		//This is because this position is very close to the spawn position, so it can be ignored.
		BezierSpline(map->GetNodePosition(searchResult[0]->x, searchResult[0]->y),
			map->GetNodePosition(searchResult[0]->x, searchResult[0]->y),
			map->GetNodePosition(searchResult[1]->x, searchResult[1]->y),
			map->GetNodePosition(searchResult[2]->x, searchResult[2]->y),
			splinePos1, splinePos2, splinePos3);
		mPath.push_back(splinePos2);
		mPath.push_back(splinePos3);

		//Each spline uses the end of the path as it currently exists as it's starting anchor, creating a smoother path.
		//After this, the previous spline mapped to the end node of that spline (For the first loop, this would be 2)
		//and so the 2nd node of the spline needs to this same node, to continue the path.
		//After this point, it uses the two nodes after the above node. If there aren't 2 nodes available, the loop is exited
		//because there is either only 1 node left to spline past to reach the goal, or the goal has been reached.
		int splineEndTracker;
		for (splineEndTracker = 2; //SplineEndTracker tracks which index the spline is currently ending on.
			splineEndTracker + 2 < searchResult.size(); //Ensures it won't attempt to spline without enough nodes.
			splineEndTracker += 2) //3 nodes at a time, with 1 node of overlap, per spline.
		{
			BezierSpline(mPath.back(), //Use the end of the last generated section of the spline as the anchor for the next section of the spline.
				map->GetNodePosition(searchResult[splineEndTracker]->x, searchResult[splineEndTracker]->y),
				map->GetNodePosition(searchResult[splineEndTracker + 1]->x, searchResult[splineEndTracker + 1]->y),
				map->GetNodePosition(searchResult[splineEndTracker + 2]->x, searchResult[splineEndTracker + 2]->y),
				splinePos1, splinePos2, splinePos3);

			mPath.push_back(splinePos1);
			mPath.push_back(splinePos2);
			mPath.push_back(splinePos3);
		}

		//The size of the array searchResult is 1 larger than the index of the end of the array. Because splineEndTracker
		//tracks indexes, we need to take this into account. Additionally, this statement is to check that the goal has not
		//already been splined to; If it hasn't, set a spline to it.
		if (splineEndTracker + 2 == searchResult.size())
		{
			BezierSpline(mPath.back(), //Use the end of the last generated section of the spline as the anchor for the next section of the spline.
				map->GetNodePosition(searchResult[splineEndTracker]->x, searchResult[splineEndTracker]->y),
				map->GetNodePosition(searchResult[splineEndTracker + 1]->x, searchResult[splineEndTracker + 1]->y),
				map->GetNodePosition(searchResult[splineEndTracker + 1]->x, searchResult[splineEndTracker + 1]->y),
				splinePos1, splinePos2, splinePos3);

			mPath.push_back(splinePos1);
			mPath.push_back(splinePos2);
			mPath.push_back(splinePos3);
		}

	}

	mPath.push_back(map->GetNodePosition(searchResult.back()->x, searchResult.back()->y));

	SetupPathOverlay(searchResult, map);
}


void CBallHandler::SetupPathOverlay(NodeList &searchResult, CMapHandler* map)
{
	//Reset the rotation of existing models.
	for (int count = 0; count < mPathOverlay.size(); count++)
	{
		mPathOverlay[count]->ResetOrientation();
	}

	//If there are not enough overlays available, resize to make more.
	//EG, with 7 nodes on the path, need 6 overlays.
	if (mPathOverlay.size() < (searchResult.size() - 1))
	{
		int oldSize = mPathOverlay.size();
		mPathOverlay.resize(searchResult.size() - 1);

		//Create models and set skins for new models.
		for (int count = oldSize; count < mPathOverlay.size(); count++)
		{
			mPathOverlay[count] = map->BuildFromGridspaceMesh({ 0.0f, SPAWN_Y, 0.0f });
			mPathOverlay[count]->SetSkin(PATH_TEXTURE);
		}
	}

	CVector3 position;

	//Place an overlay between nodes on the path.
	//Set the texture of the overlay according to whether it represents a node on the closed list or open list.
	for (int count = 0; count < searchResult.size() - 1; count++)
	{
		position = { float(searchResult[count + 1]->x - searchResult[count]->x), //Difference in X on the grid.
					0,
					float(searchResult[count + 1]->y - searchResult[count]->y) }; //Difference in Y on the grid.
		position = position * 0.5f; //Half of the difference is required to place it between nodes.

		//Set the rotation of the models to point the arrow in the right direction.
		if (position.x > 0) mPathOverlay[count]->RotateY(RIGHT_ANGLE * ECompass::East);
		else if (position.x < 0) mPathOverlay[count]->RotateY(RIGHT_ANGLE * ECompass::West);
		else if (position.z < 0) mPathOverlay[count]->RotateY(RIGHT_ANGLE * ECompass::South);

		position += map->GetNodePosition(searchResult[count]->x, searchResult[count]->y); //Add on the position of the first node to get the position of the overlay. 

		mPathOverlay[count]->SetPosition(position.x, position.y + (OVERLAY_Y * 2.0f), position.z);
	}
}

//If the first overlay isn't at the spawn position, set all overlays to the spawn position.
void CBallHandler::HidePathDemo()
{
	if (mPathOverlay.size() > 0)
	{
		if (mPathOverlay[0]->GetY() != SPAWN_Y)
		{
			for (auto it = mPathOverlay.begin(); it != mPathOverlay.end(); it++)
			{
				(*it)->SetY(SPAWN_Y);
			}
		}
	}
}

//Performs a Bezier spline using 0.2 increments.
void CBallHandler::BezierSpline(CVector3 p1, CVector3 p2, CVector3 p3, CVector3 p4, //Indexes of the points on the path to use for the calculation.
	CVector3 &s1, CVector3 &s2, CVector3 &s3) //The coordinates of the points on the curve.
{
	s1 = (p1 * pow((1 - 0.2), 3.0)) + (p2 * (3 * 0.2) * pow((1 - 0.2), 2.0)) +
		(p3 * 3 * pow(0.2, 2.0) * (1 - 0.2)) + (p4 * pow(0.2, 3.0));
	s2 = (p1 * pow((1 - 0.4), 3.0)) + (p2 * (3 * 0.4) * pow((1 - 0.4), 2.0)) +
		(p3 * 3 * pow(0.4, 2.0) * (1 - 0.4)) + (p4 * pow(0.4, 3.0));
	s3 = (p1 * pow((1 - 0.6), 3.0)) + (p2 * (3 * 0.6) * pow((1 - 0.6), 2.0)) +
		(p3 * 3 * pow(0.6, 2.0) * (1 - 0.6)) + (p4 * pow(0.6, 3.0));
}

//Sets the Y coordinate to below the mBackground to hide it.
void CBallHandler::HideBall()
{
	mMatrix.e31 = SPAWN_Y;
}

//Place the ball at the Startnode and point it towards the north.
void CBallHandler::SpawnBall()
{
	mState = EBallState::Spawning;
	mPosition = mPath[0];
	mPosition.y = MAP_LEVEL_Y - 0.2f;
	PointAtNode(mPath[1]);
}

//Rotate the facing vector towards the next position to move towards.
void CBallHandler::PointAtNode(CVector3 modTarget) //Points towards the chosen node.
{
	modTarget.y = mPosition.y;
	mRollDirection = Normalise(modTarget - mPosition);

	// Use cross products to get other axes
	// Must normalise axes
	CVector3 vecX = Normalise(CrossProduct(kYAxis, mRollDirection));
	CVector3 vecY = Normalise(CrossProduct(mRollDirection, vecX));

	// Build matrix from axes + position
	// Matrix constructor using four CVector3 variables
	// - one for each row/column
	// (see matrix header)
	// Build matrix by row (axes + position)
	mMatrix.SetRow(0, vecX * 0.1f);
	mMatrix.SetRow(1, vecY * 0.1f);
	mMatrix.SetRow(2, mRollDirection * 0.1f);
	mMatrix.SetRow(3, mPosition);
}

//Raise the ball up through the grid into view, then roll the ball across the path, then sink the ball through the grid when it reaches the goal.
void CBallHandler::MoveBall(const float &frameTime)
{
	if (mState == EBallState::Spawning) //Shift the ball up along the Y axis while it is spawning.
	{
		mPosition.y += frameTime;
		mMatrix.SetRow(3, mPosition);

		if (mPosition.y >= MAP_LEVEL_Y + 0.2f)
		{
			mCycler = 1;
			mState = EBallState::Rolling;
		}
	}
	else if (mState == EBallState::Rolling)
	{
		//move forward
		mPosition += CVector3{ mMatrix.e20, mMatrix.e21, mMatrix.e22 } *BALL_SPEED * frameTime;
		mMatrix.SetRow(3, mPosition);

		//check for collision
		float collisionDist = PATH_COLLISION_DIST;

		//If within the area of the node, set target to next node.
		if (XZLength(mPath[mCycler] - mPosition) < collisionDist)
		{
			mCycler++;
			if (mCycler != mPath.size()) //If the last node has not been reached, point to the next node.
			{
				PointAtNode(mPath[mCycler]);
			}
			else
			{
				mCycler = 1;
				mState = EBallState::Ending;
			}
		}
	}
	else if (mState == EBallState::Ending)
	{
		mPosition -= CVector3{ mMatrix.e10, mMatrix.e11, mMatrix.e12 } *frameTime;
		mMatrix.SetRow(3, mPosition);

		if (mPosition.y <= MAP_LEVEL_Y - 0.2f)
		{
			SpawnBall();
		}
	}
}

//Length for X and Z only. 2D movement.
float CBallHandler::XZLength(const CVector3& v)
{
	float lengthSq = v.x * v.x + v.z * v.z;

	// Ensure vector is not zero length (use BaseMath.h float approx. fn with default epsilon)
	if (IsZero(lengthSq))
	{
		return 0.0f;
	}
	else
	{
		return sqrt(lengthSq);
	}
}