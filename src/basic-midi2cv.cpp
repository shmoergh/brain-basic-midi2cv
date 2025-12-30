#include "basic-midi2cv.h"

BasicMidi2CV::BasicMidi2CV(brain::io::AudioCvOutChannel cv_channel, uint8_t midi_channel) :
	button_a_(GPIO_BRAIN_BUTTON_1),
	button_b_(GPIO_BRAIN_BUTTON_2),
	pots_()
{
	init(cv_channel, midi_channel);
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
	leds_.init();
	leds_.startup_animation();
	reset_leds_ = false;
	playhead_led_ = 0;
	key_pressed_ = false;

	// Pots setup
	brain::ui::PotsConfig pots_config = brain::ui::create_default_config();
	pots_config.simple = true;
	pots_config.output_resolution = 4;
	pots_.init(pots_config);
}

void BasicMidi2CV::button_a_on_press() {
	printf("Button A pressed\n");
	if (state_ == State::kDefault) {
		state_ = State::kSetMidiChannel;
	}
}

void BasicMidi2CV::button_a_on_release() {
	printf("Button A released\n");
	if (state_ == State::kSetMidiChannel) {
		set_midi_channel(midi_channel_);
	}
	state_ = State::kDefault;
}

void BasicMidi2CV::button_b_on_press() {
	printf("Button B pressed\n");
	if (state_ == State::kDefault) {
		state_ = State::kSetCVChannel;
	}
}

void BasicMidi2CV::button_b_on_release() {
	printf("Button B released\n");
	if (state_ == State::kSetCVChannel) {
		set_pitch_channel(cv_channel_);
	}
	state_ = State::kDefault;
}

void BasicMidi2CV::update() {
	button_a_.update();
	button_b_.update();

	switch (state_) {
		// Read pot X and set MIDI channel accordingly
		case State::kSetMidiChannel: {
			if (is_note_playing()) break;
			uint8_t pot_a_value = pots_.get(POT_MIDI_CHANNEL);
			printf("Pot X: %d\n", pot_a_value);
			pot_a_value = clamp(0, 15, pot_a_value);

			midi_channel_ = pot_a_value + 1;

			leds_.set_from_mask(midi_channel_);
			reset_leds_ = true;
			break;
		}

		// Read pot Y and set CV channel
		case State::kSetCVChannel: {
			if (is_note_playing()) break;
			uint8_t pot_b_value = pots_.get(POT_CV_CHANNEL);
			printf("Pot B value: %d\n", pot_b_value);
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
			MidiToCV::update();
			if (is_note_playing()) {
				leds_.on(playhead_led_);
				key_pressed_ = true;
			} else {
				if (key_pressed_) {
					leds_.off(playhead_led_);
					playhead_led_++;
					if (playhead_led_ > 5) playhead_led_ = 0;
					key_pressed_ = false;
				}
			}

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