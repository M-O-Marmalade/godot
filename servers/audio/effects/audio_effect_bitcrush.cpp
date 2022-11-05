/*************************************************************************/
/*  audio_effect_bitcrush.cpp                                              */
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

#include "audio_effect_bitcrush.h"
#include "servers/audio_server.h"

void AudioEffectBitcrushInstance::process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) {

	float sample_count = AudioServer::get_singleton()->get_mix_rate() / base->rate;
	float depth_mul = powf(2.0, base->depth);

	for (int i = 0; i < p_frame_count; i++) {

		// if sample counter runs out...
		if (base->remaining_samples <= 1.0) {

			int next_frame = i == p_frame_count - 1 ? i : i + 1;

			base->last_sampled_values[0] = Math::lerp(p_src_frames[i].l, p_src_frames[next_frame].l, base->remaining_samples);
			base->last_sampled_values[1] = Math::lerp(p_src_frames[i].r, p_src_frames[next_frame].r, base->remaining_samples);

			base->remaining_samples += sample_count;
		}
		base->remaining_samples--;


		float current_frame[2];

		// apply S&H (sample & hold)
		current_frame[0] = base->last_sampled_values[0];
		current_frame[1] = base->last_sampled_values[1];

		// apply bit depth reduction
		current_frame[0] = floorf(current_frame[0] * depth_mul + 0.5) / depth_mul;
		current_frame[1] = floorf(current_frame[1] * depth_mul + 0.5) / depth_mul;

		// mix to output using dry/wet controls
		p_dst_frames[i].l = current_frame[0] * base->wet + p_src_frames[i].l * base->dry;
		p_dst_frames[i].r = current_frame[1] * base->wet + p_src_frames[i].r * base->dry;

	}
}

Ref<AudioEffectInstance> AudioEffectBitcrush::instantiate() {
	Ref<AudioEffectBitcrushInstance> ins;
	ins.instantiate();
	ins->base = Ref<AudioEffectBitcrush>(this);
	return ins;
}

void AudioEffectBitcrush::set_depth(float p_depth) {
	depth = p_depth;
}

float AudioEffectBitcrush::get_depth() const {
	return depth;
}

void AudioEffectBitcrush::set_rate(int p_rate) {
	rate = p_rate;
}

int AudioEffectBitcrush::get_rate() const {
	return rate;
}

void AudioEffectBitcrush::set_dry(float p_dry) {
	dry = p_dry;
}

float AudioEffectBitcrush::get_dry() const {
	return dry;
}

void AudioEffectBitcrush::set_wet(float p_wet) {
	wet = p_wet;
}

float AudioEffectBitcrush::get_wet() const {
	return wet;
}

void AudioEffectBitcrush::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_depth", "depth"), &AudioEffectBitcrush::set_depth);
	ClassDB::bind_method(D_METHOD("get_depth"), &AudioEffectBitcrush::get_depth);

	ClassDB::bind_method(D_METHOD("set_rate", "rate"), &AudioEffectBitcrush::set_rate);
	ClassDB::bind_method(D_METHOD("get_rate"), &AudioEffectBitcrush::get_rate);

	ClassDB::bind_method(D_METHOD("set_dry", "dry"), &AudioEffectBitcrush::set_dry);
	ClassDB::bind_method(D_METHOD("get_dry"), &AudioEffectBitcrush::get_dry);

	ClassDB::bind_method(D_METHOD("set_wet", "wet"), &AudioEffectBitcrush::set_wet);
	ClassDB::bind_method(D_METHOD("get_wet"), &AudioEffectBitcrush::get_wet);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "depth", PROPERTY_HINT_RANGE, "1.0,16.0,0.01"), "set_depth", "get_depth");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rate", PROPERTY_HINT_RANGE, "20,22050", PROPERTY_HINT_EXP_EASING), "set_rate", "get_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "dry", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_dry", "get_dry");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "wet", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), "set_wet", "get_wet");
}

AudioEffectBitcrush::AudioEffectBitcrush() {
	depth = 16.0;
	rate = 22050;
	dry = 0.0;
	wet = 1.0;

	remaining_samples = 0.0;
	last_sampled_values[0] = 0.0;
	last_sampled_values[1] = 0.0;
}
