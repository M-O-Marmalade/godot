/*************************************************************************/
/*  audio_effect_bitcrush.h                                                */
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

#ifndef AUDIO_EFFECT_BITCRUSH_H
#define AUDIO_EFFECT_BITCRUSH_H

#include "servers/audio/audio_effect.h"

class AudioEffectBitcrush;

class AudioEffectBitcrushInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectBitcrushInstance, AudioEffectInstance);
	friend class AudioEffectBitcrush;
	Ref<AudioEffectBitcrush> base;

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) override;
};

class AudioEffectBitcrush : public AudioEffect {
	GDCLASS(AudioEffectBitcrush, AudioEffect);

	friend class AudioEffectBitcrushInstance;

	float depth;
	int rate;
	float dry;
	float wet;

	float remaining_samples;
	float last_sampled_values[2];

protected:
	static void _bind_methods();

public:
	Ref<AudioEffectInstance> instantiate() override;
	void set_depth(float p_depth);
	float get_depth() const;
	void set_rate(int p_rate);
	int get_rate() const;
	void set_dry(float p_dry);
	float get_dry() const;
	void set_wet(float p_wet);
	float get_wet() const;

	AudioEffectBitcrush();
};

#endif // AUDIO_EFFECT_BITCRUSH_H
