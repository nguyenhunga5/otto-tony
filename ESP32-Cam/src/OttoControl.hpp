#ifndef OTTOCONTROL_H
#define OTTOCONTROL_H
#pragma once

#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>

#define STX 14
#define SRX 15
#define BAUD_RATE 115200

class OttoControl
{
private:

public:

	OttoControl();
	~OttoControl();

	SoftwareSerial* swSerial;

	void start(AsyncWebServer* server);
};
#endif