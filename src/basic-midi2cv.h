#ifndef BASIC_MIDI_TO_CV_H
#define BASIC_MIDI_TO_CV_H

#include "brain-common/brain-gpio-setup.h"
#include "brain-utils/midi-to-cv.h"
#include "brain-ui/button.h"
#include "brain-ui/led.h"

using brain::utils::MidiToCV;
using brain::ui::Button;
using brain::ui::Led;

class BasicMidi2CV
{
public:
	BasicMidi2CV();
	void init();
	void update();

private:
	MidiToCV midi_to_cv_;
	Button button_a_;
	Button button_b_;
	Led led_1_;

	static void button_a_on_release_callback();
	static void button_b_on_release_callback();

	void button_a_on_release();
	void button_b_on_release();
};

#endif