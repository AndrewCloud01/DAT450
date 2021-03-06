/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
As the name suggest, this class does the actual audio processing.
*/
class JuceDemoPluginAudioProcessor : public AudioProcessor
{
public:
	//==============================================================================
	JuceDemoPluginAudioProcessor();
	~JuceDemoPluginAudioProcessor();

	//==============================================================================
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;
	void reset() override;

	//==============================================================================
	void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
	{
		jassert(!isUsingDoublePrecision());
		process(buffer, midiMessages, delayBufferFloat);
	}

    /*
	void processBlock(AudioBuffer<double>& buffer, MidiBuffer& midiMessages) override
	{
		jassert(isUsingDoublePrecision());
		//process(buffer, midiMessages, delayBufferDouble);
	}
     */

	//==============================================================================
	bool hasEditor() const override { return true; }
	AudioProcessorEditor* createEditor() override;

	//==============================================================================
	const String getName() const override { return JucePlugin_Name; }

	bool acceptsMidi() const override { return true; }
	bool producesMidi() const override { return true; }

	double getTailLengthSeconds() const override { return 0.0; }

	//==============================================================================
	int getNumPrograms() override { return 0; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int /*index*/) override {}
	const String getProgramName(int /*index*/) override { return String(); }
	void changeProgramName(int /*index*/, const String& /*name*/) override {}

	//==============================================================================
	void getStateInformation(MemoryBlock&) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//==============================================================================
	// These properties are public so that our editor component can access them
	// A bit of a hacky way to do it, but it's only a demo! Obviously in your own
	// code you'll do this much more neatly..

	// this is kept up to date with the midi messages that arrive, and the UI component
	// registers with it so it can represent the incoming messages
	MidiKeyboardState keyboardState;

	// this keeps a copy of the last set of time info that was acquired during an audio
	// callback - the UI component will read this and display it.
	AudioPlayHead::CurrentPositionInfo lastPosInfo;

	// these are used to persist the UI's size - the values are stored along with the
	// filter's other parameters, and the UI component will update them when it gets
	// resized.
	int lastUIWidth = 700, lastUIHeight = 700;

	// Our parameters
	AudioParameterFloat* gainParam = nullptr;
	AudioParameterFloat* delayParam = nullptr;
    
    // For Filters
    AudioParameterFloat* freqParam = nullptr;
    AudioParameterFloat* qParam = nullptr;

    void initialiseSynth(int voices, int wave);
    double getSampleRate();
    int getFilterType();
    void setFilterType(int type);
    
    int getWaveform()
    {
        return waveform;
    }
    int getVoiceNum()
    {
        return voiceNum;
    }
    
    
private:
	//==============================================================================
	template <typename FloatType>
	void process(AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages, AudioBuffer<FloatType>& delayBuffer);
	template <typename FloatType>
	void applyGain(AudioBuffer<FloatType>&, AudioBuffer<FloatType>& delayBuffer);
	template <typename FloatType>
	void applyDelay(AudioBuffer<FloatType>&, AudioBuffer<FloatType>& delayBuffer);

	AudioBuffer<float> delayBufferFloat;
	//AudioBuffer<double> delayBufferDouble;

	int delayPosition = 0;
    
    int voiceNum;
    int waveform;
    
    int filterType;
    
    double sampleRate;
    

	//Synthesiser synth;
    MPESynthesiser synthe;
    MPEInstrument visualiserInstrument;
    
    // Filters
    IIRFilter L, R;
    IIRCoefficients lowFilter, highFilter, bandFilter;
    
	void updateCurrentTimeInfoFromHost();
	static BusesProperties getBusesProperties();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceDemoPluginAudioProcessor)
};
