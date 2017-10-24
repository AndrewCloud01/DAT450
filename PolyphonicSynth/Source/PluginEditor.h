/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
//#include <iostream>
//using namespace std;


//==============================================================================
/** This is the editor component that our filter will display.
*/
class JuceDemoPluginAudioProcessorEditor : public AudioProcessorEditor,
	private Timer,
    public MidiInputCallback
{
public:
	JuceDemoPluginAudioProcessorEditor(JuceDemoPluginAudioProcessor&);
	~JuceDemoPluginAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;
	void timerCallback() override;
    
    void handleIncomingMidiMessage (MidiInput*, const MidiMessage& message);

    
    
    // Begin Audio Post Message
    //static String getMidiMessageDescription (const MidiMessage& m)
    String getMidiMessageDescription (const MidiMessage& m)
    {
        if (m.isNoteOn())           return "Note on "  + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
        if (m.isNoteOff())          return "Note off " + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
        if (m.isProgramChange())    return "Program change " + String (m.getProgramChangeNumber());
        if (m.isPitchWheel())       return "Pitch wheel " + String (m.getPitchWheelValue());
        if (m.isAftertouch())       return "After touch " + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3) +  ": " + String (m.getAfterTouchValue());
        if (m.isChannelPressure())  return "Channel pressure " + String (m.getChannelPressureValue());
        if (m.isAllNotesOff())      return "All notes off";
        if (m.isAllSoundOff())      return "All sound off";
        if (m.isMetaEvent())        return "Meta event";
        
        if (m.isController())
        {
            String name (MidiMessage::getControllerName (m.getControllerNumber()));
            
            if (name.isEmpty())
                name = "[" + String (m.getControllerNumber()) + "]";
            
            return "Controller " + name + ": " + String (m.getControllerValue());
        }
        
        return String::toHexString (m.getRawData(), m.getRawDataSize());
    }
     
    
    
    void logMessage (const String& m)
    {
        midiMessagesBox.moveCaretToEnd();
        midiMessagesBox.insertTextAtCaret (m + newLine);
    }
    
    void addMessageToList (const MidiMessage& message, const String& source){
        const String description (getMidiMessageDescription (message));
        
        const String midiMessageString (description + " (" + source + ")"); // [7]
        logMessage (midiMessageString);
    }
     
    void addMessageToList (const MidiMessage& message, const String& source)
    {
        const String description (getMidiMessageDescription (message));
        const String midiMessageString (description + " (" + source + ")"); // [7]
        logMessage (midiMessageString);
    }
    
    void postMessageToList (const MidiMessage& message, const String& source)
    {
        (new IncomingMessageCallback (this, message, source))->post();
    }
    
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
    {
        
            MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
           // m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
            postMessageToList (m, "On-Screen Keybaord");
        
    }
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, /*float velocity*/)
    {
        
            MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber));
            //m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
            postMessageToList (m, "On-Screen Keyboard");
        
    }


private:
	class ParameterSlider;
    
    TextEditor midiMessagesBox;
    
    Label midiLabel;
    
    //MidiMessage message;
    AudioDeviceManager audioDeviceManager;

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
