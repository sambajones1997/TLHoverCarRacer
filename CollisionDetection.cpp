// SBAJONES 2066906 Samuel Jones
#include "CollisionDetection.h"
#include <TL-Engine.h>

using namespace tle;

const float playerRad = 2.5f;

CollisionDetection::CollisionDetection()
{
	
}


CollisionDetection::~CollisionDetection()
{
}

// This checks to see if they have collided through the checkpoint and passed it.
// The array points are 0 & 1 are left and right checkpoint X respectively
// Array position 2 & 3 are the back and front of the checkpoint respectively
bool CollisionDetection::Checkpoint(IModel* car, IModel* checkPoint)
// pPoints are the player points in a radius
// cPoints are the checkpoint collision box 
{
	bool collision = false;
	const float checkRad = 6.0f;
	float x = (car->GetX()) - (checkPoint->GetX());
	float z = (car->GetZ()) - (checkPoint->GetZ());
	float distance = sqrtf((x*x) + (z*z));

	if (distance < (checkRad + playerRad) && distance > (checkRad + playerRad - 1.0f))
	{
		collision = true; // The car has collided with the checkpoint and has been passed
	}
	return collision; // The state of collision is returned
}

// A circle - circle collision style is used as the player can't move in the y-axis,
// The procedure checks for a collision on either side of the checkpoint and returns a boolean statement for collision
bool CollisionDetection::CheckpointTyres(IModel * playerCar, IModel * checkpoints[], int kNoCheck)
{
	// Variables to be used locally are delcared
	const float offset = 8.0f; // The offeset between the origin point of the checkpoint and the tyres
	bool isCollided = false; // Whether the tyres and player are colliding
	float px = playerCar->GetX(); 
	float pz = playerCar->GetZ();
	float tx = 0.0f;
	float tz = 0.0f;
	float X;
	float Z;
	float distance;
	float tyreRad = 0.1f;

	for (int i = 0; i < kNoCheck; i++)
	{
		if (i != 5 && i != 2)
		{
			// Checking the first tyre for collision
			tx = checkpoints[i]->GetX() - offset;
			tz = checkpoints[i]->GetZ();
			X = tx - px;
			Z = tz - pz;
			distance = sqrtf ((X*X) + (Z*Z));
			if (distance < (playerRad + tyreRad))
			{
				isCollided = true;
				break; // The car collided with a tyre and so cant collide with any others
			}
			else
			{
				// The second tyre is only checked if the first one wasn't collided with to stop unnecessary logic 
				tx = checkpoints[i]->GetX() + offset;
				X = tx - px;
				distance = sqrtf((X*X) + (Z*Z));
				if (distance < (playerRad + tyreRad))
				{
					isCollided = true;
				}
			}
		}
		else
			// The same process is done here but instead the z location is checked for collisions because the checkpoints
			// Are rotated so the tyres are now in the min and max z locations rather than the x locations
		{
			tx = checkpoints[i]->GetX();
			tz = checkpoints[i]->GetZ() - offset;
			X = tx - px;
			Z = tz - pz;
			distance = sqrtf((X*X) + (Z*Z));
			if (distance < (playerRad + tyreRad))
			{
				isCollided = true;
			}
			else
			{
				tz = checkpoints[i]->GetZ() + offset;
				Z = tz - pz;
				distance = sqrtf((X*X) + (Z*Z));
				if (distance < (playerRad + tyreRad))
				{
					isCollided = true;
				}
			}
		}
	}
	return isCollided;
}

void CollisionDetection::GetPlayerRadius(float playerX, float playerZ, float playerCollision[])
{
	const float playerRadius = 3.0f;
	playerCollision[0] = playerX - playerRadius;
	playerCollision[1] = playerX + playerRadius;
	playerCollision[2] = playerZ + playerRadius;
	playerCollision[3] = playerZ + playerRadius;

}


// Checks to see if the player has reached the edge of the map and bounces them backwards into the map if they try to leave
bool CollisionDetection::MapRadius(IModel * playerCar)
{
	bool hasCollided = false;
	const float mapMaxZ = 350.0f;
	const float mapMinZ = -350.0f;
	const float mapMinX = -30.0f;
	const float mapMaxX = 350.0f;
	float x = playerCar->GetX();
	float z = playerCar->GetZ();

	 // Checks to see whether the player is outside any of the collision boundaries
	if (x - playerRad <= mapMinX || x + playerRad >= mapMaxX || z + playerRad >= mapMaxZ || z - playerRad <= mapMinZ)
	{
		hasCollided = true;
	}

	return hasCollided;
}
