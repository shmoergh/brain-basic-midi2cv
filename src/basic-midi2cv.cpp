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

	button_a_.set_on_release([this](){
		button_a_on_release();
	});
	button_b_.set_on_release([this]() {
		button_b_on_release();
	});
}

void BasicMidi2CV::button_a_on_release() {
	printf("Button A pressed\n");
}

void BasicMidi2CV::button_b_on_release() {
	printf("Button B pressed\n");
}

void BasicMidi2CV::update() {
	midi_to_cv_.update();
	button_a_.update();
	button_b_.update();
}