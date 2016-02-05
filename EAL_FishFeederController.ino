

#include <Servo.h>

#include <StandardCplusplus.h>
#include <vector>

using namespace std;

#include <SerialExt.h>
#include <Motor.h>
#include <FishFeeder.h>
using namespace Utils;


//Utils _utils;
Motor _motor;

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

	CreateFeeders(_numberOfFeeders, _statringFeederPin);//number of feeders, start pin
}

//int _incomingByte = 0;  // a string to hold incoming data from serial com.
void loop() {
	int b = 8;
	int c = b;

	int incomingByte = SerialExt::Read();
	if (incomingByte == 0){
		incomingByte = 49; //on input so run.
	}
	//_utils.Debug("incomingByte: ", incomingByte);
	bool runMotor = _motor.ShouldRunMotor(incomingByte);
	bool runMotorDemo = _motor.ShouldRunMotorDemo(incomingByte);

	if (runMotor) {
		//digitalWrite(_feederPwrSigPin,LOW); //to send relay signal
		FeedAll();
		//digitalWrite(_feederPwrSigPin,HIGH);
	}
	else if (runMotorDemo)
	{
		RunDemo();
	}
	_secondFeeding = _secondFeeding * 3600000;
	SerialExt::Print("Second Feeding in (Hours): ", _secondFeeding / 3600000);
	delay(_secondFeeding);//delay 9 hours before next feed.
	//delay(10000); //for testing
}

//--Controller Methods--

void CreateFeeders(int numOfFeeders, int startingPin){
	if (startingPin > 13 || startingPin < 2) {
		Serial.println("Error: Starting pin has to be less than 14 and greater than 1");
	}
	if (numOfFeeders > 11) {
		Serial.println("Error: Number of feeders has to be less than 12");
	}

	for (int thisFeeder = 0; thisFeeder < numOfFeeders; thisFeeder++) {
		if (startingPin <= 1) return; //pin 1 and 0 cant be outputs

		// twelve servo objects can be created on most boards
		//FishFeeder feeder(Servo(), 13, 2, 0, 14); //servo, pin, shakes, pos, speed
		Servo aServo;
		FishFeeder feeder(aServo, startingPin, 2);
		feeder.Setup();
		_feeders.push_back(feeder);
		startingPin--;
	} 
}

void FeedAll(){
	int shakesVal = GetNumberOfShakes();

	for (int thisFeeder = 0; thisFeeder < _feeders.size(); thisFeeder++) {
		FishFeeder feeder = _feeders[thisFeeder];
		feeder.Shakes = shakesVal;

		Serial.println("Feeding..");
		feeder.Feed();
	}
}
int GetNumberOfShakes(){
	int shakesVal = analogRead(_potShakesPin); // reads the value of the potentiometer (value between 0 and 1023)
	//_utils.Debug("In val: ", val);
	shakesVal = map(shakesVal, 0, 1023, 0, 10); // scale it to use it with the servo (value between 0 and 180)
	SerialExt::Print("# of Shakes: ", shakesVal);
	return shakesVal;

}

void RunDemo(){

	for (int thisFeeder = 0; thisFeeder < _feeders.size(); thisFeeder++) {
		FishFeeder feeder = _feeders[thisFeeder];
		Serial.println("Demoing motor..");
		Servo aServo = feeder.TheServo;

		_motor.RunMotorDemo(aServo);

	}
}
