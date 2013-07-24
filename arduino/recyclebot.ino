// vim: set foldmethod=marker foldmarker={,} :

#define HEATER1 8
#define HEATER2 9
#define MOTOR 10
#define LED 13
#define SENSOR 0

static int target = 440;
static int duty = 0;

void setup () {
	Serial.begin (115200);
	pinMode (HEATER1, INPUT);
	pinMode (HEATER2, INPUT);
	pinMode (MOTOR, OUTPUT);
	digitalWrite (HEATER1, LOW);
	digitalWrite (HEATER2, LOW);
	digitalWrite (MOTOR, LOW);
}

void heat (bool heating) {
	pinMode (HEATER1, heating ? OUTPUT : INPUT);
	pinMode (HEATER2, heating ? OUTPUT : INPUT);
	digitalWrite (LED, heating ? HIGH : LOW);
}

bool heating = false;

void loop () {
	while (Serial.available ()) {
		uint8_t c = Serial.read ();
		switch (c) {
		case '!':
			target = 1024;
			duty = 0;
			break;
		case '=':
			target += 1;
			break;
		case '-':
			target -= 1;
			break;
		case '+':
			target += 10;
			break;
		case '_':
			target -= 10;
			break;
		case ',':
			duty -= 1;
			break;
		case '.':
			duty += 1;
			break;
		case '<':
			duty -= 10;
			break;
		case '>':
			duty += 10;
			break;
		default:
			break;
		}
		heating = !heating;
		heat (heating);
	}
	int adc = analogRead (SENSOR);
	heat (adc > target);
	Serial.print ("adc:");
	Serial.print (adc);
	Serial.print (" (");
	Serial.print (target);
	Serial.print ("); pwm:");
	Serial.print (duty);
	Serial.print ("/100     \r");
	if (duty > 0) {
		digitalWrite (MOTOR, HIGH);
		delay (10 * duty);
	}
	if (duty < 100) {
		digitalWrite (MOTOR, LOW);
		delay (10 * (100 - duty));
	}
}
