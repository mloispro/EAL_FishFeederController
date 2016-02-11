


#include <Servo.h>

#include <StandardCplusplus.h>
#include <vector>

using namespace std;

#include <FishFeeder.h>
#include <SerialExt.h>
#include <ServoMotor.h>
using namespace Utils;


//---Feeders---

long _secondFeeding = 16; //** make second feeding like 16 hours to only feed when lights come on
vector<FishFeeder> _feeders;
int _statringFeederPin = 7;
int _feederPwrSigPin = 12; //relay signal pin.
uint8_t _potShakesPin = A0; //A0
int _numberOfFeeders = 4;


//---End Feeders---

void setup() {
	pinMode(_feederPwrSigPin, OUTPUT); //to send relay signal
	// initialize serial:
	Serial.begin(9600);
	//Wait for four seconds or till data is available on serial, whichever occurs first.
	while (Serial.available() == 0 && millis()<2000);
	//while (!Serial); // Wait until Serial is ready - Leonardo
	Serial.println("Press - 1 to run, 2 to go back and forth..");

	//FishFeeder::FeederPwrSigPin = _feederPwrSigPin;
	_feeders = FishFeeder::CreateFeeders(_numberOfFeeders, _statringFeederPin);//number of feeders, start pin
}

//int _incomingByte = 0;  // a string to hold incoming data from serial com.
void loop() {

	int incomingByte = SerialExt::Read();
	if (incomingByte == 0){
		incomingByte = 49; //on input so run.
	}
	//_utils.Debug("incomingByte: ", incomingByte);
	bool runMotor = ServoMotor::ShouldRunMotor(incomingByte);
	bool runMotorDemo = ServoMotor::ShouldRunMotorDemo(incomingByte);

	if (runMotor) {
		SerialExt::Print("Signaling Relay ON, pin: ", _feederPwrSigPin);
		digitalWrite(_feederPwrSigPin,LOW); //to send relay signal
		int potVal = analogRead(A0); // reads the value of the potentiometer (value between 0 and 1023)
		SerialExt::Print("Pot Val: ", potVal);
		FishFeeder::FeedAll(_feeders, potVal);
		SerialExt::Print("Signaling Relay OFF, pin: ", _feederPwrSigPin);
		digitalWrite(_feederPwrSigPin,HIGH);
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

