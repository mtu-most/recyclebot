// vim: set foldmethod=marker foldmarker={,} :

#define HEATER1 8
#define HEATER2 9
#define LED 13
#define SENSOR 0

void setup () {
	Serial.begin (115200);
	pinMode (HEATER1, INPUT);
	pinMode (HEATER2, INPUT);
	digitalWrite (HEATER1, LOW);
	digitalWrite (HEATER2, LOW);
}

void heat (bool heating) {
	pinMode (HEATER1, heating ? OUTPUT : INPUT);
	pinMode (HEATER2, heating ? OUTPUT : INPUT);
	digitalWrite (LED, heating ? HIGH : LOW);
}

bool heating = false;

void loop () {
	//if (Serial.available ()) {
	//	Serial.read ();
	//	heating = !heating;
	//	heat (heating);
	//}
	int adc = analogRead (SENSOR);
	heat (adc > 440);
	Serial.print (adc);
	Serial.write ('\n');
	delay (500);
}
