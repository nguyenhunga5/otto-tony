#ifndef CAMERAHELPER_H
#define CAMERAHELPER_H
#pragma once

#include <ESPAsyncWebServer.h>

class CameraHelper
{
private:
	void makeStreamHandler(AsyncWebServerRequest* request);

public:

	CameraHelper();
	~CameraHelper();

	void startCameraServer(AsyncWebServer* server);

};
#endif