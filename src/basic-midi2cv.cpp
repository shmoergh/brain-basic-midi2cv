#include "basic-midi2cv.h"

BasicMidi2CV::BasicMidi2CV(brain::io::AudioCvOutChannel cv_channel, uint8_t midi_channel) :
	button_a_(GPIO_BRAIN_BUTTON_1),
	button_b_(GPIO_BRAIN_BUTTON_2),
	pots_()
{
	midi_to_cv_.init(cv_channel, 1);
	midi_channel_ = midi_channel;
	cv_channel_ = cv_channel;
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
	reset_leds_ = false;

	// Pots setup
	pots_.set_simple(true);
	pots_.set_output_resolution(4); // Enough to have 0-15 for pot resolution

}

void BasicMidi2CV::button_a_on_press() {
	if (state_ == State::kDefault) {
		state_ = State::kSetMidiChannel;
	}
}

void BasicMidi2CV::button_a_on_release() {
	if (state_ == State::kSetMidiChannel) {
		midi_to_cv_.set_midi_channel(midi_channel_);
	}
	state_ = State::kDefault;
}

void BasicMidi2CV::button_b_on_press() {
	if (state_ == State::kDefault) {
		state_ = State::kSetCVChannel;
	}
}

void BasicMidi2CV::button_b_on_release() {
	if (state_ == State::kSetCVChannel) {
		midi_to_cv_.set_pitch_channel(cv_channel_);
	}
	state_ = State::kDefault;
}

void BasicMidi2CV::update() {
	midi_to_cv_.update();
	button_a_.update();
	button_b_.update();

	switch (state_) {
		// Read pot X and set MIDI channel accordingly
		case State::kSetMidiChannel: {
			uint8_t pot_a_value = pots_.get(POT_MIDI_CHANNEL);
			pot_a_value = clamp(0, 15, pot_a_value);

			midi_channel_ = pot_a_value + 1;

			leds_.set_from_mask(pot_a_value);
			reset_leds_ = true;
			break;
		}

		// Read pot Y and set CV channel
		case State::kSetCVChannel: {
			uint8_t pot_b_value = pots_.get(POT_CV_CHANNEL);
			uint8_t led_mask = 0b000000;

			if (pot_b_value < POT_CV_CHANNEL_THRESHOLD) {
				cv_channel_ = brain::io::AudioCvOutChannel::kChannelA;
				led_mask = LED_MASK_CHANNEL_A;
			} else {
				cv_channel_ = brain::io::AudioCvOutChannel::kChannelB;
				led_mask = LED_MASK_CHANNEL_B;
			}

			leds_.set_from_mask(led_mask);
			reset_leds_ = true;
			break;
		}

		default: {
			if (reset_leds_) {
				leds_.off_all();
			}
			reset_leds_ = false;
			break;
		}
	}
}

State BasicMidi2CV::get_state() const {
	return state_;
}

uint8_t BasicMidi2CV::get_midi_channel() const {
	return midi_channel_;
}