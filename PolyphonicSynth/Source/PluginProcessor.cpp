/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <iostream>
using namespace std;

AudioProcessor* JUCE_CALLTYPE createPluginFilter();

//==============================================================================
// Sine Wave
class SineWave : public MPESynthesiserVoice
{
public:
    //==============================================================================
    SineWave()
    : phase (0.0), phaseDelta (0.0), tailOff (0.0)
    {
    }

    //==============================================================================
    void noteStarted() override
    {
        jassert (currentlyPlayingNote.isValid());
        jassert (currentlyPlayingNote.keyState == MPENote::keyDown
                 || currentlyPlayingNote.keyState == MPENote::keyDownAndSustained);
        
        // get data from the current MPENote
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
        
        phase = 0.0;
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        
        tailOff = 0.0;
    }
    
    void noteStopped (bool allowTailOff) override
    {
        jassert (currentlyPlayingNote.keyState == MPENote::off);
        
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            
            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearCurrentNote();
            phaseDelta = 0.0;
        }
    }
    
    void notePressureChanged() override
    {
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
    }
    
    void notePitchbendChanged() override
    {
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
    }
    
    void noteTimbreChanged() override
    {
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
    }
    
    void noteKeyStateChanged() override
    {
    }
    
    void setCurrentSampleRate (double newRate) override
    {
        if (currentSampleRate != newRate)
        {
            noteStopped (false);
            currentSampleRate = newRate;
            
            level.reset (currentSampleRate, smoothingLengthInSeconds);
            timbre.reset (currentSampleRate, smoothingLengthInSeconds);
            frequency.reset (currentSampleRate, smoothingLengthInSeconds);
        }
    }
    
    //==============================================================================
    void renderNextBlock (AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override
    {
        if (phaseDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = getNextSample() * (float)tailOff;
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                        ++startSample;
                    tailOff *= 0.99;
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        phaseDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = getNextSample();
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                        ++startSample;
                }
            }
        }
    }
    
private:
    //==============================================================================
    float getNextSample() noexcept
    {
        const double levelDb = (level.getNextValue() - 1.0) * maxLevelDb;
        const double amplitude = std::pow (10.0f, 0.05f * levelDb) * maxLevel;
        
        // timbre is used to blend between a sine and a square.
        const double f1 = std::sin (phase);
        const double a2 = timbre.getNextValue();
        const double a1 = 1.0 - a2;
        
        const float nextSample = float (2*amplitude * (a1 * f1));
        
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        phase = std::fmod (phase + phaseDelta, 2.0 * double_Pi);
        
        return nextSample;
    }
    
    //==============================================================================
    LinearSmoothedValue<double> level, timbre, frequency;
    double phase, phaseDelta, tailOff;
    
    // some useful constants
    const double maxLevel = 1.0f;
    const double maxLevelDb = 31.0f;
    const double smoothingLengthInSeconds = 0.01;
};

//==============================================================================
class SquareWave : public MPESynthesiserVoice
{
public:
    //==============================================================================
    SquareWave()
    : phase (0.0), phaseDelta (0.0), tailOff (0.0)
    {
    }
    
    //==============================================================================
    void noteStarted() override
    {
        jassert (currentlyPlayingNote.isValid());
        jassert (currentlyPlayingNote.keyState == MPENote::keyDown
                 || currentlyPlayingNote.keyState == MPENote::keyDownAndSustained);
        
        // get data from the current MPENote
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
        
        phase = 0.0;
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        
        tailOff = 0.0;
    }
    
    void noteStopped (bool allowTailOff) override
    {
        jassert (currentlyPlayingNote.keyState == MPENote::off);
        
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            
            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearCurrentNote();
            phaseDelta = 0.0;
        }
    }
    
    void notePressureChanged() override
    {
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
    }
    
    void notePitchbendChanged() override
    {
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
    }
    
    void noteTimbreChanged() override
    {
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
    }
    
    void noteKeyStateChanged() override
    {
    }
    
    void setCurrentSampleRate (double newRate) override
    {
        if (currentSampleRate != newRate)
        {
            noteStopped (false);
            currentSampleRate = newRate;
            
            level.reset (currentSampleRate, smoothingLengthInSeconds);
            timbre.reset (currentSampleRate, smoothingLengthInSeconds);
            frequency.reset (currentSampleRate, smoothingLengthInSeconds);
        }
    }
    
    //==============================================================================
    void renderNextBlock (AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override
    {
        if (phaseDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = getNextSample() * (float)tailOff;
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                        ++startSample;
                    tailOff *= 0.99;
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        phaseDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = getNextSample();
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                        ++startSample;
                }
            }
        }
    }
    
private:
    //==============================================================================
    float getNextSample() noexcept
    {
        const double levelDb = (level.getNextValue() - 1.0) * maxLevelDb;
        const double amplitude = std::pow (10.0f, 0.05f * levelDb) * maxLevel;
        
        // timbre is used to blend between a sine and a square.
        const double f1 = std::cos (phase);                        // SQUARE
        const double f2 = std::copysign (1.0, f1);
        const double a2 = timbre.getNextValue();
        
        const float nextSample = float (amplitude * (a2 * f2)); // SQUARE WAVE
        
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        phase = std::fmod (phase + phaseDelta, 2.0 * double_Pi);
        
        return nextSample;
    }
    
    //==============================================================================
    LinearSmoothedValue<double> level, timbre, frequency;
    double phase, phaseDelta, tailOff;
    
    // some useful constants
    const double maxLevel = 1.0f;
    const double maxLevelDb = 31.0f;
    const double smoothingLengthInSeconds = 0.01;
};
// END SQUARE WAVE
//==============================================================================
class TriangleWave : public MPESynthesiserVoice
{
public:
    //==============================================================================
    TriangleWave()
    : phase (0.0), phaseDelta (0.0), tailOff (0.0)
    {
    }
    
    //==============================================================================
    void noteStarted() override
    {
        jassert (currentlyPlayingNote.isValid());
        jassert (currentlyPlayingNote.keyState == MPENote::keyDown
                 || currentlyPlayingNote.keyState == MPENote::keyDownAndSustained);
        
        // get data from the current MPENote
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
        
        phase = 0.0;
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        
        tailOff = 0.0;
    }
    
    void noteStopped (bool allowTailOff) override
    {
        jassert (currentlyPlayingNote.keyState == MPENote::off);
        
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            
            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearCurrentNote();
            phaseDelta = 0.0;
        }
    }
    
    void notePressureChanged() override
    {
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
    }
    
    void notePitchbendChanged() override
    {
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
    }
    
    void noteTimbreChanged() override
    {
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
    }
    
    void noteKeyStateChanged() override
    {
    }
    
    void setCurrentSampleRate (double newRate) override
    {
        if (currentSampleRate != newRate)
        {
            noteStopped (false);
            currentSampleRate = newRate;
            
            level.reset (currentSampleRate, smoothingLengthInSeconds);
            timbre.reset (currentSampleRate, smoothingLengthInSeconds);
            frequency.reset (currentSampleRate, smoothingLengthInSeconds);
        }
    }
    
    //==============================================================================
    void renderNextBlock (AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override
    {
        if (phaseDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = getNextSample() * (float)tailOff;
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                        ++startSample;
                    tailOff *= 0.99;
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        phaseDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = getNextSample();
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                        ++startSample;
                }
            }
        }
    }
    
private:
    //==============================================================================
    float getNextSample() noexcept
    {
        const double levelDb = (level.getNextValue() - 1.0) * maxLevelDb;
        const double amplitude = std::pow (10.0f, 0.05f * levelDb) * maxLevel;
        
        const float nextSample = (float(2 * amplitude) / double_Pi) * asin(sin((phase)));
        
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        phase = std::fmod (phase + phaseDelta, 2.0 * double_Pi);
        
        return nextSample;
    }
    
    //==============================================================================
    LinearSmoothedValue<double> level, timbre, frequency;
    double phase, phaseDelta, tailOff;
    
    // some useful constants
    const double maxLevel = 1.0f;
    const double maxLevelDb = 31.0f;
    const double smoothingLengthInSeconds = 0.01;
};
// END TRIANGLE WAVE
//==============================================================================
class SawtoothWave : public MPESynthesiserVoice
{
public:
    //==============================================================================
    SawtoothWave()
    : phase (0.0), phaseDelta (0.0), tailOff (0.0)
    {
    }
    
    //==============================================================================
    void noteStarted() override
    {
        jassert (currentlyPlayingNote.isValid());
        jassert (currentlyPlayingNote.keyState == MPENote::keyDown
                 || currentlyPlayingNote.keyState == MPENote::keyDownAndSustained);
        
        // get data from the current MPENote
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
        
        phase = 0.0;
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        
        tailOff = 0.0;
    }
    
    void noteStopped (bool allowTailOff) override
    {
        jassert (currentlyPlayingNote.keyState == MPENote::off);
        
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            
            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearCurrentNote();
            phaseDelta = 0.0;
        }
    }
    
    void notePressureChanged() override
    {
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
    }
    
    void notePitchbendChanged() override
    {
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
    }
    
    void noteTimbreChanged() override
    {
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
    }
    
    void noteKeyStateChanged() override
    {
    }
    
    void setCurrentSampleRate (double newRate) override
    {
        if (currentSampleRate != newRate)
        {
            noteStopped (false);
            currentSampleRate = newRate;
            
            level.reset (currentSampleRate, smoothingLengthInSeconds);
            timbre.reset (currentSampleRate, smoothingLengthInSeconds);
            frequency.reset (currentSampleRate, smoothingLengthInSeconds);
        }
    }
    
    //==============================================================================
    void renderNextBlock (AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override
    {
        
        if (phaseDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = getNextSample() * (float)tailOff;
                    
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                        ++startSample;
                    tailOff *= 0.99;
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        phaseDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = getNextSample();
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                        ++startSample;
                }
            }
        }
        //coeff.makeLowPass(44100, 1000);
        //filter.processSamples(outputBuffer.getWritePointer(0), numSamples);
    }
    
private:
    //==============================================================================
    float getNextSample() noexcept
    {
        const double levelDb = (level.getNextValue() - 1.0) * maxLevelDb;
        const double amplitude = std::pow (10.0f, 0.05f * levelDb) * maxLevel;
        
        const float nextSample = (float (-2 * amplitude) / double_Pi) * atan(1/tan((phase)));
        
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = double_Pi * cyclesPerSample;
        phase = std::fmod (phase + phaseDelta, double_Pi);
        
        return nextSample;
    }
    
    //==============================================================================
    LinearSmoothedValue<double> level, timbre, frequency;
    double phase, phaseDelta, tailOff;
    
    // some useful constants
    const double maxLevel = 1.0f;
    const double maxLevelDb = 31.0f;
    const double smoothingLengthInSeconds = 0.01;
    //IIRCoefficients coeff;
    //IIRFilter filter;
    double m_sampleRate;
};

// END SAWTOOTH WAVE
//==============================================================================

JuceDemoPluginAudioProcessor::JuceDemoPluginAudioProcessor()
	: AudioProcessor(getBusesProperties())
{
	lastPosInfo.resetToDefault();

	// This creates our parameters. We'll keep some raw pointers to them in this class,
	// so that we can easily access them later, but the base class will take care of
	// deleting them for us.
	addParameter(gainParam = new AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.9f));
    addParameter(delayParam = new AudioParameterFloat("delay", "Delay Feedback", 0.0f, 0.5f, 0.5f));    // 0.5f -> 1.0f

	initialiseSynth(8, 1); // Start with Polyphony and Sinewave
}

JuceDemoPluginAudioProcessor::~JuceDemoPluginAudioProcessor()
{
}

// Now a public method
void JuceDemoPluginAudioProcessor::initialiseSynth(int voices, int wave)
{
    this->voiceNum = voices;
    this->waveform=wave;
    // Reset the Synth
    synthe.clearVoices();
    
    // Apply the number of voices
	int numVoices = voices;                // 8 voice synth
    
    switch(wave)
    {
        case 1:
            // Sine
            for (int i = numVoices; --i >= 0;)
            {
                synthe.addVoice(new SineWave);
            }
            break;
        case 2:
            // Triangle
            for (int i = numVoices; --i >= 0;)
            {
                synthe.addVoice(new TriangleWave);
            }
            break;
        case 3:
            // Square
            for (int i = numVoices; --i >= 0;)
            {
                synthe.addVoice(new SquareWave);
            }
            break;
        case 4:
            // Sawtooth
            for (int i = numVoices; --i >= 0;)
            {
                synthe.addVoice(new SawtoothWave);
            }
            break;
        default:
            break;
    }

    // Add Pitch Bend
    synthe.setPitchbendTrackingMode (MPEInstrument::allNotesOnChannel);  // Pitch Bend
    // All notes
    synthe.enableLegacyMode(24);
    synthe.setVoiceStealingEnabled(false);
    visualiserInstrument.enableLegacyMode(24);
}

//==============================================================================
bool JuceDemoPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
	// Only mono/stereo and input/output must have same layout
	const AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();
	const AudioChannelSet& mainInput = layouts.getMainInputChannelSet();

	// input and output layout must either be the same or the input must be disabled altogether
	if (!mainInput.isDisabled() && mainInput != mainOutput)
		return false;

	// do not allow disabling the main buses
	if (mainOutput.isDisabled())
		return false;

	// only allow stereo and mono
	if (mainOutput.size() > 2)
		return false;

	return true;
}

AudioProcessor::BusesProperties JuceDemoPluginAudioProcessor::getBusesProperties()
{
	return BusesProperties().withInput("Input", AudioChannelSet::stereo(), true)
		.withOutput("Output", AudioChannelSet::stereo(), true);
}

//==============================================================================
void JuceDemoPluginAudioProcessor::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	synthe.setCurrentPlaybackSampleRate(newSampleRate);
	keyboardState.reset();
    
    // FILTERING
    L.setCoefficients(filter.makeLowPass(newSampleRate, 100, 1.0));
    R.setCoefficients(filter.makeLowPass(newSampleRate, 100, 1.0));
    // END FILTER

	if (isUsingDoublePrecision())
	{
		delayBufferDouble.setSize(2, 12000);
		delayBufferFloat.setSize(1, 1);
	}
	else
	{
		delayBufferFloat.setSize(2, 12000);
		delayBufferDouble.setSize(1, 1);
	}

	reset();
}

void JuceDemoPluginAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	keyboardState.reset();
}

void JuceDemoPluginAudioProcessor::reset()
{
	// Use this method as the place to clear any delay lines, buffers, etc, as it
	// means there's been a break in the audio's continuity.
	delayBufferFloat.clear();
	delayBufferDouble.clear();
}

template <typename FloatType>                                                       // This is where the action of the plug in is
void JuceDemoPluginAudioProcessor::process(AudioBuffer<FloatType>& buffer,          // This is where MIDI Messages/Samples are handled
	MidiBuffer& midiMessages,                                                       
	AudioBuffer<FloatType>& delayBuffer)
{
	const int numSamples = buffer.getNumSamples();

	// Now pass any incoming midi messages to our keyboard state object, and let it
	// add messages to the buffer if the user is clicking on the on-screen keys
	keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);         // Add keyboard/MIDI keyboard to buffer


	// and now get our synth to process these midi events and generate its output.
	synthe.renderNextBlock(buffer, midiMessages, 0, numSamples);                     // Add synth to buffer
	
    
	// Apply our delay effect to the new output..
	applyDelay(buffer, delayBuffer);                                                // Add delay
    
    
    // In case we have more outputs than inputs, we'll clear any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
    {
		buffer.clear(i, 0, numSamples);
    }

	applyGain(buffer, delayBuffer); // apply our gain-change to the outgoing data.. // Add delay

    // Now ask the host for the current time so we can store it to be displayed later...
	updateCurrentTimeInfoFromHost();
}

template <typename FloatType>                                                       // Gain Functionality
void JuceDemoPluginAudioProcessor::applyGain(AudioBuffer<FloatType>& buffer, AudioBuffer<FloatType>& delayBuffer)
{
    // Gain Application
	ignoreUnused(delayBuffer);
	const float gainLevel = *gainParam;
    
    // Apply Filter?

    for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
    {
		buffer.applyGain(channel, 0, buffer.getNumSamples(), gainLevel);
    }
}


template <typename FloatType>                                                       // Delay Functionality
void JuceDemoPluginAudioProcessor::applyDelay(AudioBuffer<FloatType>& buffer, AudioBuffer<FloatType>& delayBuffer)
{
	const int numSamples = buffer.getNumSamples();
	const float delayLevel = *delayParam;

	int delayPos = 0;

	for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
	{
		auto channelData = buffer.getWritePointer(channel);
		auto delayData = delayBuffer.getWritePointer(jmin(channel, delayBuffer.getNumChannels() - 1));
		delayPos = delayPosition;

		for (int i = 0; i < numSamples; ++i)                            // Delay algorithm
		{
			auto in = channelData[i];
			channelData[i] += delayData[delayPos];
			delayData[delayPos] = (delayData[delayPos] + in) * delayLevel;

			if (++delayPos >= delayBuffer.getNumSamples())
				delayPos = 0;
		}
	}

	delayPosition = delayPos;
}


void JuceDemoPluginAudioProcessor::updateCurrentTimeInfoFromHost()
{
	if (AudioPlayHead* ph = getPlayHead())
	{
		AudioPlayHead::CurrentPositionInfo newTime;

		if (ph->getCurrentPosition(newTime))
		{
			lastPosInfo = newTime;  // Successfully got the current time from the host..
			return;
		}
	}

	// If the host fails to provide the current time, we'll just reset our copy to a default..
	lastPosInfo.resetToDefault();
}


//==============================================================================
AudioProcessorEditor* JuceDemoPluginAudioProcessor::createEditor()
{
    return new JuceDemoPluginAudioProcessorEditor(*this);
}

//==============================================================================
// Plugin Instantiation Settings
void JuceDemoPluginAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// Here's an example of how you can use XML to make it easy and more robust:

	// Create an outer XML element..
	XmlElement xml("MYPLUGINSETTINGS");

	// add some attributes to it..
	xml.setAttribute("uiWidth", lastUIWidth);
	xml.setAttribute("uiHeight", lastUIHeight);

	// Store the values of all our parameters, using their param ID as the XML attribute
	for (auto* param : getParameters())
		if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
			xml.setAttribute(p->paramID, p->getValue());

	// then use this helper function to stuff it into the binary blob and return it..
	copyXmlToBinary(xml, destData);
}

void JuceDemoPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.

	// This getXmlFromBinary() helper function retrieves our XML from the binary blob..
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState != nullptr)
	{
		// make sure that it's actually our type of XML object..
		if (xmlState->hasTagName("MYPLUGINSETTINGS"))
		{
			// ok, now pull out our last window size..
			lastUIWidth = jmax(xmlState->getIntAttribute("uiWidth", lastUIWidth), 400);
			lastUIHeight = jmax(xmlState->getIntAttribute("uiHeight", lastUIHeight), 200);

			// Now reload our parameters..
			for (auto* param : getParameters())
				if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
					p->setValue((float)xmlState->getDoubleAttribute(p->paramID, p->getValue()));
		}
	}
}

         
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new JuceDemoPluginAudioProcessor();
}
