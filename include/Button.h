/*
 * buttons.h
 *
 *  Created on: 5 lis 2019
 *      Author: c
 */
#include <Arduino.h>

#ifndef BUTTON_H_
#define BUTTON_H_

#define MIN_PRESS_TIME 20
#define MIN_LONG_PRESS_TIME 500

class Button {
public:
	enum pressed_time_outcome {
		TOO_SHORT=0,
		SHORT=1,
		LONG=2,
        PRESSED=4,
		NOT_PRESSED=8,
	};
    
	 Button(uint8_t _pin): pin{_pin}{}


	unsigned long int button_down_timestamp;
	bool state;
	const uint8_t  pin;
    unsigned long int pressed_time=0;

	int setup(){
		pinMode(pin, INPUT_PULLUP);
		state = HIGH;
		return 0;
	}
	int check_pressed() {
		bool prev_state = state;
		unsigned long int current_timestamp = millis();
		
		update_state();
		if(state == LOW){
			if(prev_state == HIGH){
				button_down_timestamp = current_timestamp;
                Serial.print(pin ); Serial.println(" pressed");
			}
			pressed_time = current_timestamp-button_down_timestamp;
            return PRESSED;
		}
		 if (state == HIGH && prev_state == LOW) {
			//button released
            //Serial.print("button "); Serial.print(pin ); Serial.print(" was pressed for "); Serial.println(pressed_time);

			if (pressed_time < MIN_PRESS_TIME) {
				pressed_time=0;
				return TOO_SHORT;
			} else if (pressed_time < MIN_LONG_PRESS_TIME) {
				//short click
				pressed_time=0;
				return SHORT;
			} else if (pressed_time < 3000) {
                Serial.println("LONG");
				pressed_time=0;
				//long click
				return LONG;
			}

		}
		return NOT_PRESSED;
	}
	bool update_state(){
		state = digitalRead(pin);
		return state;
	}

};

#endif /* BUTTON_H_ */

