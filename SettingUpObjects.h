// SBAJONES 2066906 Samuel Jones
#pragma once
#include "Vector3D.h"

class SettingUpObjects
{
public:
	SettingUpObjects();
	~SettingUpObjects();
	void SetupWalls(Vector3D WallLocations[]);
	void SetupTanks(Vector3D TankLocations[]);
	void SetupCheckpoints(Vector3D CheckpointVectors[]);
};

