


#include <FishFeeder.h>
#include <Servo.h>

#include <StandardCplusplus.h>
#include <vector>

using namespace std;

#include <SerialExt.h>
#include <Motor.h>
//#include "FishFeeder.h"
using namespace Utils;


//Utils _utils;
//Motor _motor;

//---Feeders---

long _secondFeeding = 16; //** make second feeding like 16 hours to only feed when lights come on
vector<FishFeeder> _feeders;
int _statringFeederPin = 7;
//int _feederPwrSigPin = 8; //relay signal pin.
int _potShakesPin = 0; //A0
int _numberOfFeeders = 4;


//---End Feeders---

void setup() {
	//pinMode(_feederPwrSigPin, OUTPUT); //to send relay signal
	// initialize serial:
	Serial.begin(9600);
	//Wait for four seconds or till data is available on serial, whichever occurs first.
	while (Serial.available() == 0 && millis()<2000);
	//while (!Serial); // Wait until Serial is ready - Leonardo
	Serial.println("Press - 1 to run, 2 to go back and forth..");

	_feeders = FishFeeder::CreateFeeders(_numberOfFeeders, _statringFeederPin);//number of feeders, start pin
}

//int _incomingByte = 0;  // a string to hold incoming data from serial com.
void loop() {

	int incomingByte = SerialExt::Read();
	if (incomingByte == 0){
		incomingByte = 49; //on input so run.
	}
	//_utils.Debug("incomingByte: ", incomingByte);
	bool runMotor = Motor::ShouldRunMotor(incomingByte);
	bool runMotorDemo = Motor::ShouldRunMotorDemo(incomingByte);

	if (runMotor) {
		//digitalWrite(_feederPwrSigPin,LOW); //to send relay signal
		int potVal = analogRead(_potShakesPin); // reads the value of the potentiometer (value between 0 and 1023)
		SerialExt::Print("Pot Val: ", potVal);
		FishFeeder::FeedAll(_feeders, potVal);
		//digitalWrite(_feederPwrSigPin,HIGH);
	}
	else if (runMotorDemo)
	{
		FishFeeder::RunDemo(_feeders);
	}
	_secondFeeding = _secondFeeding * 3600000;
	SerialExt::Print("Second Feeding in (Hours): ", _secondFeeding / 3600000);
	delay(_secondFeeding);//delay 9 hours before next feed.
	//delay(10000); //for testing
}

