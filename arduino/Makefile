ARDUINO_LIBS=
BOARD_TAG=uno
ARDUINO_PORT=/dev/ttyACM*

include /usr/share/arduino/Arduino.mk

run:
	stty -F /dev/ttyACM0 115200 raw -echo -echoe -echok
	gnome-terminal -e 'cat /dev/ttyACM0'
	gnome-terminal -e 'bash -c "cat > /dev/ttyACM0"'
