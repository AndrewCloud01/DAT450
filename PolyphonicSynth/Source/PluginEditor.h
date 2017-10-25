/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <iostream>
using namespace std;
// New

//==============================================================================
/** This is the editor component that our filter will display.
*/
class JuceDemoPluginAudioProcessorEditor : public AudioProcessorEditor,
    private MidiKeyboardStateListener,
    private MidiInputCallback,
    private ComboBox::Listener
{
public:
    // MAIN COMPONENT CONSTRUCTOR/DESTRUCTOR
	JuceDemoPluginAudioProcessorEditor(JuceDemoPluginAudioProcessor&);
	~JuceDemoPluginAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;
    //==============================================================================
    // BEGIN MIDI DISPLAY METHODS

    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) override;
    String getMidiMessageDescription (const MidiMessage& m);
    void logMessage (const String& m);
    void addMessageToList (const MidiMessage& message, const String& source);
    void postMessageToList (const MidiMessage& message, const String& source);
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*floatvelocity*/) override;
    void comboBoxChanged (ComboBox* box) override;
    void setMidiInput (int index);
    
    // This is used to dispach an incoming message to the message thread
    class IncomingMessageCallback   : public CallbackMessage
    {
    public:
        IncomingMessageCallback (JuceDemoPluginAudioProcessorEditor* o, const MidiMessage& m, const String& s)
        : owner (o), message (m), source (s)
        {}
        
        void messageCallback() override
        {
            if (owner != nullptr)
                owner->addMessageToList (message, source);
        }
        
        Component::SafePointer<JuceDemoPluginAudioProcessorEditor> owner;
        MidiMessage message;
        String source;
    };
    
    // END MIDI DISPLAY METHODS
    //==============================================================================

    
private:
    // GUI Components
    AudioDeviceManager deviceManager;                       // Audio Devices
    ComboBox midiInputList;                                 // MIDI Input List (Controllers
    int lastInputIndex;
    bool isAddingFromMidiInput;                             // Checks if from MIDI Controller
    
	class ParameterSlider;                                  // Pots

    TextEditor midiMessagesBox;                             // For MIDI Display
    
    MidiKeyboardState midiKeyboardState;                    // For MIDI Input
	MidiKeyboardComponent midiKeyboard;                     // For MIDI Controller
	Label midiInputListLabel, gainLabel, delayLabel;        // Slider Labels
    ScopedPointer<ParameterSlider> gainSlider, delaySlider; // Slideres
    

	//==============================================================================
	JuceDemoPluginAudioProcessor& getProcessor() const
	{
		return static_cast<JuceDemoPluginAudioProcessor&> (processor);
	}
    
};

