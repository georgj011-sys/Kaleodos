#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>
#include <vector>
#include "Osc.h"

struct VoiceParams
{
    struct Osc
    {
        int type = 0;
        float shape = 0.25f, color = 0.4f, octave = 0.0f, semi = 0.0f, fine = 0.0f, level = 0.8f, pan = 0.0f;
    };

    struct Slot { int src = 0, dst = 0; float amt = 0.0f; };

    Osc osc[3];
    float xfm = 0.0f, ring = 0.0f;

    int filterType = 1;
    float cutoff = 4000.0f, reso = 0.2f, envAmt = 0.3f, keytrack = 0.5f, drive = 0.1f;

    juce::ADSR::Parameters ampEnv { 0.005f, 0.4f, 0.8f, 0.35f };
    juce::ADSR::Parameters fltEnv { 0.005f, 0.5f, 0.4f, 0.4f };
    float velSens = 0.5f;

    float modAttack = 0.01f, modDecay = 0.5f;
    bool modLoop = false;

    int lfoWave[2] { 0, 1 };
    float lfoRate[2] { 5.0f, 0.3f };

    Slot slots[4];

    float gravity = 0.0f, entropy = 0.0f, drift = 0.1f, orbitRate = 0.3f, orbitDepth = 0.0f, spread = 0.3f;
    float glide = 0.0f;
};

class KaleidoVoice
{
public:
    static constexpr int chunk = 16;
    enum FilterType { LP12 = 0, LP24, BP, HP, Notch, Comb, FormantF };
    enum Dest { DPitch = 0, DShape1, DShape2, DShape3, DColors, DCutoff, DReso, DPan, DVolume, DXfm, DRing, DLfoRate, numDests };

    void prepare (double sampleRate)
    {
        sr = sampleRate;
        ampEnv.setSampleRate (sampleRate);
        fltEnv.setSampleRate (sampleRate);
        for (auto& o : osc)
            o.prepare (sampleRate);

        combSize = (int) (sampleRate / 15.0) + 4;
        combL.assign ((size_t) combSize, 0.0f);
        combR.assign ((size_t) combSize, 0.0f);
        combW = 0;

        resetFilters();
        kill();
    }

    void start (int note, float vel, float fromPitch, const VoiceParams& p, uint32_t seed, float spreadPosition)
    {
        if (! isActive())
        {
            for (int i = 0; i < 3; ++i)
                osc[i].resetPhases (seed * (uint32_t) (i * 7 + 1) + (uint32_t) i);
            resetFilters();
            orbitPhase = rng.nextFloat();
            lfoPhase[1] = rng.nextFloat();
            for (auto& d : driftVal) d = 0.0f;
        }

        for (int i = 0; i < 3; ++i)
            if (p.osc[i].type == KaleidoOsc::String)
                osc[i].trigger (p.osc[i].shape);

        targetPitch = (float) note;
        currentPitch = fromPitch;
        currentNote = note;
        velocity = vel;
        spreadPos = spreadPosition;
        gate = true;

        lfoPhase[0] = 0.0f;
        modEnvVal = 0.0f;
        modEnvStage = 0;
        gravityAcc = 0.0f;

        noteRand = rng.nextFloat() * 2.0f - 1.0f;
        const float e = p.entropy;
        ePitch = (rng.nextFloat() * 2.0f - 1.0f) * e * 0.6f;
        eCut   = (rng.nextFloat() * 2.0f - 1.0f) * e * 2.0f;
        ePan   = (rng.nextFloat() * 2.0f - 1.0f) * e;
        for (auto& s : eShape)
            s = (rng.nextFloat() * 2.0f - 1.0f) * e * 0.4f;

        ampEnv.setParameters (p.ampEnv);
        fltEnv.setParameters (p.fltEnv);
        ampEnv.noteOn();
        fltEnv.noteOn();
    }

    void glideTo (int note) noexcept { targetPitch = (float) note; currentNote = note; }

    void release()
    {
        gate = false;
        ampEnv.noteOff();
        fltEnv.noteOff();
    }

    void kill()
    {
        ampEnv.reset();
        fltEnv.reset();
        gate = false;
        currentNote = -1;
    }

    bool isActive() const noexcept  { return ampEnv.isActive(); }
    bool isGateOn() const noexcept  { return gate; }
    int getNote() const noexcept    { return currentNote; }
    float getPitch() const noexcept { return currentPitch; }

    juce::uint64 age = 0;

    void render (float* L, float* R, int start, int num, const VoiceParams& p, float bend, float modWheel)
    {
        if (! isActive() || num <= 0)
            return;

        ampEnv.setParameters (p.ampEnv);
        fltEnv.setParameters (p.fltEnv);

        const float fsr = (float) sr;
        const float twoPi = juce::MathConstants<float>::twoPi;
        const float glideCoeff = p.glide <= 0.0005f ? 1.0f : 1.0f - std::exp (-(float) chunk / (p.glide * fsr));
        const float velGain = 1.0f - p.velSens * (1.0f - velocity);
        const float drv = 1.0f + p.drive * 8.0f;
        const float drvComp = 1.0f / (0.7f * std::sqrt (drv));
        const float driftAmt = p.drift * p.drift * 0.35f;

        while (num > 0)
        {
            const int n = juce::jmin (chunk, num);
            const float dtChunk = (float) n / fsr;

            currentPitch += (targetPitch - currentPitch) * glideCoeff;

            // Modulation sources
            const float lfo1 = tickLfo (0, p.lfoWave[0], p.lfoRate[0] * std::exp2 (lfoRateMod * 3.0f), dtChunk);
            const float lfo2 = tickLfo (1, p.lfoWave[1], p.lfoRate[1], dtChunk);
            const float menv = tickModEnv (p, dtChunk);

            const float src[8] = { 0.0f, lfo1, lfo2, menv, velocity, modWheel, noteRand,
                                   (currentPitch - 60.0f) / 24.0f };

            float d[numDests] = {};
            for (const auto& s : p.slots)
                if (s.src > 0 && s.src < 8 && s.dst >= 0 && s.dst < numDests && s.amt != 0.0f)
                    d[s.dst] += src[s.src] * s.amt;

            lfoRateMod = d[DLfoRate];

            // Strange physics
            gravityAcc = juce::jlimit (-48.0f, 48.0f, gravityAcc + p.gravity * 12.0f * dtChunk);

            for (int o = 0; o < 3; ++o)
            {
                if (rng.nextFloat() < dtChunk * 2.0f)
                    driftTarget[o] = rng.nextFloat() * 2.0f - 1.0f;
                driftVal[o] += (driftTarget[o] - driftVal[o]) * juce::jmin (1.0f, dtChunk * 1.5f);
            }

            orbitPhase += p.orbitRate * dtChunk;
            orbitPhase -= std::floor (orbitPhase);

            const float basePitch = currentPitch + bend + d[DPitch] * 24.0f + gravityAcc + ePitch;

            float dt[3], shp[3], col[3], gL[3], gR[3];
            for (int o = 0; o < 3; ++o)
            {
                const auto& op = p.osc[o];
                const float semis = basePitch + op.octave * 12.0f + op.semi + op.fine * 0.01f + driftVal[o] * driftAmt;
                dt[o]  = 440.0f * std::exp2 ((semis - 69.0f) / 12.0f) / fsr;
                shp[o] = juce::jlimit (0.0f, 1.0f, op.shape + eShape[o] + d[DShape1 + o] * 0.5f);
                col[o] = juce::jlimit (0.0f, 1.0f, op.color + d[DColors] * 0.5f);

                const float orbit = std::sin (twoPi * (orbitPhase + (float) o / 3.0f));
                const float pan = juce::jlimit (-1.0f, 1.0f, op.pan * (1.0f - p.orbitDepth) + p.orbitDepth * orbit);
                gL[o] = std::sqrt (0.5f * (1.0f - pan)) * op.level;
                gR[o] = std::sqrt (0.5f * (1.0f + pan)) * op.level;
            }

            const float xfm  = juce::jlimit (0.0f, 1.0f, p.xfm + d[DXfm]);
            const float ring = juce::jlimit (0.0f, 1.0f, p.ring + d[DRing]);
            const float ringLevel = 0.7f * juce::jmax (p.osc[0].level, p.osc[1].level);

            const float vpan = juce::jlimit (-1.0f, 1.0f, spreadPos * p.spread + ePan + d[DPan]);
            const float vgL = vpan > 0.0f ? 1.0f - vpan : 1.0f;
            const float vgR = vpan < 0.0f ? 1.0f + vpan : 1.0f;
            const float volume = juce::jlimit (0.0f, 2.0f, 1.0f + d[DVolume]);

            // Filter
            const float fe = fltEnv.getNextSample();
            for (int k = 1; k < n; ++k)
                fltEnv.getNextSample();

            float cutoff = p.cutoff * std::exp2 (p.envAmt * fe * 7.0f + (basePitch - 60.0f) / 12.0f * p.keytrack
                                                 + d[DCutoff] * 5.0f + eCut);
            cutoff = juce::jlimit (20.0f, fsr * 0.45f, cutoff);
            const float reso = juce::jlimit (0.0f, 1.0f, p.reso + d[DReso] * 0.5f);

            Coeffs main = makeCoeffs (cutoff, 1.414f - 1.38f * reso);
            Coeffs f1, f2;
            if (p.filterType == LP24)
                f1 = makeCoeffs (cutoff, 1.414f);
            if (p.filterType == FormantF)
            {
                static constexpr float F1[5] = { 730.0f, 530.0f, 270.0f, 570.0f, 300.0f };
                static constexpr float F2[5] = { 1090.0f, 1840.0f, 2290.0f, 840.0f, 870.0f };
                const float v = juce::jlimit (0.0f, 1.0f, std::log2 (cutoff / 80.0f) / 7.0f) * 4.0f;
                const int i0 = juce::jmin (3, (int) v);
                const float fr = v - (float) i0;
                const float k = 0.5f - 0.42f * reso;
                f1 = makeCoeffs (F1[i0] + fr * (F1[i0 + 1] - F1[i0]), k);
                f2 = makeCoeffs (F2[i0] + fr * (F2[i0 + 1] - F2[i0]), k);
            }

            const float combDelay = juce::jlimit (2.0f, (float) combSize - 3.0f, fsr / cutoff);
            const float combFb = reso * 0.97f;

            for (int i = 0; i < n; ++i)
            {
                const float fmDt = dt[0] * juce::jmax (0.0f, 1.0f + xfm * 4.0f * lastOsc3);
                const float a = osc[0].process (p.osc[0].type, fmDt, shp[0], col[0]);
                const float b = osc[1].process (p.osc[1].type, dt[1], shp[1], col[1]);
                const float c = osc[2].process (p.osc[2].type, dt[2], shp[2], col[2]);
                lastOsc3 = c;

                const float rm = a * b * 1.4f * ringLevel * ring;
                float l = (a * gL[0] + b * gL[1]) * (1.0f - ring) + rm + c * gL[2];
                float r = (a * gR[0] + b * gR[1]) * (1.0f - ring) + rm + c * gR[2];

                const float yl = l - dcxL + 0.995f * dcyL; dcxL = l; dcyL = yl;
                const float yr = r - dcxR + 0.995f * dcyR; dcxR = r; dcyR = yr;

                l = std::tanh (yl * 0.7f * drv) * drvComp;
                r = std::tanh (yr * 0.7f * drv) * drvComp;

                l = filterSample (l, 0, main, f1, f2, p.filterType, combDelay, combFb);
                r = filterSample (r, 1, main, f1, f2, p.filterType, combDelay, combFb);
                if (p.filterType == Comb)
                    combW = (combW + 1) % combSize;

                const float amp = ampEnv.getNextSample() * velGain * volume * 0.45f;
                L[start + i] += l * amp * vgL;
                R[start + i] += r * amp * vgR;
            }

            start += n;
            num -= n;

            if (! ampEnv.isActive())
            {
                currentNote = -1;
                gate = false;
                break;
            }
        }
    }

private:
    struct SvfState { float ic1 = 0.0f, ic2 = 0.0f; };
    struct Coeffs   { float a1 = 0.0f, a2 = 0.0f, a3 = 0.0f, k = 1.0f; };

    Coeffs makeCoeffs (float freq, float k) const noexcept
    {
        Coeffs c;
        const float g = std::tan (juce::MathConstants<float>::pi * juce::jlimit (20.0f, (float) sr * 0.45f, freq) / (float) sr);
        c.k = k;
        c.a1 = 1.0f / (1.0f + g * (g + k));
        c.a2 = g * c.a1;
        c.a3 = g * c.a2;
        return c;
    }

    static void svf (SvfState& s, float x, const Coeffs& c, float& lp, float& bp, float& hp) noexcept
    {
        const float v3 = x - s.ic2;
        const float v1 = c.a1 * s.ic1 + c.a2 * v3;
        const float v2 = s.ic2 + c.a2 * s.ic1 + c.a3 * v3;
        s.ic1 = 2.0f * v1 - s.ic1;
        s.ic2 = 2.0f * v2 - s.ic2;
        lp = v2;
        bp = v1;
        hp = x - c.k * v1 - v2;
    }

    float filterSample (float x, int ch, const Coeffs& m, const Coeffs& f1, const Coeffs& f2,
                        int type, float combDelay, float combFb) noexcept
    {
        float lp, bp, hp;

        switch (type)
        {
            case Comb:
            {
                auto& buf = ch == 0 ? combL : combR;
                float& damp = ch == 0 ? combLpL : combLpR;
                float pos = (float) combW - combDelay;
                if (pos < 0.0f) pos += (float) combSize;
                int i0 = (int) pos;
                const float fr = pos - (float) i0;
                i0 %= combSize;
                const int i1 = (i0 + 1) % combSize;
                const float delayed = buf[(size_t) i0] + fr * (buf[(size_t) i1] - buf[(size_t) i0]);
                damp += 0.6f * (delayed - damp);
                const float y = x * 0.5f + combFb * damp;
                buf[(size_t) combW] = y;
                return std::tanh (y * (1.0f - combFb * 0.85f) * 1.5f);
            }

            case FormantF:
            {
                float lp2, bp2, hp2;
                svf (svfA[ch], x, f1, lp, bp, hp);
                svf (svfB[ch], x, f2, lp2, bp2, hp2);
                return (bp * f1.k + 0.8f * bp2 * f2.k) * 1.6f;
            }

            case LP24:
            {
                svf (svfA[ch], x, m, lp, bp, hp);
                float lp2, bp2, hp2;
                svf (svfB[ch], lp, f1, lp2, bp2, hp2);
                return lp2;
            }

            default:
                break;
        }

        svf (svfA[ch], x, m, lp, bp, hp);

        switch (type)
        {
            case BP:    return bp * (m.k * 0.75f + 0.5f);
            case HP:    return hp;
            case Notch: return x - m.k * bp;
            default:    return lp;
        }
    }

    float tickLfo (int i, int wave, float rate, float dtChunk) noexcept
    {
        const float ph = lfoPhase[i];
        float val;

        switch (wave)
        {
            case 0:  val = std::sin (juce::MathConstants<float>::twoPi * ph); break;
            case 1:  val = 1.0f - 4.0f * std::abs (ph - 0.5f); break;
            case 2:  val = 2.0f * ph - 1.0f; break;
            case 3:  val = 1.0f - 2.0f * ph; break;
            case 4:  val = ph < 0.5f ? 1.0f : -1.0f; break;
            case 5:  val = lfoHold[i]; break;
            case 6:  val = lfoDrift[i]; break;
            default: val = std::floor ((1.0f - 2.0f * std::abs (ph - 0.5f)) * 4.0f) / 2.0f - 1.0f; break;
        }

        lfoPhase[i] += rate * dtChunk;
        if (lfoPhase[i] >= 1.0f)
        {
            lfoPhase[i] -= std::floor (lfoPhase[i]);
            lfoHold[i] = rng.nextFloat() * 2.0f - 1.0f;
            lfoDriftTarget[i] = rng.nextFloat() * 2.0f - 1.0f;
        }
        lfoDrift[i] += (lfoDriftTarget[i] - lfoDrift[i]) * juce::jmin (1.0f, rate * dtChunk * 3.0f);
        return val;
    }

    float tickModEnv (const VoiceParams& p, float dtChunk) noexcept
    {
        if (modEnvStage == 0)
        {
            modEnvVal += dtChunk / juce::jmax (0.001f, p.modAttack);
            if (modEnvVal >= 1.0f) { modEnvVal = 1.0f; modEnvStage = 1; }
        }
        else if (modEnvStage == 1)
        {
            modEnvVal -= dtChunk / juce::jmax (0.001f, p.modDecay);
            if (modEnvVal <= 0.0f)
            {
                modEnvVal = 0.0f;
                modEnvStage = p.modLoop ? 0 : 2;
            }
        }
        return modEnvVal;
    }

    void resetFilters()
    {
        svfA[0] = svfA[1] = svfB[0] = svfB[1] = SvfState();
        std::fill (combL.begin(), combL.end(), 0.0f);
        std::fill (combR.begin(), combR.end(), 0.0f);
        combLpL = combLpR = 0.0f;
        dcxL = dcyL = dcxR = dcyR = 0.0f;
        lastOsc3 = 0.0f;
    }

    double sr = 44100.0;
    KaleidoOsc osc[3];
    juce::ADSR ampEnv, fltEnv;
    juce::Random rng;

    SvfState svfA[2], svfB[2];
    std::vector<float> combL, combR;
    int combSize = 4, combW = 0;
    float combLpL = 0.0f, combLpR = 0.0f;
    float dcxL = 0.0f, dcyL = 0.0f, dcxR = 0.0f, dcyR = 0.0f;
    float lastOsc3 = 0.0f;

    float targetPitch = 60.0f, currentPitch = 60.0f, velocity = 1.0f;
    int currentNote = -1;
    bool gate = false;
    float spreadPos = 0.0f;

    float lfoPhase[2] {}, lfoHold[2] {}, lfoDrift[2] {}, lfoDriftTarget[2] {};
    float lfoRateMod = 0.0f;
    float modEnvVal = 0.0f;
    int modEnvStage = 2;

    float gravityAcc = 0.0f, orbitPhase = 0.0f;
    float driftVal[3] {}, driftTarget[3] {};
    float noteRand = 0.0f, ePitch = 0.0f, eCut = 0.0f, ePan = 0.0f;
    float eShape[3] {};
};
