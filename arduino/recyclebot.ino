// vim: set foldmethod=marker foldmarker={,} filetype=cpp :

#define HEATER1 8
#define HEATER2 9
#define MOTOR 10
#define LED 13
#define NUM_SENSORS 2
#define SENSOR_HEATER 0
#define SENSOR_TARGET 1

struct Settings {
	float duty;
	float T[NUM_SENSORS];
};

union Buffer {
	Settings c;
	char b[sizeof (Settings)];
};

static uint8_t sensor_pins[NUM_SENSORS] = {SENSOR_HEATER, SENSOR_TARGET};
static float Rs[NUM_SENSORS] = {
	340,
       	340};
static float alpha[NUM_SENSORS] = {
	10.056909432214743,
       	10.056909432214743};
static float beta[NUM_SENSORS] = {
	3885.0342279785623,
       	3885.0342279785623};
static Buffer serial_buffer;
static uint8_t buffer_pos;
static Settings settings;

void setup () {
	Serial.begin (115200);
	pinMode (HEATER1, INPUT);
	pinMode (HEATER2, INPUT);
	pinMode (MOTOR, OUTPUT);
	digitalWrite (HEATER1, LOW);
	digitalWrite (HEATER2, LOW);
	digitalWrite (MOTOR, LOW);
	buffer_pos = 0;
}

void heat (bool heating) {
	pinMode (HEATER1, heating ? OUTPUT : INPUT);
	pinMode (HEATER2, heating ? OUTPUT : INPUT);
	digitalWrite (LED, heating ? HIGH : LOW);
}

float readTemp (uint8_t which) {
	uint16_t adc = analogRead (sensor_pins[which]);
	return beta[which] / (alpha[which] - log (1024. / adc - 1));
}

void do_serial () {
	serial_buffer.b[buffer_pos++] = Serial.read ();
	if (buffer_pos < sizeof (Buffer))
		return;
	if (!isnan (serial_buffer.c.duty))
		settings.duty = serial_buffer.c.duty;
	for (uint8_t t = 0; t < NUM_SENSORS; ++t) {
		if (!isnan (serial_buffer.c.T[t]))
			settings.T[t] = serial_buffer.c.T[t];
	}
}

void loop () {
	while (Serial.available ())
		do_serial ();
	bool heating = true;
	Buffer state;
	state.c.duty = settings.duty;
	for (uint8_t t = 0; t < NUM_SENSORS; ++t) {
		state.c.T[t] = readTemp (t);
		if (state.c.T[t] > settings.T[t])
			heating = false;
	}
	heat (heating);
	if (settings.duty > 0) {
		digitalWrite (MOTOR, HIGH);
		delay (int (1000 * settings.duty));
	}
	if (settings.duty < 100) {
		digitalWrite (MOTOR, LOW);
		delay (1000 * int (1 - settings.duty));
	}
	for (uint8_t t = 0; t < sizeof (Settings); ++t)
		Serial.write (state.b[t]);
}
