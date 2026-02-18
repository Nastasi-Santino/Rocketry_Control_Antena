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

Position antennaPos{0, 0, 0};
Position rocketPos{0, 0, 0};

// Buffer para recibir una línea
static const size_t RX_BUF_SIZE = 120;
char rxBuf[RX_BUF_SIZE];
size_t rxIdx = 0;

// =======================
// Function Declarations
// =======================

Angles computeAngles(Position, Position);
Angles radianTodegrees(Angles);
bool parsePacketLine(const char*, Position &, Position &);
bool readLineFromSerial(char* outLine, size_t outSize);


// =======================
// Arduino setup / loop
// =======================

void setup() {
  // Init serial, sensors, motors, etc.
  Serial.begin(115200);
}

void loop() {

  char line[RX_BUF_SIZE];

  // 1) Read a complete line if available
  if (readLineFromSerial(line, sizeof(line))) {

    // 2) Parse and update positions
    if (parsePacketLine(line, antennaPos, rocketPos)) {

      // 3) Calculate target angles
      Angles target = computeAngles(antennaPos, rocketPos);
      Angles targetDegrees = radianTodegrees(target);

      // 4) Here would go the command to the motors


    } else {
      // Invalid packet: discard it
      Serial.print("Invalid line: ");
      Serial.println(line);
    }
  }

  // The loop keeps running non-blocking
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

// =======================
// Serial packet handling
// =======================

// Tries to read a complete line (ending with '\n') from Serial.
// Returns true when a complete line is assembled in outLine.
bool readLineFromSerial(char* outLine, size_t outSize) {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    // Ignore CR (in case of \r\n)
    if (c == '\r') continue;

    if (c == '\n') {
      // End of line: terminate string and return it
      rxBuf[rxIdx] = '\0';

      // Copy to outLine (for safety)
      strncpy(outLine, rxBuf, outSize);
      outLine[outSize - 1] = '\0';

      // Reset for the next line
      rxIdx = 0;
      return true;
    }

    // Store char if there is space
    if (rxIdx < RX_BUF_SIZE - 1) {
      rxBuf[rxIdx++] = c;
    } else {
      // Overflow: discard the current line
      rxIdx = 0;
    }
  }
  return false;
}




// Parses a line of the form: P,xa,ya,za,xr,yr,zr
// Returns true if valid and updates positions.
bool parsePacketLine(const char* line, Position &aPos, Position &rPos) {
  // Quick header check
  if (line[0] != 'P' || line[1] != ',') return false;

  float xa, ya, za, xr, yr, zr;

  // NOTE: the pattern expects EXACTLY 6 floats
  int n = sscanf(line, "P,%f,%f,%f,%f,%f,%f", &xa, &ya, &za, &xr, &yr, &zr);
  if (n != 6) return false;

  aPos = {xa, ya, za};
  rPos = {xr, yr, zr};
  return true;
}