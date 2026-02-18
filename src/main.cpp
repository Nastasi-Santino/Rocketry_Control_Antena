#include <Arduino.h>
#include <math.h>

// =======================
// Types / Structs
// =======================

struct Position {
  float x;
  float y;
  float z;
};

struct Angles {
  float phi;
  float z;
};

// =======================
// Global variables
// =======================

Position antennaPos;
Position rocketPos;

// =======================
// Function Declarations
// =======================

Angles computeAngles(Position, Position);
Angles radianTodegrees(Angles);


// =======================
// Arduino setup / loop
// =======================

void setup() {
  // Init serial, sensors, motors, etc.
  // Serial.begin(115200);
}

void loop() {


  // 1) Read antenna position (encoders)

  
  // 2) Read rocket position (GPS / telemetry)



  // 3) Compute target angles



  // 4) Command motors



}

Angles computeAngles(Position aPos, Position rPos){

  // Calculations for the line of sight vector Vr - Va

  float dx = rPos.x - aPos.x;
  float dy = rPos.y - aPos.y;
  float dz = rPos.z - aPos.z;

  // Compute Angles in radians
  
  Angles targetAngles;  

  float horizontalDistance = sqrt( (dx * dx) + (dy * dy) ); // Needed for z angle
  
  targetAngles.phi = atan2(dy, dx);

  targetAngles.z = atan2(dz, horizontalDistance);

  return targetAngles;
}

Angles radianTodegrees(Angles angles){

  Angles degreeAngles;

  degreeAngles.phi = angles.phi * 180 / PI;
  degreeAngles.z = angles.z * 180 / PI;

  return degreeAngles;
}