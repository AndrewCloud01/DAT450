/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/** This is the editor component that our filter will display.
*/
class JuceDemoPluginAudioProcessorEditor : public AudioProcessorEditor,
	private Timer
{
public:
	JuceDemoPluginAudioProcessorEditor(JuceDemoPluginAudioProcessor&);
	~JuceDemoPluginAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;
	void timerCallback() override;

private:
	class ParameterSlider;

	MidiKeyboardComponent midiKeyboard;
	Label timecodeDisplayLabel, gainLabel, delayLabel;
	ScopedPointer<ParameterSlider> gainSlider, delaySlider;

	//==============================================================================
	JuceDemoPluginAudioProcessor& getProcessor() const
	{
		return static_cast<JuceDemoPluginAudioProcessor&> (processor);
	}

	void updateTimecodeDisplay(AudioPlayHead::CurrentPositionInfo);
};
