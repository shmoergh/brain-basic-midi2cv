#include "basic-midi2cv.h"

BasicMidi2CV::BasicMidi2CV() :
	led_1_(GPIO_BRAIN_LED_1),
	button_a_(GPIO_BRAIN_BUTTON_1),
	button_b_(GPIO_BRAIN_BUTTON_2)
{}

void BasicMidi2CV::init() {
	midi_to_cv_.init(brain::io::AudioCvOutChannel::kChannelB, 1);
	button_a_.init();
	button_b_.init();
}

void BasicMidi2CV::update() {
	midi_to_cv_.update();
	button_a_.update();
	button_b_.update();
}