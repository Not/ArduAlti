/*
 * buttons.h
 *
 *  Created on: 5 lis 2019
 *      Author: c
 */
#include <arduino.h>

#ifndef BUTTON_H_
#define BUTTON_H_

#define MIN_PRESS_TIME 20
#define MIN_LONG_PRESS_TIME 500

class Button {
public:
	enum button_press {
		NOT_PRESSED=0,
		PRESSED=1,
		LONG_PRESSED=2,
	};
    
	 Button(uint8_t _pin): pin{_pin}{}

	int setup(){
		pinMode(pin, INPUT_PULLUP);
		state = HIGH;
		return 0;
	}
	int check_pressed() {
		bool prev_state = state;
		unsigned long int current_time = millis();
		read();
		if (state == LOW && prev_state == HIGH) {
			button_down_time = current_time;

		} else if (state == HIGH && prev_state == LOW) {

			if (current_time - button_down_time < MIN_PRESS_TIME) {
				return NOT_PRESSED;
			} else if (current_time - button_down_time < MIN_LONG_PRESS_TIME) {
				//short click
				return PRESSED;
			} else if (current_time - button_down_time < 1500) {
				//long click
				return LONG_PRESSED;
			}

		}
		return 0;
	}
	bool read(){
		state = digitalRead(pin);
		return state;
	}
	unsigned long int button_down_time;
	bool state;
	const uint8_t  pin;
};

#endif /* BUTTON_H_ */
