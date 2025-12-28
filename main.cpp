#include <pico/stdlib.h>
#include <stdio.h>

#include "brain-utils/midi-to-cv.h"
#include "brain-io/audio-cv-out.h"

int main() {
	stdio_init_all();

	printf("brain-default-midi2cv started\n");

	brain::utils::MidiToCV midiToCV;
	midiToCV.init(brain::io::AudioCvOutChannel::kChannelA, 1);

	while (true) {
		midiToCV.update();
	}

	return 0;
}
