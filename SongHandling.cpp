#pragma once


#include "GlobalVariables.h"






void SetUpAudioEngine();

void StartNote(int channel, int sampleNumber, float pitch, int startFrame);

void PlayChannels(float* pOutputF32, ma_uint32 frameCount, ma_uint32 frameOffset);

void updateSongOnBeat();

int updateChannelOnBeat(int ch);

void RecordSong();

void StartOrStopSong();

void DrawSampleDisplay();

void DrawEnvelopeDisplay();

void GenerateAdditiveWave(Instrument* instrument, int op);

void GenerateAllInstrumentWaves(Instrument* instrument);

void ConstructWave(Instrument* instrument, int op, int waveType, float frequencies[16], float framesToWrite, float periodLength, int frequency, float* inputWave);

void DrawSamplePoint(Vector2 drawWavePos);







////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// New Audio

void readModulator(float* pOutputF32, ma_uint64 frameCount, int channel, int waveForm, float* mods[4]);

void applySubtractiveFilters(float* pOutputF32, ma_uint64 frameCount, int channel, float* input, ma_uint32 frameOffset);

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

void readWithFMAlgorithm(float* pOutputF32, ma_uint64 frameCount, int channel, ma_uint32 frameOffset);









// Decoders
ma_decoder_config decoderConfig;


// Device
ma_device_config deviceConfig;
ma_device device;


// Encoder
ma_encoder_config encoderConfig;
ma_encoder encoder;




float wrapReadPos(float readPos, int channel, int wave);


float wrapReadPos(float readPos, int channel, int wave)
{
    int inst = channels[channel].instrument;

    int loopStart = loadedInstruments[inst].waveforms[wave].loopStart;
    int loopEnd = loadedInstruments[inst].waveforms[wave].loopEnd;
    int loopType = loadedInstruments[inst].waveforms[wave].loopType;

    if (loopType == 0) // No loop.
    {
        if (readPos >= loopEnd)
            readPos = loopEnd - 1;
        else if (readPos < loopStart)
            readPos = loopStart;
    }
    else if (loopType == 1) // Forward loop.
    {
        while (readPos < loopStart)
            readPos += (loopEnd - loopStart);
        while (readPos >= loopEnd)
            readPos -= (loopEnd - loopStart);
    }
    else // Bounce loop.
    {
        bool wrapped = false;
        while (!wrapped)
        {
            if (readPos < loopStart)
            {
                readPos -= loopStart;
                readPos *= -1.0f;
                readPos += loopStart;
            }
            else if (readPos >= loopEnd)
            {
                readPos -= loopEnd;
                readPos *= -1.0f;
                readPos += loopEnd;
                readPos--;
            }
            else
                wrapped = true;
        }
    }

    return readPos;
}




void readModulator(float* pOutputF32, ma_uint64 frameCount, int channel, int op, float* mods[4])
{
    int mappedWave = loadedInstruments[channels[channel].instrument].operatorMapping[op];
    // For a mapped wave, all instrument properties are used.
    // The channel waveform state uses the direct operator waveform.


    // Make sure that the frame reading position is inside the sample.
    channels[channel].waveforms[op].wrapReadPos(loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopType,
        loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopStart, loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopEnd);


    float notePitch = channels[channel].waveforms[op].pitch;

    if (channels[channel].arpIndex > -1 && loadedInstruments[channels[channel].instrument].waveforms[mappedWave].useArp) // Arpeggiate note and find pitch.
    {
        float arpNote = ((channels[channel].arpP[channels[channel].arpIndex]) - 7.75f) * 4.0f;
        arpNote /= loadedSong.edo;
        arpNote = pow(2, arpNote);
        notePitch *= arpNote;
    }

    int lfo = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].octave - 3;
    float lfoMultiplier = pow(0.5f, lfo);

    notePitch *= lfoMultiplier;

    if (notePitch > 12.0f) notePitch = 12.0f;
    if (notePitch < -12.0f) notePitch = -12.0f;


    // Glide value.
    float glide = (loadedInstruments[channels[channel].instrument].glide) * 20.0f;
    if (glide > 0.0f)
        glide = (1.0f / glide);
    else
        glide = 1.0f;
    float interp = pow(0.99f, glide);





    


    for (int i = 0; i < frameCount; i++)
    {
        


        //////////////////////////////////////////////////////////////////////////////////////////////////////// Envelope

        channels[channel].waveforms[op].envelopePos += 0.001f * channels[channel].waveforms[op].envelopeSpeed * loadedInstruments[channels[channel].instrument].waveforms[mappedWave].envelopeScale;

        while (channels[channel].waveforms[op].envelopePos >= channels[channel].waveforms[op].nextEnvelopePos)
        {
            channels[channel].waveforms[op].currentEnvelopePos = channels[channel].waveforms[op].nextEnvelopePos;
            channels[channel].waveforms[op].currentEnvelopeAmp = channels[channel].waveforms[op].nextEnvelopeAmp;
            channels[channel].waveforms[op].currentEnvelopeIndex++;

            if (channels[channel].waveforms[op].currentEnvelopeIndex < loadedInstruments[channels[channel].instrument].waveforms[mappedWave].envelopePoints.size())
            {
                channels[channel].waveforms[op].nextEnvelopePos = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].envelopePoints[channels[channel].waveforms[op].currentEnvelopeIndex].position;
                channels[channel].waveforms[op].nextEnvelopeAmp = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].envelopePoints[channels[channel].waveforms[op].currentEnvelopeIndex].amp;
            }
            else // Envelope complete.
            {
                channels[channel].waveforms[op].nextEnvelopePos = 1000000.0f;
            }
        }


        float envInterp = float(channels[channel].waveforms[op].envelopePos - channels[channel].waveforms[op].currentEnvelopePos) /
            (channels[channel].waveforms[op].nextEnvelopePos - channels[channel].waveforms[op].currentEnvelopePos);

        float envAmp = channels[channel].waveforms[op].currentEnvelopeAmp * (1.0f - envInterp) + channels[channel].waveforms[op].nextEnvelopeAmp * envInterp;


        // Apply release
        if (channels[channel].noteStopped)
        {
            if (loadedInstruments[channels[channel].instrument].waveforms[mappedWave].noSustain)
            {
                envAmp = 0.0f;
            }
            else
            {
                channels[channel].waveforms[op].releaseTimer += (1.0f - loadedInstruments[channels[channel].instrument].waveforms[mappedWave].release) * 0.0002;
                envAmp *= 1.0f - channels[channel].waveforms[op].releaseTimer;
                if (envAmp < 0.0f)
                    envAmp = 0.0f;
            }
        }
        

        ////////////////////////////////////////////////////////////////////////////////////////////////////////


        // Glide to pitch.
        channels[channel].waveforms[op].pitch = channels[channel].waveforms[op].pitch * interp + channels[channel].waveforms[op].glideDest * (1.0f - interp);
        channels[channel].waveforms[op].glideVolume = channels[channel].waveforms[op].glideVolume * interp + envAmp * (1.0f - interp);

        
        notePitch = channels[channel].waveforms[op].pitch;

        if (channels[channel].arpIndex > -1 && loadedInstruments[channels[channel].instrument].waveforms[mappedWave].useArp) // Arpeggiate note and find pitch.
        {
            float arpNote = ((channels[channel].arpP[channels[channel].arpIndex]) - 7.75f) * 4.0f;
            arpNote /= loadedSong.edo;
            arpNote = pow(2, arpNote);
            notePitch *= arpNote;
        }

        notePitch *= lfoMultiplier;

        if (notePitch > 12.0f) notePitch = 12.0f;
        if (notePitch < -12.0f) notePitch = -12.0f;



        ////////////////////////////////////////////////////////////////////////////////////////////////////////// Read frame data.
        float sampleRateIndex = int(channels[channel].waveforms[op].sampleReadPos / channels[channel].waveforms[op].sampleRate) * channels[channel].waveforms[op].sampleRate;
        int index1 = int(sampleRateIndex);
        int index2 = int(sampleRateIndex) + 1;
        

        float t = channels[channel].waveforms[op].sampleReadPos - index1;  // Fractional part


        if (index2 >= loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopEnd)
            index2 = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopStart;

        float frameVol = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].pcmFrames[index1] * (1.0f - t) + loadedInstruments[channels[channel].instrument].waveforms[mappedWave].pcmFrames[index2] * t;


        float frameVolStereo[2] = { frameVol, frameVol };

        channels[channel].waveforms[op].sampleReadPos += notePitch;

        /////////////////////////////////////// Read interpolation volume.
        if (channels[channel].waveforms[op].interpTimer > 0.0f)
        {
            // Interpolation frame reading position.
            float interpolateIndex = channels[channel].waveforms[op].interpLastReadPos;

            int lastInstrument = channels[channel].interpLastInstrument;

            // Make sure that the frame reading position is inside the sample.
            if (interpolateIndex < 0.0f)
                interpolateIndex = 0.0f;
            if (interpolateIndex >= loadedInstruments[lastInstrument].waveforms[mappedWave].pcmFrames.size() - 1)
                interpolateIndex = loadedInstruments[lastInstrument].waveforms[mappedWave].pcmFrames.size() - 1;



            float iTime = channels[channel].waveforms[op].interpTimer;


            float interpSampleRateIndex = int(interpolateIndex / channels[channel].waveforms[op].sampleRate) * channels[channel].waveforms[op].sampleRate;
            int interpIndex1 = int(interpSampleRateIndex);
            int interpIndex2 = int(interpSampleRateIndex) + 1;


            float interpT = interpolateIndex - interpIndex1;  // Fractional part


            if (interpIndex2 >= loadedInstruments[lastInstrument].waveforms[mappedWave].pcmFrames.size())
                interpIndex2 = 0;

            float interpFrameVol = loadedInstruments[lastInstrument].waveforms[mappedWave].pcmFrames[interpIndex1] * (1.0f - interpT) + loadedInstruments[lastInstrument].waveforms[mappedWave].pcmFrames[interpIndex2] * interpT;


            frameVolStereo[0] = interpFrameVol * iTime + frameVolStereo[0] * (1.0f - iTime);
            frameVolStereo[1] = interpFrameVol * iTime + frameVolStereo[1] * (1.0f - iTime);

            channels[channel].waveforms[op].interpLastReadPos += notePitch;
            channels[channel].waveforms[op].interpTimer -= 0.01f;
        }



        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        


        

        for (int modNum = 0; modNum < 4; modNum++)
        {
            if (mods[modNum] != nullptr) // Modulate
            {
                for (int stereoOffset = 0; stereoOffset < 2; stereoOffset++)
                {
                    if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 0) // FM
                    {
                        channels[channel].waveforms[op].sampleReadPos += mods[modNum][i * 2 + stereoOffset] * loadedInstruments[channels[channel].instrument].modScale[modNum] * 1.0f;
                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 1) // AM
                    {
                        // AM
                        frameVolStereo[stereoOffset] *= abs(mods[modNum][i * 2 + stereoOffset]) * loadedInstruments[channels[channel].instrument].modScale[modNum] * 2.0f;
                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 2) // Apply delay.
                    {
                        int sampleLen = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].pcmFrames.size();

                        float delayIndex = channels[channel].waveforms[op].sampleReadPos + mods[modNum][i * 2 + stereoOffset] * 100.0f * loadedInstruments[channels[channel].instrument].modScale[modNum] * 2.0f;

                        delayIndex = wrapReadPos(delayIndex, channel, mappedWave);

                        ma_uint32 dIndex1 = delayIndex;
                        ma_uint32 dIndex2 = delayIndex + 1;

                        float t2 = delayIndex - dIndex1;  // Fractional part
                        if (dIndex2 >= sampleLen)
                            dIndex2 = 0;

                        frameVolStereo[stereoOffset] += loadedInstruments[channels[channel].instrument].waveforms[mappedWave].pcmFrames[dIndex1] * (1.0f - t2) + loadedInstruments[channels[channel].instrument].waveforms[mappedWave].pcmFrames[dIndex2] * t2;
                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 6) // Apply fuzz.
                    {
                        float drySignal = frameVolStereo[stereoOffset];

                        float modFuzz = abs(mods[modNum][i * 2 + stereoOffset]) * 1.0f;

                        if (modFuzz > 1.0f) modFuzz = 1.0f; // Cap fuzz level.
                        modFuzz *= 16.0f;
                        frameVolStereo[stereoOffset] *= 1.0f + modFuzz;

                        float effect = loadedInstruments[channels[channel].instrument].modScale[modNum];

                        if (loadedInstruments[channels[channel].instrument].waveforms[mappedWave].fuzzType == 0) // Clip
                        {
                            // Clamp the volume to a normal range.
                            if (frameVolStereo[stereoOffset] > 1.0f) frameVolStereo[stereoOffset] = 1.0f;
                            else if (frameVolStereo[stereoOffset] < -1.0f) frameVolStereo[stereoOffset] = -1.0f;

                        }
                        else if (loadedInstruments[channels[channel].instrument].waveforms[mappedWave].fuzzType == 1) // Fold
                        {
                            bool waveFolded = false;
                            while (!waveFolded)
                            {
                                if (frameVolStereo[stereoOffset] <= 1.0f && frameVolStereo[stereoOffset] >= -1.0f)
                                    waveFolded = true;

                                if (frameVolStereo[stereoOffset] > 1.0f)
                                {
                                    frameVolStereo[stereoOffset] -= 1.0f;
                                    frameVolStereo[stereoOffset] *= -1.0f;
                                    frameVolStereo[stereoOffset] += 1.0f;
                                }
                                else if (frameVolStereo[stereoOffset] < -1.0f)
                                {
                                    frameVolStereo[stereoOffset] += 1.0f;
                                    frameVolStereo[stereoOffset] *= -1.0f;
                                    frameVolStereo[stereoOffset] -= 1.0f;
                                }
                            }
                        }
                        else // Ring
                        {
                            bool waveFolded = false;
                            while (!waveFolded)
                            {
                                if (frameVolStereo[stereoOffset] <= 1.0f && frameVolStereo[stereoOffset] >= -1.0f)
                                    waveFolded = true;

                                if (frameVolStereo[stereoOffset] > 1.0f)
                                {
                                    frameVolStereo[stereoOffset] -= 1.0f;
                                    frameVolStereo[stereoOffset] *= -1.0f;
                                    frameVolStereo[stereoOffset] += 1.0f;
                                }
                                else if (frameVolStereo[stereoOffset] < 0.0f)
                                {
                                    frameVolStereo[stereoOffset] *= -1.0f;
                                }
                            }
                        }

                        frameVolStereo[stereoOffset] /= (1.0f + modFuzz * 0.125f);

                        frameVolStereo[stereoOffset] = frameVolStereo[stereoOffset] * effect + drySignal * (1.0f - effect);

                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 7) // Apply PM.
                    {
                        float mapPos = ((mods[modNum][i * 2 + stereoOffset] * mods[modNum][i * 2 + stereoOffset] * mods[modNum][i * 2 + stereoOffset] * loadedInstruments[channels[channel].instrument].modScale[modNum]) + 0.5f) * 4.0f;
                        while (mapPos > 1.0f) mapPos--;
                        while (mapPos < 0.0f) mapPos++;
                        int sampleLen = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].pcmFrames.size() - 1;
                        mapPos *= sampleLen;
                        channels[channel].waveforms[op].sampleReadPos = mapPos;

                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 8) // Apply bit depth.
                    {
                        float bitDepth = abs(mods[modNum][i * 2 + stereoOffset] * loadedInstruments[channels[channel].instrument].modScale[modNum]) * 256.0f;
                        if (bitDepth > 256.0f) bitDepth = 256.0f;
                        else if (bitDepth < 1.0f) bitDepth = 1.0f;
                        frameVolStereo[stereoOffset] *= 256.0f;
                        frameVolStereo[stereoOffset] = int(frameVolStereo[stereoOffset] / bitDepth) * bitDepth;
                        frameVolStereo[stereoOffset] /= 256.0f;
                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 9) // Apply envelope speed.
                    {
                        float maxSpeed = 16.0f;
                        float speed = abs(mods[modNum][i * 2 + stereoOffset] * loadedInstruments[channels[channel].instrument].modScale[modNum]) * maxSpeed;
                        if (speed > maxSpeed) speed = maxSpeed;
                        else if (speed < 0.0f) speed = 0.0f;
                        channels[channel].waveforms[op].envelopeSpeed = speed;
                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 10) // Apply high-pass filter.
                    {
                        channels[channel].waveforms[op].highPass = abs(mods[modNum][i * 2 + stereoOffset] * loadedInstruments[channels[channel].instrument].modScale[modNum]) * 2.0f;
                        
                        if (channels[channel].waveforms[op].highPass > 2.0f) channels[channel].waveforms[op].highPass = 2.0f;
                        else if (channels[channel].waveforms[op].highPass < 0.0f) channels[channel].waveforms[op].highPass = 0.0f;

                        float cutoffFreq = channels[channel].waveforms[op].highPass * 2000.0f;
                        if (cutoffFreq <= 0)
                            cutoffFreq = 1.0f;
                        float RC = 1.0f / (2.0f * 3.14159265f * cutoffFreq);
                        channels[channel].waveforms[op].alphaHigh = RC / (RC + (1.0f / 48000.0f));


                        // High-pass filter
                        float alpha2 = channels[channel].waveforms[op].alphaHigh;

                        float oldInputL = 0;
                        float oldInputR = 0;

                        float frame = frameVolStereo[stereoOffset] * 0.5f;




                        // High-pass
                        oldInputL = frame;
                        frame = alpha2 * (channels[channel].waveforms[op].prevHighPassSample[stereoOffset] + frame - channels[channel].waveforms[op].prevHighPassSampleI[stereoOffset]);
                        channels[channel].waveforms[op].prevHighPassSampleI[stereoOffset] = oldInputL;
                        channels[channel].waveforms[op].prevHighPassSample[stereoOffset] = frame;


                        frameVolStereo[stereoOffset] = frame;
                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] == 11) // Apply sample rate.
                    {
                        channels[channel].waveforms[op].sampleRate = (abs(mods[modNum][i * 2 + stereoOffset] * loadedInstruments[channels[channel].instrument].modScale[modNum]) * 32.0f) + 1.0f;

                        if (channels[channel].waveforms[op].sampleRate > 32.0f) channels[channel].waveforms[op].sampleRate = 32.0f;
                        else if (channels[channel].waveforms[op].sampleRate < 1.0f) channels[channel].waveforms[op].sampleRate = 1.0f;


                    }
                    else if (loadedInstruments[channels[channel].instrument].modulationTypes[modNum] > 2) // Apply low-pass.
                    {
                        int modType = loadedInstruments[channels[channel].instrument].modulationTypes[modNum];
                        float lpCutoff; // Cutoff frequency in Hz

                        // Set initial parameters
                        if (modType == 4)
                            lpCutoff = loadedInstruments[channels[channel].instrument].modScale[modNum] * 4000.0f * abs(notePitch);
                        else if (modType == 5)
                            lpCutoff = (abs(mods[modNum][i * 2 + stereoOffset])) * 4000.0f * abs(notePitch) * loadedInstruments[channels[channel].instrument].modScale[modNum] * 2.0f;
                        else
                            lpCutoff = (abs(mods[modNum][i * 2 + stereoOffset])) * 4000.0f * abs(notePitch);

                        
                        
                        if (lpCutoff < 200.0f) lpCutoff = 200.0f;
                        else if (lpCutoff > 24000.0f) lpCutoff = 24000.0f;

                        // Update parameters.
                        // Normalize cutoff frequency (0 to 1, where 1 = Nyquist)
                        float f = 2.0f * lpCutoff / 48000.0f;

                        // Clamp to valid range
                        if (f > 0.99) f = 0.99;

                        // Calculate feedback amount from resonance
                        float resonance;
                        if (modType == 3)
                            resonance = loadedInstruments[channels[channel].instrument].modScale[modNum] * 2.0f;
                        else if (modType == 5)
                            resonance = loadedInstruments[channels[channel].instrument].modScale[modNum] * 2.0f * loadedInstruments[channels[channel].instrument].modScale[modNum];
                        else
                            resonance = abs(mods[modNum][i * 2 + stereoOffset]) * 2.0f;

                        float q = 1.0 - f;
                        float fb = resonance + resonance / (1.0f - f);

                        // Process audio samples
                        frameVolStereo[stereoOffset] = channels[channel].waveforms[op].process(frameVol, f, fb, q);
                    }
                }
            }
        }
        


        // The added fuzz level.
        float fuzzLevel = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].fuzz;
        if (fuzzLevel > 1.0f) fuzzLevel = 1.0f; // Cap fuzz level.
        else if (fuzzLevel < 0.0f) fuzzLevel = 0.0f; // Cap fuzz level.
        float fuzz = fuzzLevel * 16.0f;

        
        for (int j = 0; j < 2; j++)
        {
            frameVolStereo[j] *= channels[channel].waveforms[op].volume;
            frameVolStereo[j] *= channels[channel].waveforms[op].glideVolume;

            // Add the fuzz effect.
            if (fuzzLevel > 0)
            {
                frameVolStereo[j] *= 1.0f + fuzz;

                if (loadedInstruments[channels[channel].instrument].waveforms[mappedWave].fuzzType == 0) // Clip
                {
                    // Clamp the volume to a normal range.
                    if (frameVolStereo[j] > 1.0f) frameVolStereo[j] = 1.0f;
                    else if (frameVolStereo[j] < -1.0f) frameVolStereo[j] = -1.0f;
                }
                else if (loadedInstruments[channels[channel].instrument].waveforms[mappedWave].fuzzType == 1) // Fold
                {
                    bool waveFolded = false;
                    while (!waveFolded)
                    {
                        if (frameVolStereo[j] <= 1.0f && frameVolStereo[j] >= -1.0f)
                            waveFolded = true;

                        if (frameVolStereo[j] > 1.0f)
                        {
                            frameVolStereo[j] -= 1.0f;
                            frameVolStereo[j] *= -1.0f;
                            frameVolStereo[j] += 1.0f;
                        }
                        else if (frameVolStereo[j] < -1.0f)
                        {
                            frameVolStereo[j] += 1.0f;
                            frameVolStereo[j] *= -1.0f;
                            frameVolStereo[j] -= 1.0f;
                        }
                    }
                }
                else // Ring
                {
                    bool waveFolded = false;
                    while (!waveFolded)
                    {
                        if (frameVolStereo[j] <= 1.0f && frameVolStereo[j] >= -1.0f)
                            waveFolded = true;

                        if (frameVolStereo[j] > 1.0f)
                        {
                            frameVolStereo[j] -= 1.0f;
                            frameVolStereo[j] *= -1.0f;
                            frameVolStereo[j] += 1.0f;
                        }
                        else if (frameVolStereo[j] < 0.0f)
                        {
                            frameVolStereo[j] *= -1.0f;
                        }
                    }
                }

                frameVolStereo[j] /= (1.0f + fuzz * 0.125f);
            }
        }
        

        




        pOutputF32[i * 2] += frameVolStereo[0];
        if (loadedInstruments[channels[channel].instrument].waveforms[mappedWave].invertStereo)
            pOutputF32[i * 2 + 1] -= frameVolStereo[1];
        else
            pOutputF32[i * 2 + 1] += frameVolStereo[1];


        ////////////////////////////////////////////

        // Wave Volume Slide
        if (channels[channel].waveforms[op].volumeSlide != 0.0f)
        {
            channels[channel].waveforms[op].volume += channels[channel].waveforms[op].volumeSlide * 0.0001f * 120.0f;
            if (channels[channel].waveforms[op].volume > 1.0f) channels[channel].waveforms[op].volume = 1.0f;
            else if (channels[channel].waveforms[op].volume < 0.0f) channels[channel].waveforms[op].volume = 0.0f;
        }

        // Pitch Slide
        if (channels[channel].pitchSlide != 0)
        {
            if (channels[channel].waveforms[op].pitch > 0.0f)
                channels[channel].waveforms[op].glideDest += channels[channel].pitchSlide * 0.00001f * 120.0f;
            else
                channels[channel].waveforms[op].glideDest -= channels[channel].pitchSlide * 0.00001f * 120.0f;

            if (channels[channel].waveforms[op].glideDest > 8.0f)
                channels[channel].waveforms[op].glideDest = 8.0f;
            else if (channels[channel].waveforms[op].glideDest < -8.0f)
                channels[channel].waveforms[op].glideDest = -8.0f;
        }


         


        /////////////////////////////////////////////// End of sample.
        channels[channel].waveforms[op].wrapReadPos(loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopType,
            loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopStart, loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopEnd);
        
    }


    




    return;
}



void applySubtractiveFilters(float* pOutputF32, ma_uint64 frameCount, int channel, float* input, ma_uint32 frameOffset)
{
    // Add parameter slides

    

    float volumeL = 0;
    float volumeR = 0;

    // Panning
    float pan = channels[channel].stereo;
    volumeL = 1.0f - pan;
    volumeR = pan;

    if (volumeL > 1.0f) volumeL = 1.0f;
    else if (volumeL < 0.0f) volumeL = 0.0f;
    if (volumeR > 1.0f) volumeR = 1.0f;
    else if (volumeR < 0.0f) volumeR = 0.0f;


    

    for (int i = 0; i < frameCount; i++)
    {
        float noteVolume = channels[channel].volume * loadedInstruments[channels[channel].instrument].volume;
        if (noteVolume > 1.0f) noteVolume = 1.0f;
        else if (noteVolume < 0.0f) noteVolume = 0.0f;

        float oldInputL = 0;
        float oldInputR = 0;

        float frameL = input[i * 2] * 0.5f;
        float frameR = input[i * 2 + 1] * 0.5f;

        





        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        

        // Write the frames.


        pOutputF32[(i + frameOffset) * 2] += frameL * noteVolume * volumeL;
        pOutputF32[(i + frameOffset) * 2 + 1] += frameR * noteVolume * volumeR;


        /////////////////////////////////////////////// Update note parameters.

        // Volume Slide
        if (channels[channel].volumeSlide != 0)
        {
            channels[channel].volume += channels[channel].volumeSlide * 0.0001f * 120.0f;
            if (channels[channel].volume > 1) channels[channel].volume = 1;
            else if (channels[channel].volume < 0) channels[channel].volume = 0;
        }

        // Jump Slide
        if (channels[channel].jumpSlide != 0)
        {
            channels[channel].jumpPoint += channels[channel].jumpSlide * 0.005f;
            if (channels[channel].jumpPoint < 0.0f) channels[channel].jumpPoint = 0.0f;
        }

        // Retrigger Slide
        if (channels[channel].retriggerSlide != 0)
        {
            channels[channel].retrigger += channels[channel].retriggerSlide * 0.01f;
            if (channels[channel].retrigger < 1.0f) channels[channel].retrigger = 1.0f;
            else if (channels[channel].retrigger > 255.0f) channels[channel].retrigger = 255.0f;
        }
    }


    return;
}



void readWithFMAlgorithm(float* pOutputF32, ma_uint64 frameCount, int channel, ma_uint32 frameOffset)
{
    if (!channels[channel].playing)
        return;


    float frames[480 * 2];
    std::fill(frames, frames + 480 * 2, 0.0f);
    float* framePointer = &frames[0];


    int modTypes[4] = { 0,0,0,0 };
    for (int i = 0; i < 4; i++)
        modTypes[i] = loadedInstruments[channels[channel].instrument].modulationTypes[i];


    // Each value in the mod pointer list is mapped to one of the intreument's 4 modulation instances, each with its own modulation type.

    float* nullPointer[4] = { NULL, NULL, NULL, NULL };

    // Modulators are applied left to right, and top to bottom.

    switch (loadedInstruments[channels[channel].instrument].algorithmType)
    {

    case 1:
    {
        readModulator(framePointer, frameCount, channel, 0, nullPointer); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, nullPointer); // READ CARRIER 2
        break;
    }

    case 2:
    {
        float mod2[480 * 2] = { 0 };

        float* goTo1[4] = { &mod2[0], NULL, NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 1, nullPointer); // Read mod 2 to carrier 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1

        break;
    }

    case 3:
    {
        readModulator(framePointer, frameCount, channel, 0, nullPointer); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, nullPointer); // READ CARRIER 2
        readModulator(framePointer, frameCount, channel, 2, nullPointer); // READ CARRIER 3
        
        break;
    }

    case 4:
    {
        float mod3[480 * 2] = { 0 };

        float* goTo1[4] = { &mod3[0], NULL, NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 2, nullPointer); // Read mod 3 to 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, nullPointer); // READ CARRIER 2

        break;
    }

    case 5:
    {
        float mod2[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };

        float* goTo1[4] = { &mod2[0], &mod3[0], NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 1, nullPointer); // Read mod 2 to 1
        readModulator(goTo1[1], frameCount, channel, 2, nullPointer); // Read mod 3 to 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1

        break;
    }

    case 6:
    {
        float mod2[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };

        float* goTo2[4] = { NULL, &mod3[0], NULL, NULL };
        readModulator(goTo2[1], frameCount, channel, 2, nullPointer); // Read mod 3 to 2

        float* goTo1[4] = { &mod2[0], NULL, NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 1, goTo2); // Read mod 2 to 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1

        break;
    }

    case 7:
    {
        float mod3[480 * 2] = { 0 };

        float* goTo1[4] = { &mod3[0], NULL, NULL, NULL };
        float* goTo2[4] = { NULL, &mod3[0], NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 2, nullPointer); // Read mod 3 to 2 and 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, goTo2); // READ CARRIER 2

        break;
    }

    case 8:
    {
        float mod2[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };
        float mod4[480 * 2] = { 0 };

        float* goTo3[4] = { NULL, NULL, &mod4[0], NULL };
        readModulator(goTo3[2], frameCount, channel, 3, nullPointer); // Read mod 4 to 3

        float* goTo2[4] = { NULL, &mod3[0], NULL, NULL };
        readModulator(goTo2[1], frameCount, channel, 2, goTo3); // Read mod 3 to 2

        float* goTo1[4] = { &mod2[0], NULL, NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 1, goTo2); // Read mod 2 to 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1

        break;
    }

    case 9:
    {
        float mod2[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };
        float mod4[480 * 2] = { 0 };

        float* goTo2[4] = { NULL, &mod3[0], &mod4[0], NULL };
        readModulator(goTo2[1], frameCount, channel, 2, nullPointer); // Read mod 3 to 2
        readModulator(goTo2[2], frameCount, channel, 3, nullPointer); // Read mod 4 to 2

        float* goTo1[4] = { &mod2[0], NULL, NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 1, goTo2); // Read mod 2 to 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1

        break;
    }

    case 10:
    {
        float mod2[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };
        float mod4[480 * 2] = { 0 };
        
        
        
        float* goTo2[4] = { NULL, NULL, &mod4[0], NULL };
        readModulator(goTo2[2], frameCount, channel, 3, nullPointer); // Read mod 4 to 2

        float* goTo1[4] = { &mod2[0], &mod3[0], NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 1, goTo2); // Read mod 2 to 1
        readModulator(goTo1[1], frameCount, channel, 2, nullPointer); // Read mod 3 to 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1

        break;
    }

    case 11:
    {
        float mod2[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };
        float mod4[480 * 2] = { 0 };
        
        float* goTo3[4] = { NULL, NULL, NULL, &mod4[0] };
        float* goTo2[4] = { NULL, NULL, &mod4[0], NULL };
        readModulator(goTo3[3], frameCount, channel, 3, nullPointer); // Read mod 4 to 2 and 3

        float* goTo1[4] = { &mod2[0], &mod3[0], NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 1, goTo2); // Read mod 2 to 1
        readModulator(goTo1[1], frameCount, channel, 2, goTo3); // Read mod 3 to 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1

        break;
    }

    case 12:
    {
        float mod4[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };
        
        float* goTo3[4] = { NULL, NULL, &mod4[0], NULL };

        readModulator(goTo3[2], frameCount, channel, 3, nullPointer); // Read mod 4 to 3

        float* goTo1[4] = { &mod3[0], NULL, NULL, NULL };
        float* goTo2[4] = { NULL, &mod3[0], NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 2, goTo3); // Read mod 3 to 1 and 2

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, goTo2); // READ CARRIER 2

        break;
    }

    case 13:
    {
        float mod4[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };

        float* goTo3[4] = { NULL, &mod4[0], NULL, NULL };
        readModulator(goTo3[1], frameCount, channel, 3, nullPointer); // Read mod 4 to 3

        float* goTo1[4] = { &mod3[0], NULL, NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 2, goTo3); // Read mod 3 to 1

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, nullPointer); // READ CARRIER 2

        break;
    }

    case 14:
    {
        float mod2[480 * 2] = { 0 };
        float mod3[480 * 2] = { 0 };
        float mod4[480 * 2] = { 0 };
        
        float* goTo1[4] = { &mod2[0], &mod3[0], &mod4[0], NULL };

        readModulator(goTo1[0], frameCount, channel, 1, nullPointer); // Read mod 2 to 1
        readModulator(goTo1[1], frameCount, channel, 2, nullPointer); // Read mod 3 to 1
        readModulator(goTo1[2], frameCount, channel, 3, nullPointer); // Read mod 4 to 1
        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1

        break;
    }

    case 15:
    {
        float mod3[480 * 2] = { 0 };
        float mod4[480 * 2] = { 0 };

        float* goTo1[4] = { &mod3[0], NULL, NULL, NULL };
        float* goTo2[4] = { NULL, &mod4[0], NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 2, nullPointer); // Read mod 3 to 1
        readModulator(goTo2[1], frameCount, channel, 3, nullPointer); // Read mod 4 to 2

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, goTo2); // READ CARRIER 2


        break;
    }

    case 16:
    {
        float mod4[480 * 2] = { 0 };

        float* goTo1[4] = { &mod4[0], NULL, NULL, NULL };
        float* goTo2[4] = { NULL, &mod4[0], NULL, NULL };
        float* goTo3[4] = { NULL, NULL, &mod4[0], NULL };
        readModulator(goTo1[0], frameCount, channel, 3, nullPointer); // Read mod 4 to 1, 2 and 3

        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, goTo2); // READ CARRIER 2
        readModulator(framePointer, frameCount, channel, 2, goTo3); // READ CARRIER 3

        break;
    }

    case 17:
    {
        float mod4[480 * 2] = { 0 };

        float* goTo2[4] = { &mod4[0], NULL, NULL, NULL };
        readModulator(goTo2[0], frameCount, channel, 3, nullPointer); // Read mod 4 to 2

        readModulator(framePointer, frameCount, channel, 0, nullPointer); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, goTo2); // READ CARRIER 2
        readModulator(framePointer, frameCount, channel, 2, nullPointer); // READ CARRIER 3

        break;
    }

    case 18:
    {
        float mod3[480 * 2] = { 0 };
        float mod4[480 * 2] = { 0 };

        float* goTo1[4] = { &mod3[0], &mod4[0], NULL, NULL };
        readModulator(goTo1[0], frameCount, channel, 2, nullPointer); // Read mod 3 to 1
        readModulator(goTo1[1], frameCount, channel, 3, nullPointer); // Read mod 4 to 1
        readModulator(framePointer, frameCount, channel, 0, goTo1); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, nullPointer); // READ CARRIER 2

        break;
    }

    case 19:
    {
        readModulator(framePointer, frameCount, channel, 0, nullPointer); // READ CARRIER 1
        readModulator(framePointer, frameCount, channel, 1, nullPointer); // READ CARRIER 2
        readModulator(framePointer, frameCount, channel, 2, nullPointer); // READ CARRIER 3
        readModulator(framePointer, frameCount, channel, 3, nullPointer); // READ CARRIER 4

        break;
    }

    default:
    {
        readModulator(framePointer, frameCount, channel, 0, nullPointer);
        break;
    }
    }

    applySubtractiveFilters(pOutputF32, frameCount, channel, framePointer, frameOffset);
    


    return;
}



void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    std::lock_guard<std::shared_mutex> lock(mtx);
    //std::shared_lock<std::shared_mutex> lock(mtx);

    
    float* pOutputF32 = (float*)pOutput;
    std::fill(pOutputF32, pOutputF32 + 960, 0.0f);

    

    if (editor.playingSong)
    {
        ma_uint32 frameOffset = 0;
        ma_uint32 framesToRead = 480;

        ma_uint32 fInOldBeat = loadedSong.timeInNote * 48.0f;

        float elapsedMS = float(frameCount) / 48000.0f * 1000.0f;

        loadedSong.timeInNote += elapsedMS;
        loadedSong.timeInSong += elapsedMS;


        ma_uint32 fInNote = loadedSong.timeInNote * 48.0f;

        ma_uint32 fInBeat = (60000.0f / (loadedSong.bpm * 4.0f)) * 48.0f;

        while (fInNote >= fInBeat)
        {
            float fInThisBeat = fInBeat - fInOldBeat;
            PlayChannels(pOutputF32, fInThisBeat, frameOffset);
            frameOffset += fInThisBeat;
            fInOldBeat = 0.0f;
            fInNote -= fInBeat;
            loadedSong.timeInNote -= fInBeat / 48.0f;
            framesToRead -= fInThisBeat;
            updateSongOnBeat();
        }

        PlayChannels(pOutputF32, framesToRead, frameOffset);
        
    }
    else // if (sampleDisplay.playingSample)
    {
        ma_uint32 frameOffset = 0;
        // For each channel, read out frames of data in 4000 frame groups.
        PlayChannels(pOutputF32, frameCount, frameOffset);
    }


    
    
    (void)pInput;
    (void)pDevice;
}


void PlayChannels(float* pOutputF32, ma_uint32 frameCount, ma_uint32 frameOffset)
{
    for (int channel = 0; channel < 8; channel++)
    {
        if (!(channels[channel].muted || (editor.playSolo && !channels[channel].solo)))
        {
            if (loadedInstruments[channels[channel].instrument].arpLength >= 0) // Arpeggiate note.
            {
                channels[channel].arpTimer += frameCount;

                float arpAmount = 48000.0f * (120.0f / loadedSong.bpm) * ((loadedInstruments[channels[channel].instrument].arpSpeed + 0.0625f)) / 8.0f;
                //float arpAmount = loadedInstruments[channels[channel].instrument].arpSpeed * 16.0f;

                if (channels[channel].arpTimer > arpAmount)
                {
                    channels[channel].arpTimer -= arpAmount;
                    channels[channel].arpIndex++;
                    if (channels[channel].arpIndex > loadedInstruments[channels[channel].instrument].arpLength)
                        channels[channel].arpIndex = -1;
                }
            }


            if (channels[channel].patternOffset > 0) // Start offset note.
            {
                channels[channel].patternOffset -= frameCount;

                if (channels[channel].patternOffset <= 0) // Start the new note.
                {
                    int framesOver = -channels[channel].patternOffset;

                    readWithFMAlgorithm(pOutputF32, ma_uint64(frameCount - framesOver), channel, frameOffset);
                    ma_uint64 delayedFrameOffset = ma_uint64(frameOffset + frameCount - framesOver);
                    channels[channel].patternOffset = 0;
                    StartNote(channel, channels[channel].offsetInstrument, channels[channel].offsetNote, channels[channel].jumpPoint);
                    readWithFMAlgorithm(pOutputF32, framesOver, channel, delayedFrameOffset);
                }
                else
                    readWithFMAlgorithm(pOutputF32, frameCount, channel, frameOffset);
            }
            else if (channels[channel].retrigger > 0) // Retrigger note.
            {
                channels[channel].retriggerTimer += frameCount;

                float retrigAmount = 48000.0f * (120.0f / float(loadedSong.bpm)) * ((channels[channel].retrigger - 1.0f) / 256.0f);

                if (channels[channel].retriggerTimer > retrigAmount)
                {
                    channels[channel].retriggerTimer -= retrigAmount;
                    StartNote(channel, channels[channel].offsetInstrument, channels[channel].offsetNote, channels[channel].jumpPoint);
                    
                }
                readWithFMAlgorithm(pOutputF32, frameCount, channel, frameOffset);
            }
            else
                readWithFMAlgorithm(pOutputF32, frameCount, channel, frameOffset);
        }
    }

    return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








void SetUpAudioEngine()
{
    // In this example, all decoders need to have the same output format.
    decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 48000);

    /* Create only a single device. The decoders will be mixed together in the callback. In this example the data format needs to be the same as the decoders. */
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = 48000;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = NULL;

    ma_device_init(NULL, &deviceConfig, &device);

    // Now we start playback and wait for the audio thread to tell us to stop.
    ma_device_start(&device);
    

    // Set up the encoder
    encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_f32, 2, 48000);
    



    return;
}




void StartNote(int channel, int sampleNumber, float pitch, int startFrame)
{

    channels[channel].offsetNote = pitch;


    if (!editor.playingSong)
        for (int ch = 0; ch < 8; ch++)
            channels[ch].playing = false;

    if (!loadedInstruments[sampleNumber].enabled) // Don't play samples that are not loaded.
        return;

    if (sampleNumber < 0) // Don't play notes with samples below zero. (-1 is used to stop notes.)
        return;

    

    channels[channel].playing = true;


    // Set interpolation values for the previous waveforms that do not continue.
    for (int wave = 0; wave < 4; wave++)
    {
        int mappedWave = loadedInstruments[channels[channel].instrument].operatorMapping[wave];

        if (!loadedInstruments[channels[channel].instrument].waveforms[mappedWave].continueNote)
            channels[channel].waveforms[wave].interpTimer = 1.0f;
    }

    channels[channel].interpLastInstrument = channels[channel].instrument;
    bool instrumentChanged = channels[channel].instrument != sampleNumber;
    channels[channel].instrument = sampleNumber;


    // Set pitch.
    pitch -= loadedSong.edo * 4;
    

    // Reset arpeggiation.
    channels[channel].arpIndex = -1;
    channels[channel].arpTimer = 0.0f;
    for (int i = 0; i < loadedInstruments[sampleNumber].arpLength + 1; i++)
    {
        channels[channel].arpP[i] = loadedInstruments[sampleNumber].arpPitches[i];
    }

    

    for (int wave = 0; wave < 4; wave++)
    {
        int mappedWave = loadedInstruments[channels[channel].instrument].operatorMapping[wave];
        


        if (loadedInstruments[sampleNumber].waveforms[mappedWave].pitchToNote)
        {
            float wavePitch = pitch;
            wavePitch = pow(2, wavePitch / loadedSong.edo);

            channels[channel].waveforms[wave].glideDest = wavePitch;
        }
        else
        {
            channels[channel].waveforms[wave].pitch = 1.0f;
            channels[channel].waveforms[wave].glideDest = 1.0f;
        }


        // Set old read position.
        if (!loadedInstruments[sampleNumber].waveforms[mappedWave].continueNote || instrumentChanged)
        {
            channels[channel].waveforms[wave].interpLastReadPos = channels[channel].waveforms[wave].sampleReadPos;
        }
        else
            channels[channel].waveforms[wave].interpLastReadPos = 0.0f;

        

        


        // Set frame reading position.
        if (!loadedInstruments[sampleNumber].waveforms[mappedWave].continueNote || instrumentChanged)
        {
            //if (channels[channel].waveforms[wave].reverseRead)
            //    channels[channel].waveforms[wave].sampleReadPos = loadedInstruments[sampleNumber].waveforms[mappedWave].pcmFrames.size() - 1;
            //else
            channels[channel].waveforms[wave].sampleReadPos = 0;


            //if (channels[channel].waveforms[wave].reverseRead)
            //    channels[channel].waveforms[wave].sampleReadPos -= startFrame;
            //else
            channels[channel].waveforms[wave].sampleReadPos += startFrame;


            // Make sure that the sample reading position is inside the sample.
            channels[channel].waveforms[wave].wrapReadPos(loadedInstruments[sampleNumber].waveforms[mappedWave].loopType,
                loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopStart, loadedInstruments[channels[channel].instrument].waveforms[mappedWave].loopEnd);
        }

        

        // Set envelope position.
        channels[channel].waveforms[wave].currentEnvelopeIndex = -1;
        channels[channel].waveforms[wave].envelopePos = 0.0f;

        channels[channel].waveforms[wave].currentEnvelopeAmp = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].envelopeStartAmp;
        channels[channel].waveforms[wave].currentEnvelopePos = 0.0f;

        if (loadedInstruments[channels[channel].instrument].waveforms[mappedWave].envelopePoints.size() > 0)
        {
            channels[channel].waveforms[wave].nextEnvelopeAmp = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].envelopePoints[0].amp;
            channels[channel].waveforms[wave].nextEnvelopePos = loadedInstruments[channels[channel].instrument].waveforms[mappedWave].envelopePoints[0].position;
        }
        else
        {
            channels[channel].waveforms[wave].nextEnvelopeAmp = channels[channel].waveforms[wave].currentEnvelopeAmp;
            channels[channel].waveforms[wave].nextEnvelopePos = 10000.0f;
        }
        

        channels[channel].waveforms[wave].envelopeSpeed = 1.0f;
    }

    channels[channel].noteStopped = false;
    for (int wave = 0; wave < 4; wave++) // Restart release timers.
    {
        channels[channel].waveforms[wave].releaseTimer = 0.0f;
    }

    

    if (channels[channel].volume > 1)
        channels[channel].volume = 1;
    else if (channels[channel].volume < 0)
        channels[channel].volume = 0;



    return;
}







void updateSongOnBeat()
{
    // The interface has changed, and must be redrawn.
    gui.drawFrameThisFrame = true;

    loadedSong.currentNote++;



    int frameToJumpTo = -1; // The frame to jump to if jumping to a frame this cycle. This allows all effects to be played before a frame jump happens.


    for (int ch = 0; ch < 8;ch++)
    {
        if (channels[ch].loopAtEnd) // Loop channel pattern
        {
            int rowCount = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].rows;

            if (loadedSong.currentNote % rowCount == 0)
            {
                loadedSong.noteChannelIndex[ch] = 0;
                loadedSong.volumeChannelIndex[ch] = 0;
                loadedSong.effectChannelIndex[ch] = 0;
            }
        }
    }


    if (loadedSong.currentNote >= loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].rows) // Start new frame.
    {

        loadedSong.currentPattern++;
        loadedSong.currentNote = 0;

        if (loadedSong.currentPattern >= loadedSong.patternSequence.size()) // Restart song.
        {
            loadedSong.currentPattern = 0;
            loadedSong.timeInSong = 0.0f;
            if (editor.recordingSong) // Stop recording.
            {
                editor.recordingSong = false;
            }
        }


        loadCurrentPattern();



        for (int ch = 0; ch < 8; ch++)
        {
            loadedSong.noteChannelIndex[ch] = 0;
            loadedSong.volumeChannelIndex[ch] = 0;
            loadedSong.effectChannelIndex[ch] = 0;
            loadedSong.toNextChannelNote[ch] = 0;
            loadedSong.toNextChannelVolume[ch] = 0;
            loadedSong.toNextChannelEffect[ch] = 0;
        }
    }


    // Delay note.
    /*
    if (channels[channel].frameOffset > 0)
    {

        if (resampleIndex > channels[channel].jumpPoint + channels[channel].frameOffset)
        {
            std::cout << "a";
            channels[channel].frameOffset = 0;


            StartSample(channel, channels[channel].offsetInstrument, channels[channel].offsetNote, channels[channel].jumpPoint);
            //lastFrameReadPos = int(channels[channel].waveforms[op].frameReadPos);
            //float reaBetweenFrames = channels[channel].waveforms[op].frameReadPos - lastFrameReadPos;
            //channels[channel].waveforms[op].frameReadPos = channels[channel].jumpPoint;
            //channels[channel].waveforms[op].frameReadPos = channels[channel].jumpPoint + reaBetweenFrames;
            //if (channels[channel].waveforms[op].frameReadPos > loadedInstruments[channels[channel].instrument].waveforms[waveForm].pcmFrames.size())
            //    channels[channel].waveforms[op].frameReadPos = loadedInstruments[channels[channel].instrument].waveforms[waveForm].pcmFrames.size() - 1;

            //nextFrameReadPos = int(channels[channel].waveforms[op].frameReadPos) + 1.0f;
            resampleIndex = channels[channel].waveforms[op].frameReadPos;
        }
        /*
        channels[channel].frameOffset -= frameCount;
        int framesLeft = -channels[channel].frameOffset;

        if (framesLeft > 0) // Start the new note.
        {
            if (channelInitialized[channel])
                readWithFMAlgorithm(pOutputF32, frameCount - framesLeft, channel, frameOffset);
            frameOffset += frameCount - framesLeft;
            StartSample(channel, channels[channel].offsetInstrument, channels[channel].offsetNote, channels[channel].offsetPos);
            readWithFMAlgorithm(pOutputF32, framesLeft, channel, frameOffset);
        }
        else if (channelInitialized[channel])
            readWithFMAlgorithm(pOutputF32, frameCount, channel, frameOffset);
        
    }*/


    // Read note data an display notes.
    for (int ch = 0; ch < 8; ch++)
    {
        int newFrame = updateChannelOnBeat(ch);
        if (newFrame != -1)
            frameToJumpTo = newFrame;
    }

    /*
    for (int ch = 0; ch < 8; ch++) // Play notes.
    {
        if (loadedSong.currentNote % 4 == 0)
            StartSample(ch, 0, 32);
    }*/

    if (frameToJumpTo > -1)
    {
        if (frameToJumpTo >= loadedSong.patternSequence.size())
            frameToJumpTo = loadedSong.patternSequence.size() - 1;
        loadedSong.currentPattern = frameToJumpTo;
        loadedSong.currentNote = 0;

        loadCurrentPattern();

        // Reset frame values.
        loadedSong.timeInNote = 0.0f;
        loadedSong.timeInSong = 0.0f;
        gui.patternScroll = 0;


        for (int ch2 = 0; ch2 < 8; ch2++)
        {
            loadedSong.noteChannelIndex[ch2] = 0;
            loadedSong.volumeChannelIndex[ch2] = 0;
            loadedSong.effectChannelIndex[ch2] = 0;
            loadedSong.toNextChannelNote[ch2] = 0;
            loadedSong.toNextChannelVolume[ch2] = 0;
            loadedSong.toNextChannelEffect[ch2] = 0;
        }
    }

    return;
}



int updateChannelOnBeat(int ch)
{
    int frameToJumpTo = -1;

    //bool restartFrame = false;




    



    // Find distance to first note, volume and effect.
    int noteIndex = loadedSong.noteChannelIndex[ch];
    int volumeIndex = loadedSong.volumeChannelIndex[ch];
    int effectIndex = loadedSong.effectChannelIndex[ch];




    
    



    if (loadedSong.noteChannelIndex[ch] == 0) // Initial "to next note" at the start of each frame.
    {
        loadedSong.toNextChannelNote[ch] = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes[0];
        loadedSong.noteChannelIndex[ch]++;
        noteIndex++;
        loadedSong.toNextChannelVolume[ch] = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].volumes[0];
        loadedSong.volumeChannelIndex[ch]++;
        volumeIndex++;
        loadedSong.toNextChannelEffect[ch] = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].effects[0];
        loadedSong.effectChannelIndex[ch]++;
        effectIndex++;

        
        //int patternLength = loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].rows;

        //if (loadedSong.currentNote >= patternLength)
        //    loadedSong.currentNote = 0;
    }


    loadedSong.toNextChannelNote[ch]--;
    loadedSong.toNextChannelVolume[ch]--;
    loadedSong.toNextChannelEffect[ch]--;


    



    if (loadedSong.toNextChannelVolume[ch] < 0) // Read next volume.
    {
        if (volumeIndex < loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].volumes.size())
        {
            int volume = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].volumes[volumeIndex];
            loadedSong.volumeChannelIndex[ch]++;
            volumeIndex++;

            channels[ch].volume = float(volume) / 255.0f;


            // Set distance to next note.
            if (volumeIndex < loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].volumes.size())
            {
                loadedSong.toNextChannelVolume[ch] = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].volumes[volumeIndex];
                loadedSong.volumeChannelIndex[ch]++;
            }
            else
                loadedSong.toNextChannelVolume[ch] = 255; // No more notes in this channel in the frame.
        }
    }
    
    
    if (loadedSong.toNextChannelNote[ch] < 0)
    {
        if (noteIndex < loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes.size()) // Stop notes do not reset notes.
        {
            int note = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes[noteIndex];
            if (note != 255)
                channels[ch].resetChannelEffects(false);
        }
    }



    ma_uint64 sampleStartOffset = 0;


    if (loadedSong.toNextChannelEffect[ch] < 0) // Read next effect.
    {
        if (effectIndex < loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].effects.size())
        {
            int effect = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].effects[effectIndex];
            loadedSong.effectChannelIndex[ch]++;
            effectIndex++;


            int effectType = effect / 16;
            int effectVal = effect % 16;


            if (effectType < 5 && effectType > 0) // Set operator volume. (1st param = operator, 2nd param = volume)
            {

                for (int i = 0; i < 4; i++) // Apply the effect to all operators mapped to this wave.
                {
                    int mappedWave = loadedInstruments[channels[ch].instrument].operatorMapping[i];
                    if (mappedWave == effectType - 1)
                        channels[ch].waveforms[i].volume = float(effectVal) / 15.0f;
                }
            }
            else if (effectType == 10) // Increase pitch.
                channels[ch].pitchSlide = float(effectVal) / 100.0f;
            else if (effectType == 11) // Decrease pitch.
                channels[ch].pitchSlide = float(effectVal) / -100.0f;
            else if (effectType == 12) // Sample jump
            {
                if (effectVal < loadedInstruments[channels[ch].instrument].jumpPoints.size())
                {
                    effectVal = loadedInstruments[channels[ch].instrument].jumpPoints.size() - effectVal - 1;

                    for (int wave = 0; wave < 4; wave++)
                    {
                        int mappedWave = loadedInstruments[channels[ch].instrument].operatorMapping[wave];
                        channels[ch].waveforms[wave].sampleReadPos = loadedInstruments[channels[ch].instrument].jumpPoints[effectVal];
                        channels[ch].waveforms[wave].wrapReadPos(loadedInstruments[channels[ch].instrument].waveforms[mappedWave].loopType,
                            loadedInstruments[channels[ch].instrument].waveforms[mappedWave].loopStart, loadedInstruments[channels[ch].instrument].waveforms[mappedWave].loopEnd);
                        sampleStartOffset = channels[ch].waveforms[wave].sampleReadPos;
                        channels[ch].jumpPoint = sampleStartOffset;
                    }
                }
            }
            else if (effectType == 13) // Delay note.
            {
                float fInBeat = (60000.0f / (loadedSong.bpm * 4.0f)) * 48.0f;
                channels[ch].patternOffset = fInBeat * (float(effectVal) / 16.0f);
                //std::cout << "  " << channels[ch].frameOffset;
            }
            else if (effectType == 14) // Retrigger.
            {
                channels[ch].retrigger = float(effectVal * 2);
            }




            // Set distance to next note.
            if (effectIndex < loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].effects.size())
            {
                loadedSong.toNextChannelEffect[ch] = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].effects[effectIndex];
                loadedSong.effectChannelIndex[ch]++;
            }
            else
                loadedSong.toNextChannelEffect[ch] = 255; // No more notes in this channel in the frame.
        }
    }




    

    if (loadedSong.toNextChannelNote[ch] < 0) // Read next note.
    {
        if (noteIndex < loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes.size())
        {
            int note = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes[noteIndex];
            loadedSong.noteChannelIndex[ch]++;
            noteIndex++;
            //channels[ch].pitch = note;

            int instrument = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes[noteIndex];
            loadedSong.noteChannelIndex[ch]++;
            noteIndex++;

            // Set distance to next note.
            if (noteIndex < loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes.size())
            {
                float pastNote = loadedSong.toNextChannelNote[ch];
                loadedSong.toNextChannelNote[ch] = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes[noteIndex];
                loadedSong.toNextChannelNote[ch] += pastNote + 1;
                loadedSong.noteChannelIndex[ch]++;
            }
            else
            {
                loadedSong.toNextChannelNote[ch] = 255; // No more notes in this channel in the frame.
            }

            if (note == 255) // Stop note.
            {
                channels[ch].noteStopped = true;
                /*
                for (int wave = 0; wave < 4; wave++)
                {
                    channels[ch].waveforms[wave].envelopePos = loadedInstruments[channels[ch].instrument].waveforms[wave].attack + loadedInstruments[channels[ch].instrument].waveforms[wave].sustain * 2.0f;
                }*/
            }
            else
            {
                channels[ch].offsetNote = note;
                channels[ch].offsetInstrument = instrument;
                channels[ch].offsetPos = sampleStartOffset;
                channels[ch].retriggerTimer = 0;

                for (int wave = 0; wave < 4; wave++)
                {
                    channels[ch].waveforms[wave].note = note;
                }

                float randVal = float(rand() % 4000) * loadedInstruments[channels[ch].instrument].scatter;
                channels[ch].patternOffset += int(randVal);

                if (channels[ch].patternOffset == 0) // Start offset note.
                    StartNote(ch, instrument, note, sampleStartOffset);
            }
        }
    }



    



    /*
    if (restartFrame)
    {
        loadedSong.noteChannelIndex[ch] = 0;
        loadedSong.volumeChannelIndex[ch] = 0;
        loadedSong.effectChannelIndex[ch] = 0;
        loadedSong.toNextChannelNote[ch] = 0;
        loadedSong.toNextChannelVolume[ch] = 0;
        loadedSong.toNextChannelEffect[ch] = 0;

        updateChannelOnBeat(ch);
    }*/

    return frameToJumpTo;
}



void RecordSong()
{
    if (editor.playingSong)
        StartOrStopSong();
    saveCurrentPattern();
    while (loadedSong.patternSequence[loadedSong.currentPattern] >= loadedSong.patterns.size()) // Create a new frame when changed to one not yet used.
    {
        PatternIndexTable newFrame;
        //newFrame.beatsPerMeasure = loadedPattern.beatsPerMeasure;
        loadedSong.patterns.emplace_back(newFrame);
    }
    loadedSong.currentPattern = 0;
    loadCurrentPattern();


    StartOrStopSong();


    std::string fileName = fileNavigator.currentFilePath.std::filesystem::path::string() + "/" + loadedSong.songName + ".wav";
    const char* name = &fileName[0];
    ma_encoder_init_file(name, &encoderConfig, &encoder);

    

    editor.recordingSong = true;

    while (editor.recordingSong)
    {
        float frameCount = 480;

        float pOut[960] = { 0.0f };
        float* pOutputF32 = (float*)pOut;
        std::fill(pOutputF32, pOutputF32 + 960, 0.0f);


        ma_uint32 frameOffset = 0;
        ma_uint32 framesToRead = 480;

        float fInOldBeat = loadedSong.timeInNote * 48.0f;

        float elapsedMS = float(frameCount) / 48000.0f * 1000.0f;

        loadedSong.timeInNote += elapsedMS;
        loadedSong.timeInSong += elapsedMS;


        float fInNote = loadedSong.timeInNote * 48.0f;

        float fInBeat = (60000.0f / (loadedSong.bpm * 4.0f)) * 48.0f;

        while (fInNote >= fInBeat)
        {
            float fInThisBeat = fInBeat - fInOldBeat;
            PlayChannels(pOutputF32, fInThisBeat, frameOffset);
            frameOffset += fInThisBeat;
            fInOldBeat = 0.0f;
            fInNote -= fInBeat;
            loadedSong.timeInNote -= fInBeat / 48.0f;
            framesToRead -= fInThisBeat;
            updateSongOnBeat();
        }

        PlayChannels(pOutputF32, framesToRead, frameOffset);


        ma_uint64 framesWritten;
        ma_encoder_write_pcm_frames(&encoder, pOutputF32, 480, &framesWritten); // Write frames to file if recording.


    }

    
    
    StartOrStopSong();


    ma_encoder_uninit(&encoder);

    

    return;
}




void StartOrStopSong()
{


    /*
    if (editor.recordingSong && !editor.playingSong) // Set up recording.
    {
        std::string fileName = "Export/" + loadedSong.songName + ".wav";
        const char* name = &fileName[0];
        ma_encoder_init_file(name, &encoderConfig, &encoder);
    }*/

    editor.playingSong = !editor.playingSong;
    loadedSong.bpm = loadedSong.startingBPM;


    saveCurrentPattern();

    for (int ch = 0; ch < 8; ch++)
    {
        channels[ch].playing = false;
    }

    loadedSong.currentNote = -1;
    loadedSong.timeInNote = (60000.0f / (loadedSong.bpm * 4.0f));
    loadedSong.timeInSong = 0.0f;
    for (int ch = 0; ch < 8; ch++)
    {
        loadedSong.noteChannelIndex[ch] = 0;
        loadedSong.volumeChannelIndex[ch] = 0;
        loadedSong.effectChannelIndex[ch] = 0;
        loadedSong.toNextChannelNote[ch] = 0;
        loadedSong.toNextChannelVolume[ch] = 0;
        loadedSong.toNextChannelEffect[ch] = 0;





        for (int wave = 0; wave < 4; wave++)
        {
            channels[ch].waveforms[wave].glideVolume = 0.0f;
            channels[ch].waveforms[wave].note = 0;
            channels[ch].waveforms[wave].volume = 1.0f;

            
            // Reset low-pass values.
            channels[ch].waveforms[wave].y1 = 0.0f;
            channels[ch].waveforms[wave].y2 = 0.0f;
            channels[ch].waveforms[wave].y3 = 0.0f;
            channels[ch].waveforms[wave].y4 = 0.0f;
            channels[ch].waveforms[wave].oldx = 0.0f;
            channels[ch].waveforms[wave].oldy1 = 0.0f;
            channels[ch].waveforms[wave].oldy2 = 0.0f;
            channels[ch].waveforms[wave].oldy3 = 0.0f;

            if (loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes.size() > 1)
            {
                // Set pitch.
                float pitch = loadedSong.channelPatterns[ch].patterns[loadedSong.patterns[loadedSong.patternSequence[loadedSong.currentPattern]].channelPatterns[ch]].notes[1];
                pitch -= loadedSong.edo * 4;
                pitch = pow(2, pitch / loadedSong.edo);
                channels[ch].waveforms[wave].pitch = pitch;
            }
            else
                channels[ch].waveforms[wave].pitch = 0.0f;
        }


        channels[ch].resetChannelEffects(true);

    }
}




void DrawSampleDisplay()
{
    if (sampleDisplay.displayType == 1)
    {
        DrawEnvelopeDisplay();
        return;
    }

    if (editor.selectedInstrument < 0 || !loadedInstruments[editor.selectedInstrument].enabled)
        return;


    float loopStartPos = float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopStart) / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
    float loopEndPos = float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopEnd) / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
    float startPos = float(sampleDisplay.sampleStartPos) / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
    float selectionEndPos = float(sampleDisplay.sampleSelectionEnd) / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());



    if (sampleDisplay.zoomed)
    {
        float zoomStart = float(sampleDisplay.zoomStart) / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
        float zoomEnd = float(sampleDisplay.zoomEnd) / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());
        float zoomScale = (zoomEnd - zoomStart);

        loopStartPos -= zoomStart;
        loopStartPos /= zoomScale;

        loopEndPos -= zoomStart;
        loopEndPos /= zoomScale;

        startPos -= zoomStart;
        startPos /= zoomScale;

        selectionEndPos -= zoomStart;
        selectionEndPos /= zoomScale;
    }

    loopStartPos *= 528.0f;
    loopEndPos *= 528.0f;
    startPos *= 528.0f;
    selectionEndPos *= 528.0f;


    if (selectionEndPos < startPos)
        std::swap(startPos, selectionEndPos);


    if (loopStartPos > 527)
        loopStartPos = 527;
    else if (loopStartPos < 0)
        loopStartPos = 0;

    if (loopEndPos > 527)
        loopEndPos = 527;
    else if (loopEndPos < 0)
        loopEndPos = 0;

    if (startPos > 527)
        startPos = 527;
    else if (startPos < 0)
        startPos = 0;

    if (selectionEndPos > 527)
        selectionEndPos = 527;
    else if (selectionEndPos < 0)
        selectionEndPos = 0;

    


    for (int x = 0; x < 528; x++)
    {

        float brightness = 1.0f;

        if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopType > 0)
        {
            if (x > loopStartPos && x < loopEndPos)
                brightness *= 0.25f;
        }

        
        int subdivision = int((float(x) / 528.0f) * (sampleDisplay.snapSubdivisions));
        bool lighten = subdivision % 2 == 0;

        for (int y = 0; y < 192 - 16; y++)
        {
            sampleDisplay.pixelData[x + 528 * y] = { 0, 0, 0 };

            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;

            if (x > startPos && x < selectionEndPos)
            {
                if (lighten)
                {
                    sampleDisplay.pixelData[x + 528 * y].r *= 0.8f;
                    sampleDisplay.pixelData[x + 528 * y].g *= 0.8f;
                    sampleDisplay.pixelData[x + 528 * y].b *= 0.8f;
                }
            }
            else
            {
                if (lighten)
                {
                    sampleDisplay.pixelData[x + 528 * y].r *= 0.5f;
                    sampleDisplay.pixelData[x + 528 * y].g *= 0.5f;
                    sampleDisplay.pixelData[x + 528 * y].b *= 0.5f;
                }
                else
                {
                    sampleDisplay.pixelData[x + 528 * y].r *= 0.25f;
                    sampleDisplay.pixelData[x + 528 * y].g *= 0.25f;
                    sampleDisplay.pixelData[x + 528 * y].b *= 0.25f;
                }
            }
        }
        for (int y = 192 - 16; y < 192; y++)
        {
            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;
        }
        for (int y = 192 - 16 + 4; y < 192 - 4; y++)
        {
            sampleDisplay.pixelData[x + 528 * y] = { 100, 100, 0 };
        }
        for (int y = 0; y < 16; y++)
        {
            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;
        }
        for (int y = 4; y < 16 - 4; y++)
        {
            if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopType != 0)
            {
                if (x > float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopStart) * (528.0f / loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()) && x < float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopEnd) * (528.0f / loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size()))
                {
                    sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[39] * 255.0f;
                    sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[40] * 255.0f;
                    sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[41] * 255.0f;
                }
            }
        }
    }




    if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() > 0)
    {

        float lastFrameVal = 0;


        for (int x = 0; x < 528; x++)
        {
            int frameIndex = (float(x) / 528.0f) * (((loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())));

            
            if (sampleDisplay.zoomed)
            {
                float zoomStart = sampleDisplay.zoomStart;
                float zoomEnd = sampleDisplay.zoomEnd;
                float zoomScale = (zoomEnd - zoomStart) / loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size();

                frameIndex *= zoomScale;
                frameIndex += zoomStart;
            }


            if (frameIndex >= loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size())
                break;

            int frameVal = int(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames[frameIndex] * 96.0f) + 96.0f;

            if (frameVal > 191)
                frameVal = 191;
            else if (frameVal < 0)
                frameVal = 0;

            

            float brightness = 127.0f;

            if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopType > 0)
            {
                if (x > loopStartPos && x < loopEndPos)
                    brightness *= 0.5f;
            }

            

            if (frameVal > 96)
            {
                for (int y = 96; y < frameVal; y++)
                {
                    float t = float(y - 96) / float(frameVal - 96);
                    sampleDisplay.pixelData[x + 528 * y].r = sampleDisplay.pixelData[x + 528 * y].r * (1.0f - t) + gui.uiColors[48] * 255.0f * t;
                    sampleDisplay.pixelData[x + 528 * y].g = sampleDisplay.pixelData[x + 528 * y].g * (1.0f - t) + gui.uiColors[49] * 255.0f * t;
                    sampleDisplay.pixelData[x + 528 * y].b = sampleDisplay.pixelData[x + 528 * y].b * (1.0f - t) + gui.uiColors[50] * 255.0f * t;
                }
            }
            else
            {
                for (int y = frameVal; y < 96; y++)
                {
                    float t = float(96 - y) / float(96 - frameVal);
                    sampleDisplay.pixelData[x + 528 * y].r = sampleDisplay.pixelData[x + 528 * y].r * (1.0f - t) + gui.uiColors[48] * 255.0f * t;
                    sampleDisplay.pixelData[x + 528 * y].g = sampleDisplay.pixelData[x + 528 * y].g * (1.0f - t) + gui.uiColors[49] * 255.0f * t;
                    sampleDisplay.pixelData[x + 528 * y].b = sampleDisplay.pixelData[x + 528 * y].b * (1.0f - t) + gui.uiColors[50] * 255.0f * t;
                }
            }

            lastFrameVal = frameVal;
        }


        // Draw jump points.
        for (int i = 0; i < loadedInstruments[editor.selectedInstrument].jumpPoints.size(); i++)
        {
            int samplePos = (loadedInstruments[editor.selectedInstrument].jumpPoints[i]) * (528.0f / loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size());

            for (int y = 0; y < 16; y++)
            {
                for (int x = samplePos - abs(y * 0.5); x < samplePos + abs(y * 0.5) + 1; x++)
                {
                    if (x > 0 && x < 528)
                        sampleDisplay.pixelData[x + 528 * (y + 175)] = { 255, 255, 0 };
                }
            }
            for (int y = 0; y < 175; y++)
            {
                sampleDisplay.pixelData[samplePos + 528 * y] = { 255, 255, 0 };
            }
        }

        // Draw loop points.
        if (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].loopType > 0)
        {
            for (int y = 0; y < 16; y++)
            {
                for (int x = loopStartPos - abs(y * 0.5); x < loopStartPos + abs(y * 0.5) + 1; x++)
                {
                    if (x > 0 && x < 528)
                        sampleDisplay.pixelData[x + 528 * (15 - y)] = { 0, 255, 255 };
                }
            }
            if (loopStartPos < 528)
            {
                for (int y = 0; y < 175; y++)
                {
                    sampleDisplay.pixelData[int(loopStartPos) + 528 * y] = { 0, 255, 255 };
                }
            }

            for (int y = 0; y < 16; y++)
            {
                for (int x = loopEndPos - abs(y * 0.5); x < loopEndPos + abs(y * 0.5) + 1; x++)
                {
                    if (x > 0 && x < 528)
                        sampleDisplay.pixelData[x + 528 * (15 - y)] = { 0, 255, 255 };
                }
            }
            if (loopEndPos < 528)
            {
                for (int y = 0; y < 175; y++)
                {
                    sampleDisplay.pixelData[int(loopEndPos) + 528 * y] = { 0, 255, 255 };
                }
            }
        }


        // Draw the position marker.
        for (int y = 0; y < 8; y++)
        {
            for (int x = startPos - abs(y * 0.5); x < startPos + abs(y * 0.5) + 1; x++)
            {
                if (x > 0 && x < 528)
                {
                    sampleDisplay.pixelData[x + 528 * (y + 175)] = { 255, 255, 255 };
                    sampleDisplay.pixelData[x + 528 * (15 - y)] = { 255, 255, 255 };
                }
            }

            for (int x = selectionEndPos - abs(y * 0.5); x < selectionEndPos + abs(y * 0.5) + 1; x++)
            {
                if (x > 0 && x < 528)
                {
                    sampleDisplay.pixelData[x + 528 * (y + 175)] = { 255, 255, 255 };
                    sampleDisplay.pixelData[x + 528 * (15 - y)] = { 255, 255, 255 };
                }
            }
        }
        for (int y = 0; y < 192; y++)
        {
            sampleDisplay.pixelData[int(startPos) + 528 * y] = { 255, 255, 255 };
            sampleDisplay.pixelData[int(selectionEndPos) + 528 * y] = { 255, 255, 255 };
        }
    }
    else
    {
        for (int x = 0; x < 528; x++)
        {
            for (int y = 192 - 16; y < 192; y++)
            {
                sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * (y - 192 + 16) * 16;
                sampleDisplay.pixelData[x + 528 * y].g = 0;
                sampleDisplay.pixelData[x + 528 * y].b = 0;
            }
        }
    }

    

    return;
}



void DrawEnvelopeDisplay()
{
    if (sampleDisplay.displayType != 1)
        return;

    if (editor.selectedInstrument < 0 || !loadedInstruments[editor.selectedInstrument].enabled)
        return;





    for (int x = 0; x < 528; x++)
    {

        float brightness = 1.0f;


        int subdivision = int((float(x) / 528.0f) * (loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength));
        bool lighten = subdivision % 2 == 0;

        for (int y = 0; y < 192 - 16; y++)
        {
            sampleDisplay.pixelData[x + 528 * y] = { 0, 0, 0 };

            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;

            if (lighten)
            {
                sampleDisplay.pixelData[x + 528 * y].r *= 0.5f;
                sampleDisplay.pixelData[x + 528 * y].g *= 0.5f;
                sampleDisplay.pixelData[x + 528 * y].b *= 0.5f;
            }
            else
            {
                sampleDisplay.pixelData[x + 528 * y].r *= 0.25f;
                sampleDisplay.pixelData[x + 528 * y].g *= 0.25f;
                sampleDisplay.pixelData[x + 528 * y].b *= 0.25f;
            }
        }
        for (int y = 192 - 32; y < 192; y++)
        {
            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;
        }
        for (int y = 192 - 16 + 4; y < 192 - 4; y++)
        {
            sampleDisplay.pixelData[x + 528 * y] = { 100, 100, 0 };
        }
        for (int y = 0; y < 32; y++)
        {
            sampleDisplay.pixelData[x + 528 * y].r = gui.uiColors[3] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].g = gui.uiColors[4] * 255.0f;
            sampleDisplay.pixelData[x + 528 * y].b = gui.uiColors[5] * 255.0f;
        }
    }



    // Draw points.
    float amp = loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeStartAmp;
    float nextAmp = amp;

    int pos = 0;
    int nextPos = 0;
    int posIndex = 0;
    int interpAmp = 0;

    for (int x = 0; x < 528; x++)
    {
        if (x > nextPos)
        {
            

            amp = nextAmp;
            pos = nextPos;
            if (posIndex < loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints.size())
            {
                nextPos = float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints[posIndex].position)
                    * (528.0f / float(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopeLength));
                nextAmp = loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].envelopePoints[posIndex].amp;
                posIndex++;
            }
            else
                nextPos = 528;

            float interp = float(x - pos) / float(nextPos - pos);
            int y = int((amp * (1.0f - interp) + nextAmp * interp) * 128 + 32);

            for (int a = -4; a < 4; a++)
            {
                for (int b = -4; b < 4; b++)
                {
                    if (x + a >= 0 && x + a < 528 && y + b >= 0 && y + b < 192)
                    {
                        sampleDisplay.pixelData[x + a + 528 * (y + b)].r = gui.uiColors[51] * 255.0f;
                        sampleDisplay.pixelData[x + a + 528 * (y + b)].g = gui.uiColors[52] * 255.0f;
                        sampleDisplay.pixelData[x + a + 528 * (y + b)].b = gui.uiColors[53] * 255.0f;
                    }
                }
            }
        }

        float interp = float(x - pos) / float(nextPos - pos);
        interpAmp = int((amp * (1.0f - interp) + nextAmp * interp) * 128 + 32);

        for (int y = 32; y < interpAmp; y++)
        {
            float t = float(y - 32) / float(interpAmp - 32);
            sampleDisplay.pixelData[x + 528 * y].r = sampleDisplay.pixelData[x + 528 * y].r * (1.0f - t) + gui.uiColors[48] * 255.0f * t;
            sampleDisplay.pixelData[x + 528 * y].g = sampleDisplay.pixelData[x + 528 * y].g * (1.0f - t) + gui.uiColors[49] * 255.0f * t;
            sampleDisplay.pixelData[x + 528 * y].b = sampleDisplay.pixelData[x + 528 * y].b * (1.0f - t) + gui.uiColors[50] * 255.0f * t;
        }
    }

    


    return;
}



void GenerateAdditiveWave(Instrument* instrument, int op)
{
    //std::cout << "< ";
    if (!instrument->enabled) // Create a new sample.
    {
        instrument->enabled = true;
    }


    if (instrument->waveforms[op].waveType == 4) // Generate noise
    {
        int sampleLen = 480 * instrument->waveforms[op].numOfSineWaves * instrument->waveforms[op].numOfSineWaves;

        instrument->waveforms[op].pcmFrames.clear();
        instrument->waveforms[op].pcmFrames.resize(sampleLen);
        std::fill(instrument->waveforms[op].pcmFrames.begin(), instrument->waveforms[op].pcmFrames.begin() + sampleLen, 0.0f);


        for (int wave = 0; wave < 11; wave++)
        {
            if (instrument->waveforms[op].frequencies[wave] > 0)
            {
                srand(0);

                float noiseVol1 = float((rand() % 256) - 127) / 128.0f;
                float noiseVol2 = float((rand() % 256) - 127) / 128.0f;
                float index = 0.0f;

                

                for (int x = 0; x < sampleLen; x++)
                {
                    if (int(index + 1.0f / (float(16 - wave))) != int(index))
                    {
                        noiseVol1 = noiseVol2;
                        noiseVol2 = float((rand() % 256) - 127) / 128.0f;
                    }

                    index += 1.0f / (float(16 - wave));
                    float interp = index - int(index);

                    float interpVol = noiseVol1 * (1.0f - interp) + noiseVol2 * interp;

                    interpVol *= (instrument->waveforms[op].frequencies[wave] + 2.0f) / 16.0f;

                    if (x >= instrument->waveforms[op].dutyCycle * float(sampleLen)) // Duty cycle
                        interpVol = 0.0f;

                    instrument->waveforms[op].pcmFrames[x] += interpVol;
                }
            }
        }


        //loadedSamples[editor.selectedSample].waveforms[sampleDisplay.selectedOperator].pcmFrames.data()

        //float vol = 0;

        //float index = float(x) / (float(16) * 1.0f);
        //float index2 = index + 1;
        //while (index2 >= noiseFrames.size())
        //    index2 -= noiseFrames.size();

        //float interp = index - int(index);

        //float interpVol = noiseFrames[index] * (1.0f - interp) + noiseFrames[index2] * interp;

        //interpVol *= frequencies[frequency] * 0.1f;

        //vol += interpVol;

        //inputWave[x] += vol;
    }
    else
    {
        // Generate additive wave. (Only uses the base frequency at full volume.)
        if (instrument->waveforms[op].operatorType == 2)
        {
            float waveTotal = 48000.0f / 261.625f;


            ma_uint64 framesToWrite = ma_uint64(waveTotal);

            // G

            // C - C# - D - D# - E - F - F# - G - G# - A - A# - B - C

            // *= ((12 + 7) / 12)

            instrument->waveforms[op].pcmFrames.clear();
            instrument->waveforms[op].pcmFrames.assign(framesToWrite, 0.0f);

            float waveSize = 48000.0f / 261.625f;
            waveSize *= 2.0f;

            //std::cout << "  " << waveSize;

            float freqs[11] = {
                0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
            };

            ConstructWave(instrument, op, instrument->waveforms[op].waveType, freqs, framesToWrite, waveSize, 3, instrument->waveforms[op].pcmFrames.data());

            for (int x = 0; x < instrument->waveforms[op].pcmFrames.size(); x++)
                instrument->waveforms[op].pcmFrames[x] += (instrument->waveforms[op].offset - 0.5f) * 2.0f;



            instrument->waveforms[op].loopStart = 0;
            instrument->waveforms[op].loopEnd = instrument->waveforms[op].pcmFrames.size();

            if (instrument->waveforms[op].reverseFrames)
                std::reverse(instrument->waveforms[op].pcmFrames.begin(), instrument->waveforms[op].pcmFrames.end());






            // Reset all of the frame reading positions since the wave may have changed sizes.
            for (int ch = 0; ch < 8; ch++)
            {
                for (int op = 0; op < 4; op++)
                    channels[ch].waveforms[op].sampleReadPos = 0;
            }

            return;
        }


        // Size of wave = x frames

        // 261.625 = periods per second

        // 48000 frames per second

        // 1 frame = 1/48000 seconds

        // 1 period = 1/261.625 seconds

        // frames per period = (1/261.625) * (1/48000)

        // 1 period = 48000/261.625 frames



        // Find LCM of waves.
        // Then divide by 6720.0f and multiply by the size of the fundamental frequency to find the number of frames to read.


        float freqSizes[11] = {
            3360.0f, 2520.0f, 1680.0f, 840.0f, 420.0f, 280.0f, 210.0f, 168.0f, 140.0f, 120.0f, 105.0f
        };

        int factor7[11] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 };

        int factor5[11] = { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 };

        int factor3[11] = { 1, 2, 1, 1, 1, 0, 1, 1, 0, 1, 1 };

        int factor2[11] = { 8, 6, 7, 6, 5, 6, 4, 6, 5, 6, 3 };

        // Multiply the wave value of all included waves.
        float fact7 = 0.0f;
        float fact5 = 0.0f;
        float fact3 = 0.0f;
        float fact2 = 0.0f;


        for (int i = 0; i < 11; i++)
        {
            if (instrument->waveforms[op].frequencies[i] > 0)
            {
                if (fact7 < factor7[i]) fact7 = factor7[i];
                if (fact5 < factor5[i]) fact5 = factor5[i];
                if (fact3 < factor3[i]) fact3 = factor3[i];
                if (fact2 < factor2[i]) fact2 = factor2[i];
            }
        }

        float waveTotal = pow(7, fact7) * pow(5, fact5) * pow(3, fact3) * pow(2, fact2);
        if (waveTotal < 840.0f * 8.0f) waveTotal = 840.0f * 8.0f;
        waveTotal /= 840.0f;
        waveTotal *= 48000.0f;
        waveTotal /= 261.625f;
        waveTotal /= 8.0f;
        

        // Make sure the waves are not too small, which can cause problems with the tunning.
        //waveTotal *= float(instrument->waveforms[op].periods);

        ma_uint64 framesToWrite = ma_uint64(waveTotal);


        

        if (framesToWrite == 0) // If there are no waves, create empty frames.
        {
            instrument->waveforms[op].pcmFrames.clear();

            for (int fr = 0; fr < 480; fr++)
                instrument->waveforms[op].pcmFrames.emplace_back(instrument->waveforms[op].offset);
        }
        else
        {
            float waveScale = waveTotal / float(framesToWrite);

            // G

            // C - C# - D - D# - E - F - F# - G - G# - A - A# - B - C

            // *= ((12 + 7) / 12)

            instrument->waveforms[op].pcmFrames.clear();
            instrument->waveforms[op].pcmFrames.assign(framesToWrite, 0.0f);

            for (int freq = 0; freq < 11; freq++)
            {
                float waveSize = ((freqSizes[freq] / 840.0f) * 48000.0f) / 261.625f;
                waveSize *= 2.0f;

                //std::cout << "  " << waveSize;

                if (instrument->waveforms[op].frequencies[freq] != 0)
                    ConstructWave(instrument, op, instrument->waveforms[op].waveType, instrument->waveforms[op].frequencies, framesToWrite, waveSize, freq, instrument->waveforms[op].pcmFrames.data());
            }
        }
    }

    for (int x = 0; x < instrument->waveforms[op].pcmFrames.size(); x++)
    {
        instrument->waveforms[op].pcmFrames[x] += (instrument->waveforms[op].offset - 0.5f) * 2.0f;
        if (instrument->waveforms[op].pcmFrames[x] > 0.85f) instrument->waveforms[op].pcmFrames[x] = 0.85f;
        else if (instrument->waveforms[op].pcmFrames[x] < -0.85f) instrument->waveforms[op].pcmFrames[x] = -0.85f;
    }


    
    instrument->waveforms[op].loopStart = 0;
    instrument->waveforms[op].loopEnd = instrument->waveforms[op].pcmFrames.size();

    if (instrument->waveforms[op].reverseFrames)
        std::reverse(instrument->waveforms[op].pcmFrames.begin(), instrument->waveforms[op].pcmFrames.end());
    




    // Reset all of the frame reading positions since the wave may have changed sizes.
    for (int ch = 0; ch < 8; ch++)
    {
        for (int op = 0; op < 4; op++)
            channels[ch].waveforms[op].sampleReadPos = 0;
    }


    return;
}



void ConstructWave(Instrument* instrument, int op, int waveType, float frequencies[16], float framesToWrite, float periodLength, int frequency, float* inputWave)
{
    float periodLen = periodLength;


    


    for (int x = 0; x < framesToWrite; x++)
    {
        if (waveType == -1) // Empty wave
        {
            inputWave[x] += 1.0f;
        }
        else if (waveType == 0) // Sine wave
        {
            float vol = 0;

            float waveLen = float(periodLen * 0.5f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            if (periodPos <= instrument->waveforms[op].dutyCycle)
                vol += sin(float(x) * 2.0f * 6.28312 / periodLen) * frequencies[frequency] * 0.07f;


            inputWave[x] += vol;
        }
        else if (waveType == 1) // Square wave
        {
            float vol = 0;

            float duty = instrument->waveforms[op].dutyCycle;

            if (instrument->waveforms[op].generateFromSines)
            {
                float waveLen = float(periodLen * 0.5f);
                float periodPos = float(x);
                while (periodPos > waveLen)
                    periodPos -= waveLen;
                periodPos /= waveLen;

                if (periodPos <= duty)
                {
                    bool addSign = true;
                    for (int w = 1; w < instrument->waveforms[op].numOfSineWaves * 2; w += 2)
                    {
                        vol += (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                    }
                }
            }
            else
            {
                float round = instrument->waveforms[op].smoothness;

                float waveLen = float(periodLen * 0.5f);

                float periodPos = float(x);

                while (periodPos > waveLen)
                    periodPos -= waveLen;

                periodPos /= waveLen;


                float volume = 1.0f * frequencies[frequency] * 0.07f;


                float c1 = round * (duty * 0.5f);
                float c2 = duty - (round * (duty * 0.5f));
                float c3 = duty;
                float c4 = (round * ((1.0f - duty) * 0.5f)) + duty;
                float c5 = 1.0f - (round * ((1.0f - duty) * 0.5f));

                // Rounded corners
                if (periodPos < c1) // corner
                {
                    float x1 = periodPos - c1;
                    float r1 = c1;
                    float yVal = sqrt((r1 * r1) - (x1 * x1));
                    yVal *= 1.0f * (round * volume) / r1;
                    yVal += volume - (round * volume);
                    vol += yVal;
                }
                else if (periodPos <= c2) // flat
                {
                    vol += volume;
                }
                else if (periodPos < c3) // corner
                {
                    float x1 = periodPos - c2;
                    float r1 = c1;
                    float yVal = sqrt((r1 * r1) - (x1 * x1));
                    yVal *= 1.0f * (round * volume) / r1;
                    yVal += volume - (round * volume);
                    vol += yVal;
                }
                else if (periodPos < c4) // corner
                {
                    float x1 = periodPos - c4;
                    float r1 = c4 - duty;
                    float yVal = sqrt((r1 * r1) - (x1 * x1));
                    yVal *= 1.0f * (round * volume) / r1;
                    yVal += volume - (round * volume);
                    vol -= yVal;
                }
                else if (periodPos <= c5) // flat
                {
                    vol -= volume;
                }
                else // corner
                {
                    float x1 = periodPos - c5;
                    float r1 = 1.0f - c5;
                    float yVal = sqrt((r1 * r1) - (x1 * x1));
                    yVal *= 1.0f * (round * volume) / r1;
                    yVal += volume - (round * volume);
                    vol -= yVal;
                }
            }


            inputWave[x] += vol;
        }
        else if (waveType == 2) // Triangle wave
        {
            float vol = 0;

            float waveLen = float(periodLen * 0.5f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            if (periodPos <= instrument->waveforms[op].dutyCycle)
            {
                if (periodPos > 0.5)
                    vol += float(periodPos - 0.5f - 0.25f) * frequencies[frequency] * 0.07f * 4.0f;
                else
                    vol += (0.25f - float(periodPos)) * frequencies[frequency] * 0.07f * 4.0f;
            }

            inputWave[x] += vol;
        }
        else if (waveType == 3) // Saw wave
        {
            float vol = 0;

            float duty = instrument->waveforms[op].dutyCycle;

            if (instrument->waveforms[op].generateFromSines)
            {
                float waveLen = float(periodLen * 0.5f);
                float periodPos = float(x);
                while (periodPos > waveLen)
                    periodPos -= waveLen;
                periodPos /= waveLen;

                if (periodPos * 1.0f <= duty)
                {
                    bool addSign = true;
                    for (int w = 1; w < instrument->waveforms[op].numOfSineWaves + 1; w++)
                    {
                        if (addSign)
                            vol += (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.05f) / float(w);
                        else
                            vol -= (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.05f) / float(w);
                        addSign = !addSign;
                    }
                }
            }
            else
            {
                float round = instrument->waveforms[op].smoothness * 0.5f;

                float waveLen = float(periodLen * 0.5f);
                float periodPos = float(x);
                while (periodPos > waveLen)
                    periodPos -= waveLen;
                periodPos /= waveLen;

                float amplitude = frequencies[frequency] * 0.2f;

                if (periodPos <= duty)
                {

                    // Approximate y in a Bezier curve.
                    if (periodPos < round)
                    {
                        float t = 0.5f;
                        float interval = 0.5f;
                        for (int j = 0; j < 32; j++)
                        {
                            float estimate = (t * t) * round;
                            interval *= 0.5f;

                            float posInCurve = periodPos;

                            if (estimate > posInCurve)
                                t -= interval;
                            else
                                t += interval;
                        }
                        float roundedVol = -(t * t) * (amplitude * round + amplitude * 0.5f) + t * amplitude + amplitude * 0.5f;


                        vol -= (roundedVol)-amplitude * 0.5f;
                    }
                    else if (periodPos > 1.0f - round)
                    {
                        float t = 0.5f;
                        float interval = 0.5f;
                        for (int j = 0; j < 32; j++)
                        {
                            float estimate = 1.0f - round + 2 * round * t - (t * t) * round;
                            interval *= 0.5f;

                            float posInCurve = periodPos;

                            if (estimate > posInCurve)
                                t -= interval;
                            else
                                t += interval;
                        }
                        float roundedVol = -(t * t) * (amplitude * round + amplitude * 0.5f) + 2 * t * amplitude * round + amplitude - (amplitude * round);

                        vol += (roundedVol)-amplitude * 0.5f;
                    }
                    else
                    {
                        vol += (periodPos * amplitude) - amplitude * 0.5f;
                    }
                }
            }

            inputWave[x] += vol;
        }
        else if (waveType == 5) // Wave A
        {
            float vol = 0;

            float duty = instrument->waveforms[op].dutyCycle;

            float waveLen = float(periodLen * 1.0f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            if (periodPos * 2.0f <= duty)
            {
                bool addSign = true;
                for (int w = 1; w < instrument->waveforms[op].numOfSineWaves * 4; w += 4)
                {
                    if (addSign)
                        vol += (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                    else
                        vol -= (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                    addSign = !addSign;
                }
            }

            inputWave[x] += vol;
        }
        else if (waveType == 6) // Wave B
        {
            float vol = 0;

            float duty = instrument->waveforms[op].dutyCycle;

            float waveLen = float(periodLen * 1.0f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            if (periodPos * 2.0f <= duty)
            {
                bool addSign = true;
                for (int w = 1; w < instrument->waveforms[op].numOfSineWaves * 6; w += 2)
                {
                    if (addSign)
                    {
                        vol += (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                        vol -= abs(cos(periodPos * float(w) * 6.283f * 2.0f) * frequencies[frequency] * 0.07f) / float(w);
                    }
                    else
                    {
                        vol -= (sin(periodPos * float(w) * 6.283f) * frequencies[frequency] * 0.07f) / float(w);
                        vol += abs(cos(periodPos * float(w) * 6.283f * 2.0f) * frequencies[frequency] * 0.07f) / float(w);
                    }
                    addSign = !addSign;
                }
            }

            inputWave[x] += vol;
        }
        else if (waveType == 7) // Wave C
        {
            float vol = 0;

            float waveLen = float(periodLen * 0.5f);
            float periodPos = float(x);
            while (periodPos > waveLen)
                periodPos -= waveLen;
            periodPos /= waveLen;

            


            if (periodPos <= instrument->waveforms[op].dutyCycle)
            {
                vol = sin(float(x) * 2.0f * 4.0f * 6.28312 / periodLen) * frequencies[frequency] * 0.02f;

                if (periodPos < 0.25f)
                {
                    float t = periodPos / 0.25f;
                    vol = -(frequencies[frequency] * 0.08f) * (1.0f - t);
                }
                else if (periodPos >= 0.5f && periodPos < 0.75f)
                {
                    float t = (periodPos - 0.5f) / 0.25f;
                    vol = -(frequencies[frequency] * 0.08f) * (1.0f - t);
                }
            }

            if (periodPos > 0.5f)
                vol *= -1.0f;


            inputWave[x] += vol;

        }
    }

    





    return;
}



void GenerateAllInstrumentWaves(Instrument *instrument)
{
    sampleDisplay.drawing = false; // Stop sample drawing.
    sampleDisplay.zoomed = false; // Reset zoom.
    for (int wave = 0; wave < 4; wave++)
    {
        GenerateAdditiveWave(instrument, wave);
    }
    sampleDisplay.selectedOperator = 0;

    return;
}


void DrawSamplePoint(Vector2 drawWavePos)
{
    if (drawWavePos.x < 0.0f)
        drawWavePos.x = 0.0f;
    else if (drawWavePos.x > loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() - 1)
        drawWavePos.x = loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames.size() - 1;
    if (drawWavePos.y < -1.0f)
        drawWavePos.y = -1.0f;
    else if (drawWavePos.y > 1.0f)
        drawWavePos.y = 1.0f;

    loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames[drawWavePos.x] = drawWavePos.y;
    loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames[int(drawWavePos.x)] = int(loadedInstruments[editor.selectedInstrument].waveforms[sampleDisplay.selectedOperator].pcmFrames[int(drawWavePos.x)] * 128.0f) / 128.0f;
    DrawSampleDisplay();
    return;
}