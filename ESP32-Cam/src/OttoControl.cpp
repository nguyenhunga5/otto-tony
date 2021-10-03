#include "OttoControl.hpp"  

OttoControl::OttoControl()
{
    swSerial = new SoftwareSerial();
    swSerial->begin(BAUD_RATE, SWSERIAL_8N1, STX, SRX, false, 95, 11);
}

OttoControl::~OttoControl()
{

}