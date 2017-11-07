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

// OTHER LOOK AND FEEL
class OtherLookAndFeel : public LookAndFeel_V4
{
public:
    // Constructor
    OtherLookAndFeel()
    {
        setColour(Slider::thumbColourId, Colours:: red);
    }
    
    // Methods
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        const float radius = jmin(width/2, height/2)-4.0f;
        const float centreX = x+width *0.5f;
        const float centreY = y+height*0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius *2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        g.setColour(Colours::lightslategrey);       // Knob Color
        g.fillEllipse(rx,ry,rw,rw);
        
        Path p;
        const float pointerLength = radius * 0.33f;
        const float pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(AffineTransform::rotation(angle).translated(centreX,centreY));
        
        g.setColour(Colours::white);          // Slit Color
        g.fillPath(p);
    }
};

//==============================================================================
/** This is the editor component that our filter will display.
*/
class JuceDemoPluginAudioProcessorEditor : public AudioProcessorEditor,
    private MidiKeyboardStateListener,
    private MidiInputCallback,
    private ComboBox::Listener,
    private Button::Listener
{
public:
    // MAIN COMPONENT CONSTRUCTOR/DESTRUCTOR
	JuceDemoPluginAudioProcessorEditor(JuceDemoPluginAudioProcessor& owner);
	~JuceDemoPluginAudioProcessorEditor();
    
	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;
    void buttonClicked (Button* button) override; // Poly Toggle Button
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
    // Advanced GUI Components (BTS)
    AudioDeviceManager deviceManager;                       // Audio Devices
<<<<<<< HEAD
    ComboBox midiInputList;									// MIDI Input List (Controllers
=======
>>>>>>> Sprint-Review
    int lastInputIndex;
    bool isAddingFromMidiInput;                             // Checks if from MIDI Controller
    MidiKeyboardState midiKeyboardState;                    // For MIDI Input
    MidiKeyboardComponent midiKeyboard;                     // For MIDI Controller
    
    // Front-End
	class ParameterSlider;                                  // Pots
<<<<<<< HEAD

	ComboBox waveformSelection;
	Label waveformLabel;

	ComboBox filterSelection;
	Label filterLabel;

=======
    ComboBox midiInputList;                                 // MIDI Input List (Controllers)
>>>>>>> Sprint-Review
    TextEditor midiMessagesBox;                             // For MIDI Display
    Label midiInputListLabel, gainLabel, delayLabel;        // Slider Labels
    ScopedPointer<ParameterSlider> gainSlider, delaySlider; // Sliders
    ToggleButton Poly;                                      // Toggle for Poly or Mono Synth
    
<<<<<<< HEAD
    MidiKeyboardState midiKeyboardState;                    // For MIDI Input
	MidiKeyboardComponent midiKeyboard;                     // For MIDI Controller
	Label midiInputListLabel, gainLabel, delayLabel;        // Slider Labels
    ScopedPointer<ParameterSlider> gainSlider, delaySlider; // Slideres

=======
    // Custom Pots
    OtherLookAndFeel otherLookAndFeel;
>>>>>>> Sprint-Review
    
	//==============================================================================
	JuceDemoPluginAudioProcessor& getProcessor() const
	{
		return static_cast<JuceDemoPluginAudioProcessor&> (processor);
	}
    
};

