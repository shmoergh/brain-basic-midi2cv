#include "basic-midi2cv.h"

BasicMidi2CV::BasicMidi2CV(brain::io::AudioCvOutChannel cv_channel, uint8_t midi_channel) :
	button_a_(GPIO_BRAIN_BUTTON_1),
	button_b_(GPIO_BRAIN_BUTTON_2),
	pots_()
{
	init(cv_channel, midi_channel);
	set_max_cc_voltage(5);

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

	button_a_pressed_ = false;
	button_b_pressed_ = false;

	// Init leds
	leds_.init();
	leds_.startup_animation();
	reset_leds_ = false;
	playhead_led_ = 0;
	key_pressed_ = false;

	// Pots setup
	brain::ui::PotsConfig pots_config = brain::ui::create_default_config();
	pots_config.simple = true;
	pots_config.output_resolution = 8;
	pots_.init(pots_config);

	// Panic
	panic_timer_start_ = 0;
}

void BasicMidi2CV::button_a_on_press() {
	if (state_ == State::kDefault && state_ != State::kSetMidiChannel) {
		state_ = State::kSetMidiChannel;
	}

	// If button B is pressed and button A is pressed then panic mode starts
	if (state_ == State::kSetCVChannel) {
		state_ = State::kPanicStarted;
	}
}

void BasicMidi2CV::button_a_on_release() {
	if (state_ == State::kSetMidiChannel) {
		set_midi_channel(midi_channel_);
	}
	if (state_ == State::kPanicStarted) {
		reset_panic();
	}
	state_ = State::kDefault;
}

void BasicMidi2CV::button_b_on_press() {
	if (state_ == State::kDefault && state_ != State::kSetCVChannel) {
		state_ = State::kSetCVChannel;
	}

	if (state_ == State::kSetMidiChannel) {
		state_ = State::kPanicStarted;
	}
}

void BasicMidi2CV::button_b_on_release() {
	if (state_ == State::kSetCVChannel) {
		set_pitch_channel(cv_channel_);
	}
	if (state_ == State::kPanicStarted) {
		reset_panic();
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

			// Divide into 16 bins (0-15) for stable MIDI channel selection
			uint8_t binned_value = pot_a_value / 16;
			uint8_t new_midi_channel = binned_value + 1;

			midi_channel_ = new_midi_channel;
			leds_.set_from_mask(midi_channel_);
			reset_leds_ = true;
			break;
		}

		// Read pot Y and set CV channel
		case State::kSetCVChannel: {
			if (is_note_playing()) break;

			uint8_t pot_b_value = pots_.get(POT_CV_CHANNEL);
			uint8_t pot_c_value = pots_.get(POT_MODE);

			uint8_t cv_led_mask = 0b000000;
			if (pot_b_value < POT_CV_CHANNEL_THRESHOLD) {
				cv_channel_ = brain::io::AudioCvOutChannel::kChannelA;
				cv_led_mask = LED_MASK_CHANNEL_A;
			} else {
				cv_channel_ = brain::io::AudioCvOutChannel::kChannelB;
				cv_led_mask = LED_MASK_CHANNEL_B;
			}

			uint16_t new_mode = floor(3 * pot_c_value / 256);
			uint8_t cc_led_mask = 0b000000;
			switch (new_mode) {
				case 0:
					cc_led_mask = LED_MASK_MODE_DEFAULT;
					MidiToCV::set_mode(MidiToCV::Mode::kDefault);
					break;
					case 1:
					cc_led_mask = LED_MASK_MODE_MODWHEEL;
					MidiToCV::set_mode(MidiToCV::Mode::kModWheel);
					break;
					case 2:
					cc_led_mask = LED_MASK_MODE_UNISON;
					MidiToCV::set_mode(MidiToCV::Mode::kUnison);
					break;

			default:
				break;
			}

			uint8_t led_mask = cv_led_mask | cc_led_mask;
			leds_.set_from_mask(led_mask);
			reset_leds_ = true;
			break;
		}

		// Panic
		case State::kPanicStarted: {
			if (panic_timer_start_ == 0) {
				panic_timer_start_ = get_absolute_time();
				leds_.on_all();
			}

			absolute_time_t now = get_absolute_time();
			if (absolute_time_diff_us(panic_timer_start_, now) / 1000 >= PANIC_HOLD_THRESHOLD_MS) {
				MidiToCV::set_gate(false);
				MidiToCV::reset_note_stack();
				state_ = State::kDefault;
				reset_panic();
			}
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

void BasicMidi2CV::reset_panic() {
	panic_timer_start_ = 0;
	leds_.off_all();
}