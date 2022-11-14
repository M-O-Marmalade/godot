/*************************************************************************/
/*  audio_effect_sample_rate.cpp                                              */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "audio_effect_sample_rate.h"
#include "servers/audio_server.h"
#include "core/math/math_funcs.h"

void AudioEffectSampleRateInstance::process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) {

	float frames_until_next_sample = AudioServer::get_singleton()->get_mix_rate() / base->rate;

	for (int i = 0; i < p_frame_count; i++) {
		
		if (processed_frames >= frames_until_next_sample) {

			// sample new frame
			last_sampled_frame = p_src_frames[i];

			// apply noise to sampled frame
			float makeup_gain = 1.0f * (1.0f + (0.5f - Math::absf(-0.5f + base->noise_width)) * Math::db_to_linear(1.5f));
			float noise_m = 1.0f + (base->noise * (Math::randf() - 0.5f) * makeup_gain);
			float noise_l = 1.0f + (base->noise * (Math::randf() - 0.5f) * makeup_gain);
			float noise_r = 1.0f + (base->noise * (Math::randf() - 0.5f) * makeup_gain);
			last_sampled_frame.l *= Math::lerp(noise_m, noise_l, base->noise_width);
			last_sampled_frame.r *= Math::lerp(noise_m, noise_r, base->noise_width);

			processed_frames -= frames_until_next_sample;
		}
		processed_frames++;

		AudioFrame current_frame(last_sampled_frame);

		// mix with original signal
		current_frame = current_frame * base->mix + p_src_frames[i] * (1.0f - base->mix);

		// output to destination frame
		p_dst_frames[i] = current_frame;
	}
}

Ref<AudioEffectInstance> AudioEffectSampleRate::instantiate() {
	Ref<AudioEffectSampleRateInstance> ins;
	ins.instantiate();
	ins->base = Ref<AudioEffectSampleRate>(this);
	return ins;
}

void AudioEffectSampleRate::set_rate(float p_rate) {
	rate = p_rate;
}
float AudioEffectSampleRate::get_rate() const {
	return rate;
}

void AudioEffectSampleRate::set_noise(float p_noise) {
	noise = p_noise / 100;
}
float AudioEffectSampleRate::get_noise() const {
	return noise * 100;
}

void AudioEffectSampleRate::set_noise_width(float p_noise_width) {
	noise_width = p_noise_width / 100;
}
float AudioEffectSampleRate::get_noise_width() const {
	return noise_width * 100;
}

void AudioEffectSampleRate::set_mix(float p_mix) {
	mix = p_mix / 100;
}
float AudioEffectSampleRate::get_mix() const {
	return mix * 100;
}

void AudioEffectSampleRate::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_rate", "rate"), &AudioEffectSampleRate::set_rate);
	ClassDB::bind_method(D_METHOD("get_rate"), &AudioEffectSampleRate::get_rate);

	ClassDB::bind_method(D_METHOD("set_noise", "noise"), &AudioEffectSampleRate::set_noise);
	ClassDB::bind_method(D_METHOD("get_noise"), &AudioEffectSampleRate::get_noise);

	ClassDB::bind_method(D_METHOD("set_noise_width", "noise_width"), &AudioEffectSampleRate::set_noise_width);
	ClassDB::bind_method(D_METHOD("get_noise_width"), &AudioEffectSampleRate::get_noise_width);

	ClassDB::bind_method(D_METHOD("set_mix", "mix"), &AudioEffectSampleRate::set_mix);
	ClassDB::bind_method(D_METHOD("get_mix"), &AudioEffectSampleRate::get_mix);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rate", PROPERTY_HINT_RANGE, "20.0,48000.0,10.0,suffix:Hz"), "set_rate", "get_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "noise", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,suffix:%"), "set_noise", "get_noise");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "noise width", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,suffix:%"), "set_noise_width", "get_noise_width");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mix", PROPERTY_HINT_RANGE, "0.0,100.0,0.01,suffix:%"), "set_mix", "get_mix");
}

AudioEffectSampleRate::AudioEffectSampleRate() {
	rate = 48000.0f;
	noise = 0.0f;
	noise_width = 1.0f;
	mix = 1.0f;
}
