#pragma once

#include <juce_core/juce_core.h>
#include <utility>
#include <vector>

// Values in real units; unlisted parameters return to their defaults.
// Osc model: 0 Saw 1 Pulse 2 Fold 3 Sync 4 FM 5 Formant 6 Noise 7 Vector 8 Chip 9 Swarm 10 PhaseDist 11 Harmonic 12 String
// FM ratio via color: 0 = 0.5, .091 = 1, .273 = 2, .455 = 3, .545 = 3.5, .636 = 4, .909 = 7
// Filter: 0 LP12 1 LP24 2 BP 3 HP 4 Notch 5 Comb 6 Formant
// LFO: 0 Sine 1 Tri 2 RampUp 3 RampDown 4 Square 5 S&H 6 Drift 7 Stairs
// Mod src: 0 None 1 LFO1 2 LFO2 3 ModEnv 4 Velocity 5 ModWheel 6 NoteRandom 7 Key
// Mod dst: 0 Pitch(24st) 1 Shape1 2 Shape2 3 Shape3 4 Colors 5 Cutoff 6 Reso 7 Pan 8 Volume 9 XFM 10 Ring 11 LFO1rate
// Mode: 0 Mono 1 Legato 2 Poly
// Arp mode: 0 Up 1 Down 2 UpDown 3 Played 4 Random 5 Converge 6 Diverge 7 Pinky 8 Thumb 9 OctBounce 10 RandomWalk 11 Chord
// Arp rate: 0 1/4 1 1/8 2 1/8T 3 1/16 4 1/16T 5 1/32
// Rhythm: 0 Straight 1 Tresillo 2 Cinquillo 3 E5/16 4 E7/16 5 Gallop 6 Offbeat 7 Bossa 8 Stutter 9 Sparse
// Drive: 0 Soft 1 Hard 2 Fold 3 Crush 4 Rectify | Delay sync: 0 Free 1 1/4 2 1/8 3 1/8D 4 1/16 5 1/4T
// Gate rate: 0 1/8 1 1/16 2 1/32 3 1/8T 4 1/16T | Gate pattern: 0 Pulse 1 Trance 2 Offbeat 3 Stutter 4 Broken 5 Triplet
struct PresetDef
{
    juce::String name;
    std::vector<std::pair<juce::String, float>> values;
};

inline const std::vector<PresetDef>& getFactoryPresets()
{
    static const std::vector<PresetDef> presets {
        { "Init", {} },

        //=== Keys =============================================================
        { "Keys - Velvet E-Piano", {
            { "o1_type", 4 }, { "o1_shape", 0.15f }, { "o1_color", 0.091f }, { "o1_level", 0.85f },
            { "o2_type", 4 }, { "o2_shape", 0.55f }, { "o2_color", 0.909f }, { "o2_level", 0.12f },
            { "filter_type", 0 }, { "cutoff", 6000 }, { "fenv_amt", 0.1f },
            { "amp_attack", 0.002f }, { "amp_decay", 2.5f }, { "amp_sustain", 0.25f }, { "amp_release", 0.6f },
            { "menv_attack", 0.001f }, { "menv_decay", 0.8f },
            { "m1_src", 3 }, { "m1_dst", 1 }, { "m1_amt", 0.5f },
            { "m2_src", 4 }, { "m2_dst", 1 }, { "m2_amt", 0.3f },
            { "chorus_mix", 0.3f }, { "reverb_mix", 0.2f }, { "pan_depth", 0.25f }, { "pan_rate", 3.0f }, { "vel_sens", 0.8f } } },

        { "Keys - Glass Keys", {
            { "o1_type", 11 }, { "o1_shape", 0.7f }, { "o1_color", 0.5f },
            { "o2_type", 4 }, { "o2_shape", 0.3f }, { "o2_color", 0.636f }, { "o2_level", 0.25f }, { "o2_octave", 1 },
            { "filter_type", 0 }, { "cutoff", 9000 }, { "fenv_amt", 0 },
            { "amp_attack", 0.002f }, { "amp_decay", 1.8f }, { "amp_sustain", 0.15f }, { "amp_release", 1.0f },
            { "reverb_mix", 0.3f }, { "reverb_size", 0.6f }, { "delay_sync", 3 }, { "delay_mix", 0.2f } } },

        { "Keys - Felt String Piano", {
            { "o1_type", 12 }, { "o1_shape", 0.35f }, { "o1_color", 0.8f }, { "o1_level", 0.9f },
            { "o2_type", 12 }, { "o2_shape", 0.55f }, { "o2_color", 0.85f }, { "o2_fine", 4 }, { "o2_level", 0.5f },
            { "filter_type", 0 }, { "cutoff", 4500 }, { "fenv_amt", 0 },
            { "amp_attack", 0.001f }, { "amp_sustain", 1.0f }, { "amp_release", 0.8f },
            { "reverb_mix", 0.25f }, { "reverb_size", 0.55f }, { "chorus_mix", 0.1f }, { "vel_sens", 0.7f } } },

        { "Keys - Church Organ", {
            { "o1_type", 11 }, { "o1_shape", 0.55f }, { "o1_color", 0.0f },
            { "o2_type", 11 }, { "o2_octave", 1 }, { "o2_level", 0.4f }, { "o2_color", 0.7f },
            { "filter_type", 0 }, { "cutoff", 8000 }, { "fenv_amt", 0 },
            { "amp_attack", 0.03f }, { "amp_sustain", 1.0f }, { "amp_release", 0.3f }, { "vel_sens", 0 },
            { "reverb_mix", 0.4f }, { "reverb_size", 0.9f }, { "chorus_mix", 0.15f } } },

        { "Keys - Toy Box", {
            { "o1_type", 8 }, { "o1_shape", 0.5f }, { "o1_color", 0.3f },
            { "o2_type", 1 }, { "o2_level", 0.2f }, { "o2_octave", 1 }, { "o2_shape", 0.8f },
            { "filter_type", 0 }, { "cutoff", 10000 }, { "fenv_amt", 0 },
            { "amp_attack", 0.001f }, { "amp_decay", 0.5f }, { "amp_sustain", 0.0f }, { "amp_release", 0.3f },
            { "delay_sync", 2 }, { "delay_mix", 0.25f }, { "delay_fb", 0.4f } } },

        { "Keys - Lo-Fi Clav", {
            { "o1_type", 1 }, { "o1_shape", 0.7f }, { "o1_color", 0.0f },
            { "filter_type", 1 }, { "cutoff", 900 }, { "fenv_amt", 0.7f }, { "flt_decay", 0.2f }, { "flt_sustain", 0.0f },
            { "amp_decay", 0.4f }, { "amp_sustain", 0.3f }, { "amp_release", 0.15f },
            { "tape_age", 0.5f }, { "tape_wow", 0.2f }, { "dist_type", 4 }, { "dist_amt", 0.25f } } },

        { "Keys - Vector Rhodes Dream", {
            { "o1_type", 7 }, { "o1_shape", 0.15f }, { "o1_color", 0.1f },
            { "o2_type", 4 }, { "o2_shape", 0.4f }, { "o2_color", 0.091f }, { "o2_level", 0.3f }, { "o2_octave", 1 },
            { "filter_type", 0 }, { "cutoff", 3500 }, { "fenv_amt", 0.25f },
            { "amp_attack", 0.002f }, { "amp_decay", 3.0f }, { "amp_sustain", 0.2f }, { "amp_release", 0.9f },
            { "phaser_mix", 0.35f }, { "phaser_rate", 0.4f }, { "pan_depth", 0.3f }, { "pan_rate", 4.5f },
            { "reverb_mix", 0.25f }, { "tape_wow", 0.15f } } },

        //=== Pads =============================================================
        { "Pad - Ronnie's Lovers Pad", {
            { "o1_type", 9 }, { "o1_shape", 0.35f }, { "o1_color", 0.6f }, { "o1_level", 0.7f },
            { "o2_type", 0 }, { "o2_shape", 0.2f }, { "o2_color", 0.3f }, { "o2_octave", -1 }, { "o2_fine", -6 }, { "o2_level", 0.35f },
            { "o3_type", 11 }, { "o3_shape", 0.3f }, { "o3_color", 0.2f }, { "o3_octave", 1 }, { "o3_level", 0.25f },
            { "filter_type", 0 }, { "cutoff", 1800 }, { "reso", 0.1f }, { "fenv_amt", 0.15f }, { "keytrack", 0.4f },
            { "flt_attack", 1.2f }, { "flt_decay", 2.0f }, { "flt_sustain", 0.6f }, { "flt_release", 2.0f },
            { "amp_attack", 0.7f }, { "amp_decay", 1.5f }, { "amp_sustain", 0.9f }, { "amp_release", 2.5f }, { "vel_sens", 0.3f },
            { "m1_src", 1 }, { "m1_dst", 0 }, { "m1_amt", 0.006f }, { "lfo1_rate", 4.8f },
            { "m2_src", 2 }, { "m2_dst", 5 }, { "m2_amt", 0.08f }, { "lfo2_rate", 0.2f },
            { "drift", 0.25f }, { "spread", 0.5f },
            { "chorus_mix", 0.45f }, { "chorus_rate", 0.35f }, { "chorus_depth", 0.6f },
            { "reverb_mix", 0.3f }, { "reverb_size", 0.75f }, { "width", 1.3f } } },

        { "Pad - Odin Dust", {
            { "o1_type", 9 }, { "o1_shape", 0.5f }, { "o1_color", 0.7f },
            { "o2_type", 7 }, { "o2_shape", 0.5f }, { "o2_color", 0.5f }, { "o2_level", 0.4f }, { "o2_octave", 1 },
            { "filter_type", 1 }, { "cutoff", 2500 }, { "reso", 0.25f }, { "fenv_amt", 0 },
            { "m1_src", 2 }, { "m1_dst", 5 }, { "m1_amt", 0.25f }, { "lfo2_wave", 6 }, { "lfo2_rate", 0.1f },
            { "amp_attack", 1.5f }, { "amp_release", 3.0f }, { "spread", 0.7f },
            { "cloud_mix", 0.35f }, { "cloud_size", 0.6f }, { "cloud_density", 0.5f }, { "cloud_spray", 0.4f },
            { "reverb_mix", 0.4f }, { "reverb_shimmer", 0.3f }, { "reverb_size", 0.8f } } },

        { "Pad - Pyrate Glow", {
            { "o1_type", 10 }, { "o1_shape", 0.4f }, { "o1_color", 0.2f },
            { "o2_type", 9 }, { "o2_level", 0.5f }, { "o2_octave", -1 }, { "o2_shape", 0.4f }, { "o2_color", 0.5f },
            { "o3_type", 11 }, { "o3_octave", 1 }, { "o3_level", 0.25f },
            { "filter_type", 0 }, { "cutoff", 3000 }, { "fenv_amt", 0.1f },
            { "amp_attack", 1.0f }, { "amp_release", 3.0f },
            { "orbit_rate", 0.2f }, { "orbit_depth", 0.6f },
            { "chorus_mix", 0.4f }, { "reverb_mix", 0.45f }, { "reverb_size", 0.85f }, { "width", 1.6f } } },

        { "Pad - Frozen Choir", {
            { "o1_type", 5 }, { "o1_shape", 0.3f }, { "o1_color", 0.3f },
            { "o2_type", 5 }, { "o2_shape", 0.55f }, { "o2_color", 0.2f }, { "o2_level", 0.6f }, { "o2_fine", 7 },
            { "filter_type", 6 }, { "cutoff", 800 }, { "reso", 0.4f }, { "fenv_amt", 0 }, { "keytrack", 0 },
            { "m1_src", 1 }, { "m1_dst", 5 }, { "m1_amt", 0.12f }, { "lfo1_wave", 1 }, { "lfo1_rate", 0.15f },
            { "amp_attack", 1.8f }, { "amp_release", 3.0f },
            { "reverb_mix", 0.5f }, { "reverb_shimmer", 0.45f }, { "reverb_size", 0.85f } } },

        { "Pad - Tape Memories", {
            { "o1_type", 0 }, { "o1_shape", 0.4f }, { "o1_color", 0.6f },
            { "o2_type", 0 }, { "o2_fine", -9 }, { "o2_level", 0.6f },
            { "filter_type", 0 }, { "cutoff", 1500 }, { "fenv_amt", 0.1f },
            { "amp_attack", 0.9f }, { "amp_release", 2.0f },
            { "tape_wow", 0.6f }, { "tape_flutter", 0.3f }, { "tape_age", 0.6f },
            { "chorus_mix", 0.2f }, { "reverb_mix", 0.35f } } },

        { "Pad - Shimmer Cathedral", {
            { "o1_type", 11 }, { "o1_shape", 0.4f }, { "o1_color", 0.3f },
            { "o2_type", 11 }, { "o2_octave", 1 }, { "o2_level", 0.4f }, { "o2_shape", 0.6f },
            { "filter_type", 0 }, { "cutoff", 6000 }, { "fenv_amt", 0 },
            { "amp_attack", 2.0f }, { "amp_release", 4.0f },
            { "reverb_size", 0.95f }, { "reverb_mix", 0.5f }, { "reverb_shimmer", 0.7f } } },

        { "Pad - Granular Ocean", {
            { "o1_type", 6 }, { "o1_shape", 0.9f }, { "o1_color", 0.85f }, { "o1_level", 0.4f },
            { "o2_type", 9 }, { "o2_level", 0.6f }, { "o2_shape", 0.6f },
            { "filter_type", 2 }, { "cutoff", 1200 }, { "reso", 0.3f }, { "fenv_amt", 0 },
            { "m1_src", 2 }, { "m1_dst", 5 }, { "m1_amt", 0.35f }, { "lfo2_wave", 0 }, { "lfo2_rate", 0.07f },
            { "amp_attack", 2.0f }, { "amp_release", 4.0f },
            { "cloud_mix", 0.6f }, { "cloud_density", 0.7f }, { "cloud_size", 0.8f }, { "cloud_spray", 0.6f },
            { "reverb_mix", 0.5f }, { "reverb_size", 0.85f } } },

        { "Pad - Trance Gate", {
            { "o1_type", 9 }, { "o1_shape", 0.45f }, { "o1_color", 0.8f },
            { "filter_type", 1 }, { "cutoff", 3500 }, { "fenv_amt", 0 },
            { "amp_attack", 0.3f }, { "amp_release", 1.5f },
            { "gate_depth", 1.0f }, { "gate_pattern", 1 }, { "gate_rate", 1 },
            { "delay_sync", 3 }, { "delay_mix", 0.25f }, { "reverb_mix", 0.3f } } },

        { "Pad - Dark Matter Drone", {
            { "o1_type", 2 }, { "o1_shape", 0.4f }, { "o1_color", 0.2f }, { "o1_octave", -1 },
            { "o2_type", 3 }, { "o2_shape", 0.3f }, { "o2_color", 0.3f }, { "o2_level", 0.4f },
            { "filter_type", 1 }, { "cutoff", 700 }, { "reso", 0.4f }, { "fenv_amt", 0 },
            { "gravity", -0.05f }, { "drift", 0.6f },
            { "m1_src", 2 }, { "m1_dst", 1 }, { "m1_amt", 0.3f }, { "lfo2_wave", 6 }, { "lfo2_rate", 0.2f },
            { "amp_attack", 2.0f }, { "amp_release", 4.0f }, { "reverb_mix", 0.5f }, { "reverb_size", 0.9f } } },

        { "Pad - Analog Strings 1979", {
            { "o1_type", 0 }, { "o1_shape", 0.5f }, { "o1_color", 0.8f },
            { "o2_type", 1 }, { "o2_shape", 0.2f }, { "o2_color", 0.6f }, { "o2_level", 0.4f },
            { "filter_type", 0 }, { "cutoff", 2200 }, { "fenv_amt", 0.2f },
            { "amp_attack", 0.6f }, { "amp_release", 1.8f },
            { "chorus_mix", 0.6f }, { "chorus_depth", 0.8f }, { "chorus_rate", 0.5f }, { "reverb_mix", 0.25f } } },

        { "Pad - Orbiting Halo", {
            { "o1_type", 7 }, { "o1_shape", 0.3f }, { "o1_color", 0.4f }, { "o1_pan", -1 },
            { "o2_type", 11 }, { "o2_shape", 0.5f }, { "o2_level", 0.6f }, { "o2_octave", 1 }, { "o2_pan", 1 },
            { "o3_type", 9 }, { "o3_level", 0.4f }, { "o3_octave", -1 },
            { "orbit_rate", 0.15f }, { "orbit_depth", 1.0f },
            { "filter_type", 0 }, { "cutoff", 4000 }, { "amp_attack", 1.2f }, { "amp_release", 3.5f },
            { "reverb_mix", 0.45f }, { "reverb_size", 0.85f }, { "haas", 12 } } },

        //=== Leads ============================================================
        { "Lead - Classic Mono", {
            { "o1_type", 0 }, { "o1_shape", 0.3f }, { "o1_color", 0.5f },
            { "o2_type", 0 }, { "o2_fine", 7 }, { "o2_level", 0.5f },
            { "filter_type", 1 }, { "cutoff", 2500 }, { "reso", 0.3f }, { "fenv_amt", 0.45f },
            { "mode", 0 }, { "glide", 0.06f },
            { "m1_src", 1 }, { "m1_dst", 0 }, { "m1_amt", 0.0125f }, { "lfo1_rate", 5.5f },
            { "delay_sync", 3 }, { "delay_mix", 0.2f }, { "reverb_mix", 0.15f } } },

        { "Lead - Sync Siren", {
            { "o1_type", 3 }, { "o1_shape", 0.5f }, { "o1_color", 0.2f },
            { "m1_src", 3 }, { "m1_dst", 1 }, { "m1_amt", 0.5f }, { "menv_decay", 0.6f },
            { "filter_type", 1 }, { "cutoff", 6000 }, { "fenv_amt", 0.1f },
            { "dist_type", 0 }, { "dist_amt", 0.4f }, { "mode", 1 }, { "glide", 0.08f }, { "delay_mix", 0.25f } } },

        { "Lead - Theremin Ghost", {
            { "o1_type", 2 }, { "o1_shape", 0.03f }, { "o1_color", 0.0f },
            { "mode", 1 }, { "glide", 0.3f },
            { "m1_src", 1 }, { "m1_dst", 0 }, { "m1_amt", 0.02f }, { "lfo1_rate", 5.5f },
            { "filter_type", 0 }, { "cutoff", 2000 }, { "fenv_amt", 0 },
            { "reverb_mix", 0.45f }, { "chorus_mix", 0.3f } } },

        { "Lead - Bitcrushed Hero", {
            { "o1_type", 8 }, { "o1_shape", 0.7f }, { "o1_color", 0.6f },
            { "o2_type", 1 }, { "o2_level", 0.4f }, { "o2_octave", -1 },
            { "mode", 0 }, { "glide", 0.04f }, { "dist_type", 3 }, { "dist_amt", 0.35f },
            { "filter_type", 0 }, { "cutoff", 9000 }, { "delay_sync", 4 }, { "delay_mix", 0.2f } } },

        { "Lead - Talking Formant", {
            { "o1_type", 5 }, { "o1_shape", 0.4f }, { "o1_color", 0.5f },
            { "o2_type", 0 }, { "o2_level", 0.3f },
            { "filter_type", 6 }, { "cutoff", 1200 }, { "reso", 0.5f }, { "fenv_amt", 0.3f }, { "keytrack", 0 },
            { "m1_src", 5 }, { "m1_dst", 5 }, { "m1_amt", 0.4f },
            { "mode", 1 }, { "glide", 0.1f }, { "chorus_mix", 0.35f } } },

        { "Lead - Screaming Fold", {
            { "o1_type", 2 }, { "o1_shape", 0.6f }, { "o1_color", 0.4f },
            { "m1_src", 4 }, { "m1_dst", 1 }, { "m1_amt", 0.4f },
            { "m2_src", 1 }, { "m2_dst", 1 }, { "m2_amt", 0.2f }, { "lfo1_rate", 0.3f },
            { "filter_type", 1 }, { "cutoff", 5000 }, { "dist_type", 0 }, { "dist_amt", 0.5f }, { "mode", 0 } } },

        { "Lead - Gravity Well", {
            { "o1_type", 0 }, { "o1_shape", 0.5f }, { "o1_color", 0.6f },
            { "gravity", -0.25f }, { "entropy", 0.3f },
            { "filter_type", 1 }, { "cutoff", 3000 }, { "mode", 1 },
            { "delay_mix", 0.35f }, { "delay_fb", 0.5f }, { "reverb_mix", 0.25f } } },

        { "Lead - Alien Whistle", {
            { "o1_type", 4 }, { "o1_shape", 0.4f }, { "o1_color", 0.9f },
            { "o2_type", 7 }, { "o2_level", 0.6f }, { "o2_semi", 7 },
            { "ring", 0.5f },
            { "m1_src", 1 }, { "m1_dst", 0 }, { "m1_amt", 0.025f }, { "lfo1_wave", 5 }, { "lfo1_rate", 7.0f },
            { "filter_type", 0 }, { "cutoff", 8000 }, { "mode", 0 }, { "reverb_mix", 0.4f } } },

        { "Lead - Cross FM Snarl", {
            { "o1_type", 0 }, { "o1_shape", 0.2f },
            { "o3_type", 7 }, { "o3_level", 0.0f }, { "o3_octave", 1 }, { "o3_semi", 7 },
            { "xfm", 0.35f },
            { "m1_src", 3 }, { "m1_dst", 9 }, { "m1_amt", 0.4f }, { "menv_decay", 0.4f },
            { "filter_type", 1 }, { "cutoff", 3500 }, { "reso", 0.35f }, { "mode", 0 }, { "glide", 0.05f },
            { "dist_type", 0 }, { "dist_amt", 0.3f }, { "delay_sync", 3 }, { "delay_mix", 0.2f } } },

        //=== Bass =============================================================
        { "Bass - Rubber", {
            { "o1_type", 1 }, { "o1_shape", 0.0f }, { "o1_color", 0.0f },
            { "o3_type", 2 }, { "o3_shape", 0.0f }, { "o3_color", 0.0f }, { "o3_octave", -1 }, { "o3_level", 0.6f },
            { "filter_type", 1 }, { "cutoff", 400 }, { "reso", 0.45f }, { "fenv_amt", 0.65f },
            { "flt_decay", 0.22f }, { "flt_sustain", 0.1f },
            { "mode", 0 }, { "glide", 0.04f }, { "dist_type", 0 }, { "dist_amt", 0.25f }, { "reverb_mix", 0.0f } } },

        { "Bass - Wobble", {
            { "o1_type", 0 }, { "o1_octave", -1 },
            { "o2_type", 9 }, { "o2_octave", -1 }, { "o2_level", 0.5f },
            { "filter_type", 1 }, { "cutoff", 600 }, { "reso", 0.5f }, { "fenv_amt", 0 },
            { "m1_src", 1 }, { "m1_dst", 5 }, { "m1_amt", 0.45f }, { "lfo1_rate", 3.0f },
            { "mode", 0 }, { "dist_type", 0 }, { "dist_amt", 0.35f }, { "reverb_mix", 0.05f } } },

        { "Bass - FM Growl", {
            { "o1_type", 4 }, { "o1_shape", 0.5f }, { "o1_color", 0.273f }, { "o1_octave", -1 },
            { "m1_src", 3 }, { "m1_dst", 1 }, { "m1_amt", 0.4f }, { "menv_decay", 0.3f },
            { "filter_type", 1 }, { "cutoff", 2000 }, { "fenv_amt", 0.2f },
            { "mode", 0 }, { "dist_type", 2 }, { "dist_amt", 0.2f }, { "reverb_mix", 0.0f } } },

        { "Bass - Plucked String", {
            { "o1_type", 12 }, { "o1_shape", 0.4f }, { "o1_color", 0.9f }, { "o1_octave", -1 },
            { "o2_type", 1 }, { "o2_octave", -2 }, { "o2_level", 0.3f }, { "o2_shape", 0.0f },
            { "filter_type", 0 }, { "cutoff", 2500 }, { "fenv_amt", 0 }, { "mode", 0 }, { "reverb_mix", 0.05f } } },

        { "Bass - Acid Comb", {
            { "o1_type", 0 }, { "o1_shape", 0.0f }, { "o1_color", 0.0f },
            { "filter_type", 5 }, { "cutoff", 300 }, { "reso", 0.7f }, { "keytrack", 1.0f }, { "fenv_amt", 0.6f },
            { "flt_decay", 0.25f }, { "flt_sustain", 0.0f },
            { "dist_type", 0 }, { "dist_amt", 0.4f }, { "mode", 0 }, { "glide", 0.08f },
            { "delay_sync", 4 }, { "delay_mix", 0.2f } } },

        //=== Plucks & Bells ===================================================
        { "Pluck - Harp", {
            { "o1_type", 12 }, { "o1_shape", 0.6f }, { "o1_color", 0.85f },
            { "filter_type", 0 }, { "cutoff", 8000 }, { "fenv_amt", 0 },
            { "amp_attack", 0.001f }, { "amp_sustain", 1.0f }, { "amp_release", 1.2f },
            { "reverb_mix", 0.35f }, { "delay_sync", 3 }, { "delay_mix", 0.2f }, { "chorus_mix", 0.15f } } },

        { "Pluck - Koto Garden", {
            { "o1_type", 12 }, { "o1_shape", 0.75f }, { "o1_color", 0.7f },
            { "o2_type", 12 }, { "o2_shape", 0.5f }, { "o2_color", 0.6f }, { "o2_semi", 7 }, { "o2_level", 0.3f },
            { "filter_type", 0 }, { "cutoff", 7000 }, { "fenv_amt", 0 },
            { "amp_attack", 0.001f }, { "amp_sustain", 1.0f }, { "amp_release", 0.9f }, { "reverb_mix", 0.3f } } },

        { "Bell - Tubular", {
            { "o1_type", 4 }, { "o1_shape", 0.55f }, { "o1_color", 0.545f },
            { "o2_type", 4 }, { "o2_shape", 0.3f }, { "o2_color", 0.909f }, { "o2_level", 0.3f },
            { "filter_type", 0 }, { "cutoff", 12000 }, { "fenv_amt", 0 },
            { "amp_attack", 0.001f }, { "amp_decay", 3.0f }, { "amp_sustain", 0.0f }, { "amp_release", 2.5f },
            { "reverb_mix", 0.4f }, { "reverb_size", 0.8f } } },

        { "Pluck - Crystal", {
            { "o1_type", 11 }, { "o1_shape", 0.9f }, { "o1_color", 0.6f },
            { "filter_type", 1 }, { "cutoff", 1200 }, { "fenv_amt", 0.7f }, { "flt_decay", 0.3f }, { "flt_sustain", 0.0f },
            { "amp_decay", 0.5f }, { "amp_sustain", 0.0f }, { "amp_release", 0.4f },
            { "delay_sync", 3 }, { "delay_mix", 0.35f }, { "delay_fb", 0.5f }, { "reverb_mix", 0.25f } } },

        //=== Arpeggios ========================================================
        { "Arp - Pinky Run", {
            { "o1_type", 1 }, { "o1_shape", 0.3f }, { "o1_color", 0.3f },
            { "filter_type", 1 }, { "cutoff", 2500 }, { "fenv_amt", 0.5f }, { "flt_decay", 0.2f }, { "flt_sustain", 0.0f },
            { "amp_decay", 0.25f }, { "amp_sustain", 0.0f }, { "amp_release", 0.2f },
            { "arp_on", 1 }, { "arp_mode", 7 }, { "arp_rate", 3 }, { "arp_octaves", 2 }, { "arp_gate", 0.5f },
            { "delay_sync", 3 }, { "delay_mix", 0.25f } } },

        { "Arp - Octave Bounce Cinquillo", {
            { "o1_type", 0 }, { "o1_shape", 0.3f },
            { "filter_type", 1 }, { "cutoff", 1800 }, { "fenv_amt", 0.55f }, { "flt_decay", 0.18f }, { "flt_sustain", 0.0f },
            { "amp_decay", 0.3f }, { "amp_sustain", 0.0f }, { "amp_release", 0.2f },
            { "arp_on", 1 }, { "arp_mode", 9 }, { "arp_rate", 3 }, { "arp_rhythm", 2 }, { "arp_gate", 0.4f },
            { "delay_sync", 3 }, { "delay_mix", 0.3f }, { "reverb_mix", 0.2f } } },

        { "Arp - Euclid Machine", {
            { "o1_type", 8 }, { "o1_shape", 0.4f },
            { "filter_type", 0 }, { "cutoff", 6000 },
            { "amp_decay", 0.2f }, { "amp_sustain", 0.0f }, { "amp_release", 0.1f },
            { "arp_on", 1 }, { "arp_mode", 10 }, { "arp_rate", 3 }, { "arp_rhythm", 4 }, { "arp_octaves", 2 },
            { "arp_ratchet", 0.3f }, { "arp_jump", 0.2f },
            { "dist_type", 3 }, { "dist_amt", 0.2f }, { "delay_sync", 4 }, { "delay_mix", 0.2f } } },

        { "Arp - Converging Bells", {
            { "o1_type", 4 }, { "o1_shape", 0.45f }, { "o1_color", 0.545f },
            { "filter_type", 0 }, { "cutoff", 10000 }, { "fenv_amt", 0 },
            { "amp_attack", 0.001f }, { "amp_decay", 1.5f }, { "amp_sustain", 0.0f }, { "amp_release", 1.2f },
            { "arp_on", 1 }, { "arp_mode", 5 }, { "arp_rate", 1 }, { "arp_rhythm", 7 }, { "arp_octaves", 2 },
            { "reverb_mix", 0.4f }, { "delay_sync", 3 }, { "delay_mix", 0.2f } } },

        { "Arp - Chord Stabber", {
            { "o1_type", 9 }, { "o1_shape", 0.4f }, { "o1_color", 0.6f },
            { "filter_type", 1 }, { "cutoff", 3000 }, { "fenv_amt", 0.5f }, { "flt_decay", 0.15f }, { "flt_sustain", 0.0f },
            { "amp_decay", 0.2f }, { "amp_sustain", 0.0f }, { "amp_release", 0.15f },
            { "arp_on", 1 }, { "arp_mode", 11 }, { "arp_rate", 1 }, { "arp_rhythm", 5 }, { "arp_gate", 0.3f },
            { "reverb_mix", 0.25f }, { "delay_sync", 2 }, { "delay_mix", 0.2f } } },

        { "Arp - Swinging Thumb", {
            { "o1_type", 7 }, { "o1_shape", 0.6f }, { "o1_color", 0.4f },
            { "filter_type", 1 }, { "cutoff", 2200 }, { "fenv_amt", 0.4f }, { "flt_decay", 0.2f }, { "flt_sustain", 0.1f },
            { "amp_decay", 0.3f }, { "amp_sustain", 0.0f }, { "amp_release", 0.2f },
            { "arp_on", 1 }, { "arp_mode", 8 }, { "arp_rate", 3 }, { "arp_swing", 0.5f }, { "arp_gate", 0.5f },
            { "chorus_mix", 0.3f }, { "reverb_mix", 0.2f } } },

        //=== Weird ============================================================
        { "FX - Broken Satellite", {
            { "o1_type", 6 }, { "o1_shape", 0.2f }, { "o1_color", 0.3f },
            { "o2_type", 4 }, { "o2_shape", 0.7f }, { "o2_level", 0.5f },
            { "filter_type", 5 }, { "cutoff", 300 }, { "keytrack", 1.0f }, { "reso", 0.8f }, { "fenv_amt", 0 },
            { "m1_src", 1 }, { "m1_dst", 5 }, { "m1_amt", 0.3f }, { "lfo1_wave", 5 }, { "lfo1_rate", 6.0f },
            { "entropy", 0.7f }, { "cloud_mix", 0.4f }, { "dist_type", 3 }, { "dist_amt", 0.3f } } },

        { "FX - Insect Orchestra", {
            { "o1_type", 5 }, { "o1_shape", 0.8f }, { "o1_color", 0.9f },
            { "o2_type", 8 }, { "o2_level", 0.4f }, { "o2_octave", 1 },
            { "m1_src", 1 }, { "m1_dst", 0 }, { "m1_amt", 0.1f }, { "lfo1_wave", 4 }, { "lfo1_rate", 12.0f },
            { "orbit_rate", 4.0f }, { "orbit_depth", 1.0f }, { "spread", 1.0f },
            { "gate_depth", 0.8f }, { "gate_pattern", 3 }, { "filter_type", 3 }, { "cutoff", 400 }, { "reverb_mix", 0.3f } } },

        { "FX - Melting Clock", {
            { "o1_type", 0 }, { "o1_shape", 0.3f },
            { "o2_type", 11 }, { "o2_level", 0.5f }, { "o2_octave", 1 },
            { "gravity", -0.15f }, { "drift", 0.8f },
            { "amp_attack", 0.3f }, { "amp_release", 2.5f },
            { "tape_wow", 1.0f }, { "tape_flutter", 0.5f }, { "tape_age", 0.5f }, { "reverb_mix", 0.4f } } },

        { "FX - Rising Phoenix", {
            { "o1_type", 9 }, { "o1_shape", 0.6f }, { "o1_color", 0.8f },
            { "gravity", 0.3f },
            { "filter_type", 1 }, { "cutoff", 1500 }, { "fenv_amt", 0.6f }, { "flt_attack", 3.0f }, { "flt_sustain", 1.0f },
            { "amp_attack", 1.0f }, { "amp_release", 4.0f },
            { "reverb_mix", 0.5f }, { "reverb_shimmer", 0.8f }, { "reverb_size", 0.9f } } },

        { "FX - Ring Robot", {
            { "o1_type", 1 }, { "o1_shape", 0.3f },
            { "o2_type", 4 }, { "o2_semi", 5 }, { "o2_level", 0.8f },
            { "ring", 0.9f },
            { "filter_type", 2 }, { "cutoff", 1500 }, { "reso", 0.5f }, { "fenv_amt", 0 },
            { "m1_src", 7 }, { "m1_dst", 5 }, { "m1_amt", 0.3f },
            { "dist_type", 4 }, { "dist_amt", 0.5f }, { "mode", 0 } } },

        { "FX - Entropy Choir", {
            { "o1_type", 5 }, { "o1_shape", 0.35f }, { "o1_color", 0.4f },
            { "o2_type", 5 }, { "o2_shape", 0.6f }, { "o2_level", 0.5f },
            { "entropy", 1.0f }, { "spread", 1.0f },
            { "filter_type", 6 }, { "cutoff", 1000 }, { "reso", 0.4f }, { "keytrack", 0 },
            { "m1_src", 6 }, { "m1_dst", 5 }, { "m1_amt", 0.3f },
            { "amp_attack", 1.0f }, { "amp_release", 3.0f },
            { "cloud_mix", 0.4f }, { "cloud_spray", 0.7f }, { "reverb_mix", 0.45f } } },

        { "FX - Laser Zap", {
            { "o1_type", 0 }, { "o1_shape", 0.2f },
            { "m1_src", 3 }, { "m1_dst", 0 }, { "m1_amt", 0.8f },
            { "menv_attack", 0.001f }, { "menv_decay", 0.15f },
            { "filter_type", 1 }, { "cutoff", 6000 }, { "mode", 0 },
            { "delay_mix", 0.4f }, { "delay_fb", 0.55f }, { "delay_time", 0.25f } } },

        { "FX - Looping Machine Heart", {
            { "o1_type", 3 }, { "o1_shape", 0.2f },
            { "o2_type", 6 }, { "o2_level", 0.3f }, { "o2_shape", 0.6f },
            { "menv_mode", 1 }, { "menv_attack", 0.02f }, { "menv_decay", 0.35f },
            { "m1_src", 3 }, { "m1_dst", 1 }, { "m1_amt", 0.7f },
            { "m2_src", 3 }, { "m2_dst", 5 }, { "m2_amt", 0.3f },
            { "filter_type", 1 }, { "cutoff", 1500 }, { "reso", 0.4f },
            { "phaser_mix", 0.5f }, { "reverb_mix", 0.3f }, { "pan_depth", 0.5f }, { "pan_rate", 0.5f } } },
    };

    return presets;
}
