// SBAJONES 2066906 Samuel Jones
#pragma once
#include <TL-Engine.h>
using namespace tle;
class CollisionDetection
{
public:
	CollisionDetection();
	~CollisionDetection();
	bool Checkpoint(IModel* car, IModel* checkPoint);
	bool CheckpointTyres(IModel* playerCar, IModel* checkpoints[], int kNoCheck);
	void GetPlayerRadius(float playerX, float playerZ, float playerCollision[]);
	bool MapRadius(IModel* playerCar);
private:

};

