#include "basic-midi2cv.h"

BasicMidi2CV::BasicMidi2CV(brain::io::AudioCvOutChannel cv_channel, uint8_t midi_channel) :
	button_a_(GPIO_BRAIN_BUTTON_1),
	button_b_(GPIO_BRAIN_BUTTON_2),
	pots_()
{
	midi_to_cv_.init(cv_channel, 1);
	midi_channel_ = midi_channel;
	state_ = State::kDefault;

	button_a_.init();
	button_b_.init();

	// Button A handlers
	button_a_.set_on_press([this]() {
		button_a_on_press();
	});
	button_a_.set_on_release([this]() {
		button_a_on_release();
	});

	// Button B handlers
	button_b_.set_on_press([this]() {
		button_b_on_press();
	});
	button_b_.set_on_release([this]() {
		button_b_on_release();
	});

	// Init leds
	for (size_t i = 0; i < NO_OF_LEDS; i++) {
		leds_[i] = Led(led_pins[i]);
	}

	// Pots setup
	pots_.set_simple(true);
	pots_.set_output_resolution(4);
}

void BasicMidi2CV::button_a_on_press() {
	if (state_ == State::kDefault) {
		state_ = State::kSetMidiChannel;
	}
}

void BasicMidi2CV::button_a_on_release() {
	midi_to_cv_.set_midi_channel(midi_channel_);
	state_ = State::kDefault;
}

void BasicMidi2CV::button_b_on_press() {
	if (state_ == State::kDefault) {
		state_ = State::kSetCVChannel;
	}
}

void BasicMidi2CV::button_b_on_release() {
	state_ = State::kDefault;
}

void BasicMidi2CV::update() {
	midi_to_cv_.update();
	button_a_.update();
	button_b_.update();

	switch (state_)
	{
	// Read pot X and set MIDI channel accordingly
	case State::kSetMidiChannel:
		pots_.get(0);
		midi_channel_ = 1;
		break;

	// Read pot Y and set CV channel
	case State::kSetCVChannel:
		cv_channel_ = brain::io::AudioCvOutChannel::kChannelB;
		break;

	default:
		break;
	}
}

State BasicMidi2CV::get_state() {
	return state_;
}

uint8_t BasicMidi2CV::get_midi_channel() {
	return midi_channel_;
}