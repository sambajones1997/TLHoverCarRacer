// SBAJONES 2066906 Samuel Jones
// Assignment3.cpp: A program using the TL-Engine
// Samuel Jones 20663906
// HOVER CAR RACING GAMES CONCEPTS ASSIGNMENT 3

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <string>
#include "CollisionDetection.h"
#include "Vector3D.h"
#include "SettingUpObjects.h"

using namespace tle;
using namespace std;

// Creating the enumerations to be used through the game
enum GameState { Start, Racing, Over, Paused }; // Stage 1 is Racing, Stage 2 is Over
enum PlayerState { Accelerating, Reversing, Neutral, Boosting,Dead };
enum CameraState { Chase, FPS };
enum carBob { Down, Up };
enum checkpointState {active, passed};

struct CheckPoint {
	IModel* checkpointM;
	checkpointState checkState;
	float rotation;
};

struct Isle
{
	IModel* isleB;
	IModel* isleT;
	IModel* wall;
	float maxX;
	float minX;
	float maxZ;
	float minZ;
	float rot;
};

struct HoverCar
{
	IModel* base;
	float fwdThrust = 0.0f;
	float timeSinceDMG = 0.0f;
	float matrix[4][4];
	float bestLap = 0.0f;
	bool bNewRecord = false;
	int health;
	carBob cb = Down;
	Vector3D facingVector = Vector3D(0.0f, 0.0f, 0.0f);
	Vector3D momentumVector = Vector3D(0.0f,0.0f,0.0f);
};

// --- FORWARD DECLARING THE PROCEDURES AND FUNCTIONS --- //
bool CheckpointCollision(IModel* playerCar, CheckPoint checkpoints[], IModel* crosses[], IMesh* cross);
bool isleWallCollision(IModel* playerCar, Isle wallIsles[], HoverCar &playerHC);
void UI(IFont* font, int height, int width, float playerSpeed, float boostTime, bool boostActive, int health);
void RestartGame(HoverCar &playerHC, CheckPoint checkpoints[], Vector3D playerSpawn, float &countdown, float &camX, float &camY);
void ProvideForwardThrust(IModel* base, float frameTime, float thrust);
bool wtCollision(IModel* wTank[], HoverCar &playerHC, float time);
void HoverCarMove(HoverCar &hc, float frameTime);
void ClearCheckPoints(IModel* crosses[],IMesh* cross);
void CrossCheckPoint(IModel* crosses[], IMesh* crossMesh, IModel* checkpoint);
void CheckCollisionTyres(HoverCar &hc, IModel* checkpoints[], float frameTime);
void PausedUI(IFont* uiFont, int width, int height);
void CheckMapBoundary(HoverCar &hc, float frameTime);
void TakeDamage(HoverCar &hc);
void CheckCameraControls(EKeyCode camFwd, EKeyCode camBwd, EKeyCode camLeft, EKeyCode camRight, IModel* cameraMount, float kCameraMoveSpeed);
void NewLapRecordUI(IFont* uiFont, int width, int height);
Vector3D GetForwardVector(IModel* base);
Vector3D GetMomentum(Vector3D facingVector, float thrust, float frameTime);
Vector3D carBobVector(HoverCar &hc, Vector3D momentumvector, float frameTime);


// Creating global constants
I3DEngine* myEngine = New3DEngine(kTLX);
const int KNOCHECK = 6;
const int KGAMESPEED = 1;
const int KREFRESHRATE = 60;
const int KNOWALLS = 21; // The number of wall isles that will be spawned in the world
const int KNOISLES = 4;
const int KNOLAPS = 4; // The number of laps that must be completed in order to finish the race
const int kNoWT = 7; // The number of water tanks in the world
const float KZERO = 0.0f;
const float kDmgTime = 0.1f;

int checkpointNo = 0;
int lapNo = 0;
bool bNewLapRecord = false;
float frameTime = 0.0f;
float lapTime = 0.0f;
float bestLap = 0.0f;

GameState gameState = Start;
PlayerState playerState = Neutral;
CameraState camState = Chase;

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	myEngine->StartWindowed();
	myEngine->StartMouseCapture();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\ProgramData\\TL-Engine\\Media" );
	myEngine->AddMediaFolder(".\\Media");

	// Creating the constant spawn vector variables
	const Vector3D cameraLoc( 0.0f, 20.0f, -40.0f );
	const Vector3D fpsCamLoc{0.0f, 5.0f,2.0f};
	const Vector3D skyboxLoc{ 0.0f,-960.0f,0.0f };
	const Vector3D playerStartLoc{ 0.0f,0.0f,-20.0f };
	const Vector3D camMountLoc{ 0.0f, 0.0f, 0.0f };

	// Creating the variables to store the key controls
	const EKeyCode accFwd = EKeyCode(Key_W);
	const EKeyCode accRear = EKeyCode(Key_S);
	const EKeyCode turnLeft = EKeyCode(Key_A);
	const EKeyCode turnRight = EKeyCode(Key_D);
	const EKeyCode boost = EKeyCode(Key_Space);
	const EKeyCode startButton = EKeyCode(Key_Space);
	const EKeyCode camFwd = EKeyCode(Key_Up);
	const EKeyCode camBwd = EKeyCode(Key_Down);
	const EKeyCode camLeft = EKeyCode(Key_Left);
	const EKeyCode camRight = EKeyCode(Key_Right);
	const EKeyCode camReset = EKeyCode(Key_1);
	const EKeyCode camFPS = EKeyCode(Key_2);
	const EKeyCode quit = EKeyCode(Key_Escape);
	const EKeyCode pause = EKeyCode(Key_P);

	const int kNoWRot = 14; // The number of walls that will be rotated
	const int rotWalls[kNoWRot]{ 2,3,4,5,11,12,13,14,15,16,17,18,19,20}; // The index locations for which walls should be rotated
	const int KHeight = myEngine->GetHeight();
	const int KWidth = myEngine->GetWidth();
	const int timeToTop = 2; // The length of time to go from zero to top speed
	const int timeToDecelerate = 5; // The length of time to go from top speed to zero
	const float kMaxBoostTime = 3.0f;
	const float kBoostCooldown = 5.0f;
	const float kTopSpeed = 75.0f; // The top speed of the vehicle
	const float kTopReverseSpeed = -50.0f; // The top speed of the vehicle when reversing
	const float kTopBoostSpeed = 100.0f; // The top speed of the vehicle when using boost
	const float kCheckRot = 90.0f; // The rotation value of rotated checkpoints
	const float kCheckNoRot = 0.0f; // The rotation value of non-rotated checkpoints
	const float kCameraMoveSpeed = 10.0f; // The number of units to move the camera per second
	const float cameraRot = 10.0f; // The amount to rotate the camera by in the x direction upon spawn
	const float speedIncrease = (kTopSpeed / timeToTop); // The speed increase per second for the vehicle
	const float speedDecelerate = (kTopSpeed / timeToDecelerate); // The speed to decelerate per second
	const float kScale = 0.5f; // The scale of the vehicles compared to the world
	const float rotInc = 40.0f; // The amount to turn the player's car in degrees when turning
	const float wallIsleLength = 13.0f; // The length from the middle of a wall section the end of the connected isle
	const float wallLength = 7.5f; // The length of the seperate wall
	const float wallIsleWidth = 2.0f; // The width of the wall
	const float mouseSense = 0.5f; // The senstitivity of the mouse movement
	const float kLowestSpeed = 0.01f;
	const float kPlayerRad = 3.0f;
	const float kVSYNC = 1 / 60.0f;

	const string backgroundText = "ui_backdrop.jpg";

	const CollisionDetection collisionDetection;
	SettingUpObjects setUp;

	IFont* CDFont = myEngine->LoadFont("Arial", 50);
	IFont* uiFont = myEngine->LoadFont("Arial", 15);

	// Creating the non constant variables

	Vector3D checkpointVectors[KNOCHECK]; // This will store all of the spawn locations for each checkpoint
	Vector3D wallVectors[KNOWALLS]; // This will store all of the spawn locations for each wall piece
	Vector3D tankVectors[kNoWT];
	CheckPoint checkpointDetails[KNOCHECK]; // The storing of checkpoints
	float updateTime = 0.0f; // The time since the last frame
	float countdown = 4.0f; // The time to count down from to start the race
	float camRotX = 0.0f; // The amount the camera is rotated in the X direction
	float camRotY = 0.0f; // The amount the camera is rotated in the y direction
	float boostTime = kMaxBoostTime; // The player starts with full energy
	float boostCoolDown = 0.0f;
	float newRecordUITimer = 2.0f;
	float vSyncTimer = 1 / 60.0f; // The time between rendering each frame
	bool hasStarted = false;
	bool hasCollided = false;

	setUp.SetupTanks(tankVectors);
	setUp.SetupWalls(wallVectors);
	setUp.SetupCheckpoints(checkpointVectors);

	/**** Set up your scene here ****/
	// Loading the meshes
	IMesh* checkpointMesh = myEngine->LoadMesh("Checkpoint.x");
	IMesh* hoverCarMesh = myEngine->LoadMesh("race2.x");
	IMesh* skyboxMesh = myEngine->LoadMesh("Skybox 07.x");
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IMesh* wallMesh = myEngine->LoadMesh("Wall.x");
	IMesh* isleMesh = myEngine->LoadMesh("IsleStraight.x");
	IMesh* tankMesh1 = myEngine->LoadMesh("TankSmall1.x");
	IMesh* dummyMesh = myEngine->LoadMesh("dummy.x");
	IMesh* crossMesh = myEngine->LoadMesh("Cross.x");
	// Creating the models in the word
	// Player Character, creating the mesh and assigning it to a struct variable
	IModel* playerCar = hoverCarMesh->CreateModel(playerStartLoc.GetX(),playerStartLoc.GetY(),playerStartLoc.GetZ());
	// Creating the enviromnent models
	IModel* skyBox = skyboxMesh->CreateModel(skyboxLoc.GetX(), skyboxLoc.GetY(), skyboxLoc.GetZ());
	IModel* ground = groundMesh->CreateModel();
	// Creates the checkpoints for the race
	IModel* checkpoints[KNOCHECK];
	IModel* AIWaypoints[25];
	IModel* walls[KNOWALLS];
	IModel* isles[KNOISLES];
	IModel* waterTanks[kNoWT];
	IModel* crosses[KNOCHECK];
	// Creates a mount for the camera that will be attached to the player car
	IModel* cameraMount = dummyMesh->CreateModel();
	cameraMount->AttachToParent(playerCar);
	Isle wallIsles[KNOWALLS];
	ISprite* uiSprite = myEngine->CreateSprite(backgroundText);

	// Assigns the playerHC to hovercar struct
	HoverCar playerHC;
	//playerHC.base = dummyMesh->CreateModel(playerStartLoc.GetX(), playerStartLoc.GetY(), playerStartLoc.GetZ());
	playerHC.base = playerCar;
	//playerHC.hoverCar->AttachToParent(playerHC.base);
	playerHC.health = 100;
	
	// Spawning the water tanks into the worl
	for (int i = 0; i < kNoWT; i++)
	{
		waterTanks[i] = tankMesh1->CreateModel(tankVectors[i].GetX(),tankVectors[i].GetY(),tankVectors[i].GetZ());
	}

	waterTanks[2]->RotateX(5.0f); // The obstacle water tank is rotated slightly to become more of a threat


	// Spawns a checkpoint into an array so that they can be processed in an array
	// The size of the array is declared by a constant which is equal to the required number of checkpoints
	for (int i = 0; i < KNOCHECK; i++)
	{
		checkpoints[i] = checkpointMesh->CreateModel(checkpointVectors[i].GetX(), checkpointVectors[i].GetY(), checkpointVectors[i].GetZ());
		checkpointDetails[i].checkpointM = checkpoints[i];
		checkpointDetails[i].checkState = active;
		if (i == 2)
		{
			checkpointDetails[i].rotation = kCheckRot;
		}
		else if (i == 5)
		{
			checkpointDetails[i].rotation = -kCheckRot;
		}
		else
		{
			checkpointDetails[i].rotation = kCheckNoRot;
		}
		checkpoints[i]->RotateY(checkpointDetails[i].rotation);
	}

	// The walls and isles will be spawned together as a group so that collision detection and object referencing
	// Can be done together, to do this, a wall and two isles are placed into a struct so that they are able to be
	// Clumped together. The collision boundaries will also be a variable of the struct to reference during collision
	for (int i = 0; i < KNOWALLS; i++)
	{
		bool rot = false;
		// Isle B is the bottom isle and Isle T is the isle after the wall (the top isle)
		// This sorting means that the setup isles must be spawned in the correct order
		
		wallIsles[i].wall = wallMesh->CreateModel(wallVectors[i].GetX(), wallVectors[i].GetY(), wallVectors[i].GetZ());

		for (int rotWall = 0; rotWall < kNoWRot; rotWall++)
		{
 			if (i == rotWalls[rotWall])
			{
				rot = true;
				wallIsles[i].rot = kCheckRot; // The walls will be rotated by 90 degrees to match the rotated checkpoint
				wallIsles[i].wall->RotateY(kCheckRot);
				wallIsles[i].isleB = isleMesh->CreateModel((wallVectors[i].GetX() - wallLength),0.0f, wallVectors[i].GetZ());
				wallIsles[i].isleT = isleMesh->CreateModel((wallVectors[i].GetX() + wallLength), 0.0f, wallVectors[i].GetZ());
			}
			if (rot == true) break; // The wall has been found in the index so the for loop is exited because it no longer needs to be searched for

		}

		// If the wall's index was not in the rotation array then it is not rotated and the wall isles must be spawned at the end of the walls in the z-axis
		if (!rot)
		{
			wallIsles[i].rot = kCheckNoRot; // The walls will not be rotated but the rotation will still be stored
			wallIsles[i].isleB = isleMesh->CreateModel(wallVectors[i].GetX(), 0.0f, (wallVectors[i].GetZ() - wallLength));
			wallIsles[i].isleT = isleMesh->CreateModel(wallVectors[i].GetX(), 0.0f, (wallVectors[i].GetZ() + wallLength));
		}

		// Checks to see if the walls are rotated or not 
		if (wallIsles[i].rot == kCheckRot)
		{
			wallIsles[i].maxX = wallIsles[i].wall->GetX() + wallIsleLength; // The 
			wallIsles[i].minX = wallIsles[i].wall->GetX() - wallIsleLength;
			wallIsles[i].maxZ = wallIsles[i].wall->GetZ() + wallIsleWidth;
			wallIsles[i].minZ = wallIsles[i].wall->GetZ() - wallIsleWidth;
		}
		else
		{
			wallIsles[i].maxX = wallIsles[i].wall->GetX() + wallIsleWidth;
			wallIsles[i].minX = wallIsles[i].wall->GetX() - wallIsleWidth;
			wallIsles[i].maxZ = wallIsles[i].wall->GetZ() + wallIsleLength;
			wallIsles[i].minZ = wallIsles[i].wall->GetZ() - wallIsleLength;
		}
	}


	ICamera* myCamera = myEngine->CreateCamera(kManual,cameraLoc.GetX(),cameraLoc.GetY(),cameraLoc.GetZ());
	myCamera->AttachToParent(cameraMount);
	myCamera->RotateX(cameraRot);


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		/**** Update your scene each frame here ****/
		frameTime = myEngine->Timer(); // Gets the time since the last frame and stores it as a float
		vSyncTimer += frameTime; // The timers still need to reduce or add time between frames to ensure they work correctly
		lapTime += frameTime;
		if (bNewLapRecord) newRecordUITimer -= frameTime;
		if (vSyncTimer > kVSYNC)
		{
			// Draw the scene
			myEngine->DrawScene();
			vSyncTimer = 0.0f;
			// Timer has ended so UI no longer needs showing, return boolean value to false and reset the UI timer
			if (newRecordUITimer <= 0.0f)
			{
				bNewLapRecord = false;
				newRecordUITimer = 2.0f;
			}
			if (gameState == Racing) // Checks to see if the game is in racing mode
			{
				playerState = Neutral; // The player's state is reset every frame
				float rotFrame = rotInc * frameTime;
				playerHC.timeSinceDMG += frameTime;

				playerHC.base->GetMatrix(&playerHC.matrix[0][0]);

				if (myEngine->KeyHeld(accFwd)) // Accelerates the car if the accelerate forward key is held
				{
					playerState = Accelerating; // The car is accelerating forwards so the state is changed to match
					if (playerHC.fwdThrust < kTopSpeed) // Only increases the speed if the car is not already at the top speed
					{
						playerHC.fwdThrust += speedIncrease * frameTime; // Increases the speed by the incremental speed increase
																		 //ProvideForwardThrust(playerHC.hoverCar, frameTime, playerHC.fwdThrust);
					}
				}

				// The player should boost as long as the button is held and the booster doesn't need to cool down

				if (myEngine->KeyHeld(boost) && boostCoolDown <= 0.0f)
				{
					if (boostTime > 0.0f)
					{
						// The amount of time left to boost is reduced and the speed is increased as long as the max hasn't been reached
						boostTime -= frameTime;
						if (playerHC.fwdThrust < kTopBoostSpeed) playerHC.fwdThrust += 2 * speedIncrease * frameTime;
						playerState = Boosting;
					}
					else if (boostTime <= 0.0f)
					{
						boostCoolDown = kBoostCooldown;
					}
				}

				// If the player is boosting then the ui is changed otherwise the amount of boost is increased and cooldown decreased
				if (playerState == Boosting)
				{
					UI(uiFont, KHeight, KWidth, playerHC.fwdThrust, boostTime, true, playerHC.health);
				}
				else
				{
					boostTime += frameTime;
					if (boostTime > kMaxBoostTime) boostTime = kMaxBoostTime;
					if (boostCoolDown > 0.0f) boostCoolDown -= frameTime;
					UI(uiFont, KHeight, KWidth, playerHC.fwdThrust, boostTime, false, playerHC.health);
				}

				if (myEngine->KeyHeld(accRear)) // Decelerates and reverses the car 
				{
					playerState = Reversing;
					if (playerHC.fwdThrust > kTopReverseSpeed)
					{
						playerHC.fwdThrust -= speedDecelerate * frameTime;
					}
				}

				// The player wants to turn right and so uses the bound turn right button
				if (myEngine->KeyHeld(turnRight))
				{
					playerHC.base->RotateY(rotInc * frameTime);
					playerHC.facingVector = GetForwardVector(playerHC.base);
				}
				if (myEngine->KeyHeld(turnLeft))
				{
					playerHC.base->RotateY(-rotInc * frameTime);
					playerHC.facingVector = GetForwardVector(playerHC.base);
				}

				// Natural Deceleration

				if (playerHC.fwdThrust > KZERO) // || playerHC.speed > kTopSpeed && playerState != Boosting)
				{
					playerHC.fwdThrust -= speedDecelerate * frameTime; // Reduces the speed by a set amount since last frame
				}
				else if (playerHC.fwdThrust < kLowestSpeed && playerHC.fwdThrust > kLowestSpeed && playerState != Reversing)
				{
					playerHC.fwdThrust = KZERO; // Set the speed to nothing if it was really low
				}
				else if (playerHC.fwdThrust < KZERO && playerState == Neutral)
				{
					playerHC.fwdThrust += speedDecelerate * frameTime; // Incraeses the speed to closer to 0 when not reversing
				}

				// Moves the car using it's current momentum by getting the direction it is facing
				playerHC.facingVector = GetForwardVector(playerHC.base);
				playerHC.momentumVector = GetMomentum(playerHC.facingVector, playerHC.fwdThrust, frameTime);
				playerHC.momentumVector = carBobVector(playerHC, playerHC.momentumVector, frameTime);
				playerHC.base->Move(playerHC.momentumVector.GetX(), playerHC.momentumVector.GetY(), playerHC.momentumVector.GetZ());

				hasCollided = CheckpointCollision(playerCar, checkpointDetails, crosses, crossMesh); // Checks to see if the player has passed the current checkpoint
				hasCollided = isleWallCollision(playerHC.base, wallIsles, playerHC); // Checks to see if the player has collided with the wall
				hasCollided = wtCollision(waterTanks, playerHC, frameTime);
				CheckCollisionTyres(playerHC, checkpoints, frameTime);
				CheckMapBoundary(playerHC, frameTime);

				if (bNewLapRecord) NewLapRecordUI(CDFont, KWidth, KHeight);

				// Pausing the game during the race
				if (myEngine->KeyHit(pause)) gameState = Paused;

				// Camera Contorols
				if (camState != FPS)
				{
					// The camera mount can be moved about so that the player can set up their own personal camera angle
					CheckCameraControls(camFwd, camBwd, camLeft, camRight, cameraMount, kCameraMoveSpeed);

					camRotX += myEngine->GetMouseMovementY() * mouseSense; // The mouse movement is taken in and then
					camRotY += myEngine->GetMouseMovementX() * mouseSense; // The mouse sensitivity reduces the input
																		   // The rotation of the mount is reset so the camera doesn't 'jiggle' about and can turn upside down etc
					cameraMount->ResetOrientation();
					cameraMount->RotateX(camRotX); // The camerea is rotated in the x axis by the set amount
					cameraMount->RotateY(camRotY); // The camera is rotated in the y axis by the set amount
				}

				if (myEngine->KeyHit(camReset))
				{
					// The camera mount's position is reset so it isn't moved away from the car
					cameraMount->SetLocalPosition(camMountLoc.GetX(), camMountLoc.GetY(), camMountLoc.GetZ());
					// The local position is rest to make sure the camera is set to its default position
					myCamera->SetLocalPosition(cameraLoc.GetX(), cameraLoc.GetY(), cameraLoc.GetZ());
					// The camera has been reset so its state is changed back to chase camera
					camState = Chase;
				}
				// When the player wants to swap to the first person camera, the changes must be reset
				if (myEngine->KeyHit(camFPS))
				{
					cameraMount->ResetOrientation(); // The camera mounts rotation is reset to have no rotation
													 // The location of the mount is reset to be in the centre with the player car model
					cameraMount->SetLocalPosition(camMountLoc.GetX(), camMountLoc.GetY(), camMountLoc.GetZ());
					// The camera's local position is reset to make sure it sits in the same place
					myCamera->SetLocalPosition(fpsCamLoc.GetX(), fpsCamLoc.GetY(), fpsCamLoc.GetZ());
					// The state of the camera is changed to match its viewing angle
					camState = FPS;
				}


			}

			// The game is paused and so nothing in the world will move, a seperate UI is output during the paused phase.
			// The player can quit, continue or restart the game
			else if (gameState == Paused)
			{
				// Calls the UI function
				PausedUI(CDFont, KWidth, KHeight);
				// Checks for player input
				if (myEngine->KeyHit(quit)) myEngine->Stop();
				if (myEngine->KeyHit(pause)) gameState = Racing;
				if (myEngine->KeyHit(startButton))
				{
					if (checkpointNo != KNOCHECK - 1)
					{
						for (int i = 0; i < checkpointNo; i++)
						{
							crossMesh->RemoveModel(crosses[i]);
						}
					}
					else
					{
						for (int i = 1; i < checkpointNo; i++)
						{
							crossMesh->RemoveModel(crosses[i]);
						}
					}

					RestartGame(playerHC, checkpointDetails, playerStartLoc, countdown, camRotX, camRotY);
				}
			}
			// The game is in the start phase and is waiting for player input to begin
			else if (gameState == Start)
			{
				myCamera->AttachToParent(cameraMount);
				const string go = "GO"; // Holds the text that will be used to output the word GO on the screen
				const string start = "Press SPACE to begin";
				const float goCount = 0.99f; // This will be the point at which the text display changes to GO
				if (hasStarted)
				{
					countdown -= 1 * frameTime; // The time since the last frame is deducted from the countdown
					int iTime = (int)countdown; // The time is cast to an integer to output on the screen

					if (countdown <= goCount && countdown >= kLowestSpeed) CDFont->Draw(go, KWidth / 2, KHeight / 2, kWhite, kCentre);
					else if (countdown <= kLowestSpeed)
					{
						gameState = Racing;
						CDFont->Draw(go, KWidth / 2, KHeight / 2, kWhite, kCentre);
						camRotY = KZERO;
						camRotX = KZERO;
					}
					else CDFont->Draw(to_string(iTime), KWidth / 2, KHeight / 2, kWhite, kCentre);
				}
				else CDFont->Draw(start, KWidth / 2, KHeight / 2, kWhite, kCentre);
				if (myEngine->KeyHit(quit)) myEngine->Stop(); // If the user wants to quit then the game exits
				if (myEngine->KeyHit(startButton) && hasStarted == false) // The player starts the countdown
				{
					hasStarted = true; // The game has begun so the countdown will now start
				}
			}
			// If the game is finished because the player completed the race then the following is output
			else if (gameState == Over && playerState != Dead)
			{
				// Creating the UI strings and their positon on screen
				const string raceOver = "Race Complete";
				const string restart = "Press space to restart";
				const string fastLap = "Best Lap: " + (to_string(bestLap) + "s");
				const int raceComplete = KHeight * 0.4f;
				const int bestLapHeight = KHeight * 0.6f;
				// Drawing the UI to the screen
				CDFont->Draw(raceOver, KWidth / 2, raceComplete, kWhite, kCentre);
				CDFont->Draw(restart, KWidth / 2, KHeight / 2, kWhite, kCentre);
				CDFont->Draw(fastLap, KWidth / 2, bestLapHeight, kWhite, kCentre);
				// Player exits the game
				if (myEngine->KeyHit(quit)) myEngine->Stop();
				// The player restarts the race so the crosses must be removed and the game reset
				if (myEngine->KeyHit(startButton))
				{
					crossMesh->RemoveModel(crosses[0]);
					RestartGame(playerHC, checkpointDetails, playerStartLoc, countdown, camRotX, camRotY);
				}
			}
			// If the race is over because the player died then a similar finished screen is shown but with different strings
			else if (gameState == Over && playerState == Dead)
			{
				// Camera is detached and player car is hidden to make it look like it was destroyed
				myCamera->DetachFromParent();
				playerHC.base->SetY(-20.0f);
				// The UI strings are created
				const string gameOver = "Game Over";
				const string noHealth = "Your ship was destroyed";
				const string restart = "Press 'SPACE' to restart";
				// The positions of the UI on the screen are calculated 
				const int restartHeight = KHeight * 0.6f;
				const int destroyedH = KHeight * 0.4f;
				// The UI is drawn to the screen
				CDFont->Draw(gameOver, KWidth / 2, destroyedH, kWhite, kCentre);
				CDFont->Draw(noHealth, KWidth / 2, KHeight / 2, kWhite, kCentre);
				CDFont->Draw(restart, KWidth / 2, restartHeight, kWhite, kCentre);
				if (myEngine->KeyHit(quit)) myEngine->Stop();
				// The game is reset so the crosses are destroyed and the level is reset back to default
				if (myEngine->KeyHit(startButton))
				{
					crossMesh->RemoveModel(crosses[0]);
					RestartGame(playerHC, checkpointDetails, playerStartLoc, countdown, camRotX, camRotY);
				}
			}
		}
	}
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

void CheckCameraControls(EKeyCode camFwd, EKeyCode camBwd, EKeyCode camLeft, EKeyCode camRight, IModel* cameraMount, float kCameraMoveSpeed)
{
	if (myEngine->KeyHeld(camFwd))
	{
		cameraMount->MoveLocalZ(kCameraMoveSpeed * frameTime);
	}
	if (myEngine->KeyHeld(camBwd))
	{
		cameraMount->MoveLocalZ(-kCameraMoveSpeed * frameTime);
	}
	if (myEngine->KeyHeld(camLeft))
	{
		cameraMount->MoveLocalX(-kCameraMoveSpeed * frameTime);
	}
	if (myEngine->KeyHeld(camRight))
	{
		cameraMount->MoveLocalX(kCameraMoveSpeed * frameTime);
	}
}

// This function will restart the game and take control of any models and states that need changing, any variables local to the main loop are changed in the main loop
void RestartGame(HoverCar &playerHC, CheckPoint checkpoints[],Vector3D playerSpawn, float &countdown, float &camX, float &camY)
{
	lapNo = 0;
	checkpointNo = 0;
	bestLap = 0.0f;
	lapTime = 0.0f;
	for (int i = 0; i < KNOCHECK; i++)
	{
		checkpoints[i].checkState = active;
	}

	playerHC.fwdThrust = 0.0f; // The player's speed is reset to 0 as they haven't started racing yet
	playerHC.health = 100.0f;
	playerHC.base->SetPosition(playerSpawn.GetX(), playerSpawn.GetY(), playerSpawn.GetZ()); // The player's location is set to back behind the first checkpoint
	playerHC.base->ResetOrientation(); // The player's car rotation is reset to be facing theright direction from set off
	gameState = Start;
	countdown = 4.0f;
	camX = 0.0f;
	camY = 0.0f;
}

// This will detect for collisions between the player and the current checkpoint objective
bool CheckpointCollision(IModel* playerCar, CheckPoint checkpoints[], IModel* crosses[], IMesh* cross)
{
	CollisionDetection cpCollision;
	const float checkpointLength = 10.0f;
	const float checkpointWidth = 4.0f;
	float x;
	float z;
	bool hasPassedCP = false;

	hasPassedCP = cpCollision.Checkpoint(playerCar, checkpoints[checkpointNo].checkpointM);

	// The checkpoint number increments if a checkpoint has passed and the lap number increases if all checkpoints for the lap are pressed

	// CREATE A STATE FOR EACH CHECKPOINT SO IT ONLY CHECKS PASSED ONES
	if (hasPassedCP && checkpoints[checkpointNo].checkState == active)
	{
		checkpoints[checkpointNo].checkState = passed;
		x = checkpoints[checkpointNo].checkpointM->GetX();
		z = checkpoints[checkpointNo].checkpointM->GetZ();
		crosses[checkpointNo] = cross->CreateModel(x, 2.5f, z);

		if (checkpointNo == 2 || checkpointNo == 5)
		{
			crosses[checkpointNo]->RotateY(90.0f);
		}

		if (checkpointNo == KNOCHECK - 1)
		{
			cross->RemoveModel(crosses[0]);
		}

		if (crosses[checkpointNo] != NULL)
		{
			crosses[checkpointNo]->Scale(0.25f);
		}


		if (checkpointNo == 0)
		{
			// If they pass a checkpoint without being on their first lap, then the race starts
			
			if (lapTime < bestLap && lapNo > 1)
			{
				bestLap = lapTime;
				bNewLapRecord = true;
			}
			else if (lapNo == 1)
			{
				bestLap = lapTime;
			}
			lapNo++; // Increment the lap number if the last checkpoint is passed
			lapTime = 0.0f;
			if (lapNo > 1)
			{
				ClearCheckPoints(crosses, cross);
			}

			for (int i = 0; i < KNOCHECK; i++)
			{
				checkpoints[i].checkState = active;
			}
		}

		checkpointNo++; // If it is not the last checkpoint then increment the checkpoint number.
		if (checkpointNo == KNOCHECK) checkpointNo = 0;

		checkpoints[checkpointNo].checkState = active;
		if (lapNo > KNOLAPS) gameState = Over;
	}

	return hasPassedCP;
}

// This will call the collision class to check for collisions and affect the player's health & momentum if needed
bool wtCollision(IModel* wTank[], HoverCar &playerHC, float time)
{
	CollisionDetection cd;
	bool collision = false;
	const float playerRad = 3.0f;
	const float wtRad = 2.5f;
	float px = playerHC.base->GetX();
	float pz = playerHC.base->GetZ();
	float X;
	float Z;
	float distance;

	for (int i = 0; i < kNoWT; i++)
	{
		X = wTank[i]->GetX();
		X -= px;
		Z = wTank[i]->GetZ();
		Z -= pz;
		distance = sqrtf((X * X) + (Z * Z));
		if (distance < (playerRad + wtRad))
		{
			collision = true;
		}
	}

	// If the player collided with a water tank then they are rebounded at a slightly higher speed to exit the collision
	// The speed is then reduced to seem more appropriate
	if (collision)
	{
		// If the player is going fast enough, then they shall receive damage
		if (playerHC.timeSinceDMG >= kDmgTime && (playerHC.fwdThrust >= 10.0f || playerHC.fwdThrust <= -10.0f))
		{
			TakeDamage(playerHC);
		}
		playerHC.fwdThrust *= -1.1f;
		HoverCarMove(playerHC, frameTime);
		playerHC.fwdThrust *= 0.6f;
	}

	return collision;
}

// This function will handle all of the isles and wall collisions to check if the player is colliding with any of them
bool isleWallCollision(IModel* playerCar, Isle wallIsles[], HoverCar &playerHC)
{
	// Decalring the constants and variables that will be used for detection
	float playerRad = 2.5f;
	float pX = playerCar->GetX();
	float pZ = playerCar->GetZ();
	bool hasCollided = false;

	for (int i = 0; i < KNOWALLS; i++)
	{
		if (pX >= wallIsles[i].minX - playerRad && pX <= wallIsles[i].maxX + playerRad
			&& pZ <= wallIsles[i].maxZ + playerRad && pZ >= wallIsles[i].minZ - playerRad)
		{
			playerHC.fwdThrust *= -1.0f;
			HoverCarMove(playerHC, frameTime);
			playerHC.fwdThrust *= 0.6f;
			hasCollided = true;
		}
	}

	if (hasCollided)
	{
		if (playerHC.timeSinceDMG >= kDmgTime)
		{
			TakeDamage(playerHC);
		}
	}


	return hasCollided;

}

void CheckCollisionTyres(HoverCar &hc, IModel* checkpoints[], float frameTime)
{
	CollisionDetection cd;
	bool hasCollided = false;

		hasCollided = cd.CheckpointTyres(hc.base, checkpoints, KNOCHECK);
		if (hasCollided)
		{
			if (hc.timeSinceDMG >= kDmgTime && (hc.fwdThrust >= 10.0f || hc.fwdThrust <= -10.0f))
			{
				TakeDamage(hc);
			}
			hc.fwdThrust *= -1.1f;
			HoverCarMove(hc, frameTime);
			hc.fwdThrust *= 0.6f;
		}

}
//--- This procedure will output the GUI elements to the screen such as speed, laps, checkpoints etc ---//
void UI(IFont* font, int height, int width, float playerSpeed, float boostTime, bool boostActive, int health)
{
	// Creating variables that will store the location of the UI Elements
	float rowSpace1 = 0.005f;
	float rowSpace2 = 0.025f;
	float rowSpace3 = 0.05f;
	float colSpace1 = 0.005f;
	float colSpace2 = 0.1f;
	float colSpace3 = 0.2f;
	int x = (int)width * rowSpace1;
	int y = (int)height * colSpace1;
	int pSpeed = (int)(playerSpeed + 0.5f); // 0.5 offsets the speed so that a more constant integer is output

	//  The output will be changed for each thing that needs to printed to the screen
	// The current lap and max laps are only output if the player has started the race 
	string output = "Lap: " + (to_string(lapNo) + " / " + (to_string(KNOLAPS)));
	if (lapNo > 0)
	{
		font->Draw(output, x, y, kBlack, kLeft);
		x = (int)width * colSpace2;
		if (checkpointNo == 0)
		{
			output = " - Stage: " + (to_string(KNOCHECK)) + " / " + (to_string(KNOCHECK));
		}
		else
		{
			output = " - Stage: " + (to_string(checkpointNo)) + " / " + (to_string(KNOCHECK));
		}
		font->Draw(output, x, y, kBlack, kLeft);
	}

	// Creating the string for the speed dialogue and outputting it
	output = "Speed: " + (to_string(pSpeed));
	y = (int)height * rowSpace2;
	x = (int)width * colSpace1;
	font->Draw(output, x, y, kBlack, kLeft);

	// Creating the string for the health of the ship and output it
	output = "Health: " + (to_string(health));
	y = (int)height * rowSpace3;
	x = (int)width * colSpace1;
	font->Draw(output, x, y, kBlack);

	// Creating the Boost UI and setting the output locations on the screen
	x = (int)width * colSpace2;
	y = (int)height * rowSpace2;

	if (boostActive)
	{	
		// The boost is about to overheat, output a warning to the player
		if (boostTime <= 1.0f && boostTime >= 0.0f)
		{
			output = "WARNING! BOOST OVERHEATING!";
		}
		else
		{
			output = "Boost: Active";
		}
		font->Draw(output, x, y, kBlack);
	}
	else
	{
		output = "Boost: Inactive";
		font->Draw(output, x, y, kBlack);
	}

	// The UI outputs the best and current lap times by converting them into integers 
	// For the amount of seconds each lap has taken, this is then converted into a string and printed to the screen

	x = width * colSpace2;
	y = height * rowSpace3;

	output = "Lap Time: " + (to_string((int)lapTime) + "s");
	font->Draw(output, x, y, kBlack);

	x = width * colSpace3;
	y = height * rowSpace3;

	output = "Best Lap Time: " + (to_string((int)bestLap) + "s");
	font->Draw(output, x, y, kBlack);
}

// The UI is created and outputted for when the game is paused
void PausedUI(IFont* uiFont, int width, int height)
{
	// The location of the UI on the screen
	float pausey = 0.3f;
	float line1y = 0.45f;
	float line2y = 0.55f;
	// The UI that will be output to the screen
	const string paused = "PAUSED";
	const string pauseInstruction = "Press 'P' to continue playing";
	const string resetInstruction = "Press 'SPACE' to restart the game";
	// The UI is drawn to the screen for the player to see
	uiFont->Draw(paused, width / 2, height * pausey, kWhite, kCentre);
	uiFont->Draw(pauseInstruction, width / 2, height * line1y, kWhite, kCentre);
	uiFont->Draw(resetInstruction, width / 2, height * line2y, kWhite, kCentre);

}

void NewLapRecordUI(IFont* uiFont, int width, int height)
{
	const float yCoordinate = height / 2;
	const float xCoordinate = width / 2;
	const string newRecord = "NEW LAP RECORD!";
	uiFont->Draw(newRecord, xCoordinate, yCoordinate, kRed, kCentre);
}

// The forward vector is taken from the models matrix and normalised to find the direction it is going in
Vector3D GetForwardVector(IModel* base)
{
	float matrix[4][4];
	base->GetMatrix(&matrix[0][0]);
	Vector3D facingVector{ matrix[2][0],matrix[2][1],matrix[2][2] };
	facingVector.Normalise();
	return facingVector;
}

// The momentum of the hover car is calculated with the facing vector and the amount of thrust force applied
Vector3D GetMomentum(Vector3D facingVector, float thrust, float frameTime)
{
	const float kDrag = 0.1;
	Vector3D momentumVector = ( facingVector * thrust);
	momentumVector.SetY(0.0f);

	
	// The amount of momentum will be reduced by the amount of drag
	float x = momentumVector.GetX() * kDrag;
	float y = momentumVector.GetY() * kDrag;
	float z = momentumVector.GetZ() * kDrag;

	Vector3D dragVector{ x,y,z };
	dragVector.SetX(dragVector.GetX());
	dragVector.SetY(dragVector.GetY());
	dragVector.SetZ(dragVector.GetZ());

	momentumVector = momentumVector - dragVector;
	momentumVector.SetX(momentumVector.GetX() * frameTime);
	momentumVector.SetY(momentumVector.GetY() * frameTime);
	momentumVector.SetZ(momentumVector.GetZ() * frameTime);

	return momentumVector;
}

// The car will bob up and down ever so slightly to make it seem like it is floating in the air, the 
Vector3D carBobVector(HoverCar &hc, Vector3D momentumvector, float frameTime)
{
	const float bobSpeed = 1.0f / 1;
	float speed = bobSpeed * frameTime;
		// The car is currently moving upwards and will continue to do so until it reaches its max height limit
	if (hc.cb == Up)
	{
		if (hc.base->GetY() >= 0.5f)
		{
			hc.cb = Down;
			momentumvector.SetY(-speed);
		}
		else
		{
			momentumvector.SetY(speed);
		}
	}
	else
		// The car is moving downwards but will begin to move upwards again once it reaches the minimum height
	{
		if (hc.base->GetY() <= -0.50f)
		{
			hc.cb = Up;
			momentumvector.SetY(speed);
		}
		else
		{
			momentumvector.SetY(-speed);
		}
	}
	return momentumvector;
}

// The hover car is moved according to its momentum vector
void HoverCarMove(HoverCar &hc,float frameTime)
{
	hc.facingVector = GetForwardVector(hc.base); // Gets the facing vector of the model
	hc.momentumVector = GetMomentum(hc.facingVector, hc.fwdThrust, frameTime); // Calulates the new momentum
	hc.base->Move(hc.momentumVector.GetX(), hc.momentumVector.GetY(), hc.momentumVector.GetZ()); // The base is moved by momentum
}

// The checkpoints are cleared of the cross that marks whether the player has passed them that lap or not
void ClearCheckPoints(IModel* crosses[],IMesh* cross)
{
	for (int i = 1; i < KNOCHECK; i++)
	{
		cross->RemoveModel(crosses[i]); // The cross is removed in the array slot
	}
}

// The hover car will receive damage and reset the delay since it last took damage
void TakeDamage(HoverCar &hc)
{
	hc.health--;
	hc.timeSinceDMG = 0.0f;
	if (hc.health <= 0)
	{
		playerState = Dead;
		gameState = Over;
	}
}

// Checks to see whether the player has attempted to go outside of the race boundary
void CheckMapBoundary(HoverCar &hc, float frameTime)
{
	CollisionDetection cd;
	bool hasCollided = cd.MapRadius(hc.base);
	if (hasCollided)
	{
		// Hover car is moved out of the boundary range by going back more than the amount it moved
		hc.fwdThrust *= -1.0f;
		HoverCarMove(hc, frameTime);
		// Car is then slowed down to provide a realistic feel
		hc.fwdThrust *= 0.4f;
	}
}