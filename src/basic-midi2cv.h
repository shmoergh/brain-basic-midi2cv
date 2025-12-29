#ifndef BASIC_MIDI_TO_CV_H
#define BASIC_MIDI_TO_CV_H

#include <vector>

#include "brain-common/brain-gpio-setup.h"
#include "brain-utils/midi-to-cv.h"
#include "brain-ui/button.h"
#include "brain-ui/led.h"

using brain::utils::MidiToCV;
using brain::ui::Button;
using brain::ui::Led;

#define NO_OF_LEDS 6

const uint8_t led_pins[NO_OF_LEDS] = {
	GPIO_BRAIN_LED_1,
	GPIO_BRAIN_LED_2,
	GPIO_BRAIN_LED_3,
	GPIO_BRAIN_LED_4,
	GPIO_BRAIN_LED_5,
	GPIO_BRAIN_LED_6
};

const enum State {
	kDefault = 0,
	kSetMidiChannel = 1,
	kSetCVChannel = 2
};

class BasicMidi2CV
{
public:
	BasicMidi2CV(brain::io::AudioCvOutChannel cv_channel, uint8_t midi_channel);
	void update();
	State get_state();
	uint8_t get_midi_channel();

private:
	MidiToCV midi_to_cv_;
	Button button_a_;
	Button button_b_;
	std::vector<Led> leds_;
	uint8_t midi_channel_;
	brain::io::AudioCvOutChannel cv_channel_;
	State state_;

	void button_a_on_press();
	void button_a_on_release();

	void button_b_on_press();
	void button_b_on_release();
};

#endif