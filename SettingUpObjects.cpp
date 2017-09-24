// SBAJONES 2066906 Samuel Jones
#include "SettingUpObjects.h"
#include "Vector3D.h"

SettingUpObjects::SettingUpObjects()
{
}


SettingUpObjects::~SettingUpObjects()
{
}

// The array will be assigned the different values to provide different positions for each water tank
void SettingUpObjects::SetupTanks(Vector3D TankLocations[])
{
	TankLocations[0].SetX(-10.5f);
	TankLocations[0].SetY (0.0f);
	TankLocations[0].SetZ (220.0f);

	TankLocations[1].SetX(20.5f);
	TankLocations[1].SetY (0.0f);
	TankLocations[1].SetZ (290.0f);

	TankLocations[2].SetX(0.0f);
	TankLocations[2].SetY (-1.0f);
	TankLocations[2].SetZ(-40.0f);

	TankLocations[3].SetX(290.5f);
	TankLocations[3].SetY(0.0f);
	TankLocations[3].SetZ(290.0f);

	TankLocations[4].SetX(310.5);
	TankLocations[4].SetY(0.0f);
	TankLocations[4].SetZ(220.0f);

	TankLocations[5].SetX(250.5);
	TankLocations[5].SetY(0.0f);
	TankLocations[5].SetZ(250.0f);

	TankLocations[6].SetX(220.0);
	TankLocations[6].SetY(0.0f);
	TankLocations[6].SetZ(280.0f);
}

// The procedure will return an array that has the 3D points for the spawn location of the walls & isles
void SettingUpObjects::SetupWalls(Vector3D wallVectors[])
{
	wallVectors[0].SetX(-10.5f);
	wallVectors[0].SetY (0.0f);
	wallVectors[0].SetZ (46.0f);

	wallVectors[1].SetX(10.5f);
	wallVectors[1].SetY (0.0f);
	wallVectors[1].SetZ (46.0f);

	wallVectors[2].SetX(105.0f);
	wallVectors[2].SetY (0.0f);
	wallVectors[2].SetZ (285.0f);

	wallVectors[3].SetX(105.0f);
	wallVectors[3].SetY (0.0f);
	wallVectors[3].SetZ (315.0f);

	wallVectors[4].SetX(140.0f);
	wallVectors[4].SetY (0.0f);
	wallVectors[4].SetZ (285.0f);

	wallVectors[5].SetX(140.0f);
	wallVectors[5].SetY (0.0f);
	wallVectors[5].SetZ (315.0f);

	wallVectors[6].SetX(310.0f);
	wallVectors[6].SetY(0.0f);
	wallVectors[6].SetZ(90.0f);

	wallVectors[7].SetX(290.0f);
	wallVectors[7].SetY(0.0f);
	wallVectors[7].SetZ(90.0f);

	wallVectors[8].SetX(307.5f);
	wallVectors[8].SetY(0.0f);
	wallVectors[8].SetZ(70.0f);

	wallVectors[9].SetX(292.50f);
	wallVectors[9].SetY(0.0f);
	wallVectors[9].SetZ(70.0f);

	wallVectors[10].SetX(307.5f);
	wallVectors[10].SetY(0.0f);
	wallVectors[10].SetZ(50.0f);

	wallVectors[11].SetX(40.0f);
	wallVectors[11].SetY(0.0f);
	wallVectors[11].SetZ(-220.0f);

	wallVectors[12].SetX(60.0f);
	wallVectors[12].SetY(0.0f);
	wallVectors[12].SetZ(-220.0f);

	wallVectors[13].SetX(80.0f);
	wallVectors[13].SetY(0.0f);
	wallVectors[13].SetZ(-220.0f);

	wallVectors[14].SetX(100.0f);
	wallVectors[14].SetY(0.0f);
	wallVectors[14].SetZ(-220.0f);

	wallVectors[15].SetX(120.0f);
	wallVectors[15].SetY(0.0f);
	wallVectors[15].SetZ(-220.0f);

	wallVectors[16].SetX(160.0f);
	wallVectors[16].SetY(0.0f);
	wallVectors[16].SetZ(-220.0f);

	wallVectors[17].SetX(180.0f);
	wallVectors[17].SetY(0.0f);
	wallVectors[17].SetZ(-220.0f);

	wallVectors[18].SetX(200.0f);
	wallVectors[18].SetY(0.0f);
	wallVectors[18].SetZ(-220.0f);

	wallVectors[19].SetX(220.0f);
	wallVectors[19].SetY(0.0f);
	wallVectors[19].SetZ(-220.0f);

	wallVectors[20].SetX(240.0f);
	wallVectors[20].SetY(0.0f);
	wallVectors[20].SetZ(-220.0f);
}

// Assigning the vector positions of the spawn points for the checkpoints
void SettingUpObjects::SetupCheckpoints(Vector3D checkpointVectors[])
{
	// Declaring the variables that will be used for float positions
	const float topTurnZ = 300.0f;
	const float topEndZ = 100.0f;
	const float bottomEndZ = 0.0f;
	const float bottomTurnZ = -200.0f;
	const float turnX = 125.0f;
	const float beginningStraightX = 0.0f;
	const float secondStraightX = 300.0f;

	checkpointVectors[0].SetX(0.0f);
	checkpointVectors[0].SetY(0.0f);
	checkpointVectors[0].SetZ(0.0f);

	checkpointVectors[1].SetX(0.0f);
	checkpointVectors[1].SetY(0.0f);
	checkpointVectors[1].SetZ(topEndZ);

	checkpointVectors[2].SetX(turnX);
	checkpointVectors[2].SetY(0.0f);
	checkpointVectors[2].SetZ(topTurnZ);

	checkpointVectors[3].SetX(secondStraightX);
	checkpointVectors[3].SetY(0.0f);
	checkpointVectors[3].SetZ(topEndZ);

	checkpointVectors[4].SetX(secondStraightX);
	checkpointVectors[4].SetY(0.0f);
	checkpointVectors[4].SetZ(0.0f);

	checkpointVectors[5].SetX(turnX);
	checkpointVectors[5].SetY (0.0f);
	checkpointVectors[5].SetZ (bottomTurnZ);

}
