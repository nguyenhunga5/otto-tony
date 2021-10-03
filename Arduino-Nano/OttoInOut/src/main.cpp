#include <Arduino.h>
#include <OttoHumanoid.h>

#define LeftLeg 2 
#define LeftFoot 3
#define RightLeg 4 
#define RightFoot 5 
#define LeftArm 6 
#define RightArm 7

OttoHumanoid* humanoid;
void setup() {
  Serial.begin(115200);
  Serial.println();

  humanoid = new OttoHumanoid();
  humanoid->initHUMANOID(LeftLeg, LeftFoot, RightLeg, RightFoot, LeftArm, RightArm, true, A2, A3, A4, A5);
}

void loop() {

  humanoid->walk(5, 1000, FORWARD);
}