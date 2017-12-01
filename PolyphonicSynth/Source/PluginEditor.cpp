/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// This is a handy slider subclass that controls an AudioProcessorParameter
class JuceDemoPluginAudioProcessorEditor::ParameterSlider : public Slider
{
public:
	ParameterSlider(AudioProcessorParameter& p)
		: Slider(p.getName(256)), param(p)
	{
		setRange(0.0, 1.0, 0.0);
		updateSliderPos();
	}

	void valueChanged() override
	{
		if (isMouseButtonDown())
			param.setValueNotifyingHost((float)Slider::getValue());
		else
			param.setValue((float)Slider::getValue());
	}

	void startedDragging() override { param.beginChangeGesture(); }
	void stoppedDragging() override { param.endChangeGesture(); }

	double getValueFromText(const String& text) override { return param.getValueForText(text); }
	String getTextFromValue(double value) override { return param.getText((float)value, 1024); }

	void updateSliderPos()
	{
		const float newValue = param.getValue();

		if (newValue != (float)Slider::getValue() && !isMouseButtonDown())
			Slider::setValue(newValue);
	}

	AudioProcessorParameter& param;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSlider)
};

//==============================================================================
// BEGIN PLUGIN MAIN COMPONENT CONSTRUCTOR/DESTRUCTOR

JuceDemoPluginAudioProcessorEditor::JuceDemoPluginAudioProcessorEditor(JuceDemoPluginAudioProcessor& owner)
	: AudioProcessorEditor(owner),
	//midiKeyboard(owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard),        // Onscreen MIDI Keyboard
	midiKeyboard(midiKeyboardState, MidiKeyboardComponent::horizontalKeyboard),            // MIDI Controller
	gainLabel(String(), "Gain:"),                                                          // Throughput Level
	delayLabel(String(), "Delay:"),                                                        // Delay
	qLabel(String(), "Q:"),
	freqLabel(String(), "Frequency;")
{
	//==============================================================================
    // MIDI Device Input List
    addAndMakeVisible (midiInputListLabel);
    midiInputListLabel.setText ("MIDI Input:", dontSendNotification);
    midiInputListLabel.attachToComponent (&midiInputList, true);
    
    addAndMakeVisible (midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    const StringArray midiInputs (MidiInput::getDevices());
    midiInputList.addItemList (midiInputs, 1);
    midiInputList.addListener (this);

	// find the first enabled device and use that by default
	for (int i = 0; i < midiInputs.size(); ++i)
	{
		if (deviceManager.isMidiInputEnabled(midiInputs[i]))
		{
			setMidiInput(i);
			break;
		}
	}

	// if no enabled devices were found just use the first one in the list
	if (midiInputList.getSelectedId() == 0)
	{
		setMidiInput(0);
	}

	//==============================================================================
	// COMBO BOXES
    
    // Waveform Selection
	addAndMakeVisible(waveformSelection);
    waveformSelection.addListener(this);
	waveformLabel.setText("Choose Waveform", dontSendNotification);
	waveformLabel.attachToComponent(&waveformSelection, true);
	waveformSelection.addItem("Sine", 1);
	waveformSelection.addItem("Triangle", 2);
	waveformSelection.addItem("Square", 3);
    waveformSelection.addItem("Sawtooth", 4);
	waveformSelection.setSelectedId(1);
    // Filter Selection
	addAndMakeVisible(filterSelection);
    filterSelection.addListener(this);
	filterLabel.setText("Choose Filter", dontSendNotification);
	filterLabel.attachToComponent(&filterSelection, true);
	filterSelection.addItem("None", 1);
	filterSelection.addItem("Low Pass", 2);
	filterSelection.addItem("High Pass", 3);
	filterSelection.addItem("Band Pass", 4);
	filterSelection.setSelectedId(1);

	//==============================================================================
    // GAIN SLIDER AND LABEL
	addAndMakeVisible(gainSlider = new ParameterSlider(*owner.gainParam));              // Gain slider
	gainSlider->setSliderStyle(Slider::Rotary);                                         // Pot Slider
    gainLabel.attachToComponent(gainSlider, false);
    gainLabel.setFont(Font(11.0f));
    
    // DELAY SLIDER AND LABEL
	addAndMakeVisible(delaySlider = new ParameterSlider(*owner.delayParam));          // Delay slider
	delaySlider->setSliderStyle(Slider::Rotary);                                      // Pot Slider
    delayLabel.attachToComponent(delaySlider, false);
    delayLabel.setFont(Font(11.0f));

	// Q SLIDER AND LABEL
	addAndMakeVisible(qSlider = new ParameterSlider(*owner.delayParam));          // Q slider
	qSlider->setSliderStyle(Slider::Rotary);                                      // Pot Slider
	qLabel.attachToComponent(qSlider, false);
	qLabel.setFont(Font(11.0f));

	// FREQUENCY SLIDER AND LABEL
	addAndMakeVisible(freqSlider = new ParameterSlider(*owner.delayParam));          // Q slider
	freqSlider->setSliderStyle(Slider::Rotary);                                      // Pot Slider
	freqLabel.attachToComponent(freqSlider, false);
	freqLabel.setFont(Font(11.0f));

	//==============================================================================
    // MIDI CONTROLLER INPUT
    midiKeyboardState.addListener(this);
	
	//==============================================================================
    //MIDI Display
    addAndMakeVisible (midiMessagesBox);
    midiMessagesBox.setMultiLine (true);
    midiMessagesBox.setReturnKeyStartsNewLine (true);
    midiMessagesBox.setReadOnly (true);
    midiMessagesBox.setScrollbarsShown (true);
    midiMessagesBox.setCaretVisible (false);
    midiMessagesBox.setPopupMenuEnabled (true);
    midiMessagesBox.setColour (TextEditor::backgroundColourId, Colour (0x32ffffff));
    midiMessagesBox.setColour (TextEditor::outlineColourId, Colour (0x1c000000));
    midiMessagesBox.setColour (TextEditor::shadowColourId, Colour (0x16000000));
    // End MIDI display
    
	//==============================================================================
    // POLYPHONY
    addAndMakeVisible(Poly);
    Poly.setButtonText ("Polyphony Enabled");
    Poly.setToggleState (true, dontSendNotification);
    Poly.addListener(this);
    
	//==============================================================================
    // set resize limits for this plug-in
	//setResizeLimits(400, 200, 800, 300);  // Old Resize
    setResizeLimits(800, 400, 800, 500);

	// set our component's initial size to be the last one that was stored in the filter's settings
	setSize(owner.lastUIWidth, owner.lastUIHeight);
    setLookAndFeel(&otherLookAndFeel);
}

JuceDemoPluginAudioProcessorEditor::~JuceDemoPluginAudioProcessorEditor()
{
    midiKeyboardState.removeListener (this);
    deviceManager.removeMidiInputCallback (MidiInput::getDevices()[midiInputList.getSelectedItemIndex()], this);
    midiInputList.removeListener (this);
    filterSelection.removeListener(this);
    waveformSelection.removeListener(this);
    Poly.removeListener(this);
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::paint(Graphics& g)
{
	g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	g.fillAll();
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::resized()
{
	//==============================================================================
	// This lays out our child components...
	Rectangle<int> r(getLocalBounds().reduced(8));
    
	//==============================================================================
    // MIDI Display
    midiInputList.setBounds (r.removeFromTop (35).removeFromRight (getWidth() - 75).reduced(4));  //Flush Left
	r.removeFromTop(5);    // Space
    
	//==============================================================================
	// POLY TOGGLE
	Rectangle<int> polyToggle(r.removeFromTop(10).expanded(6));
	Poly.setBounds(polyToggle.removeFromLeft(jmin(200, polyToggle.getWidth())));
	r.removeFromTop(40);    // Space
	
	//==============================================================================
	// POTS
	Rectangle<int> sliderArea(r.removeFromTop(40).expanded(10));
	sliderArea.removeFromLeft(10); // space from left
    
	// Gain Pot Location
	gainSlider->setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 4));
    
    // Delay Pot Location
	delaySlider->setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() /3));

	// Q Pot Location
	qSlider->setBounds(sliderArea.removeFromLeft(sliderArea.getWidth()/2));

	// Frequency Pot Location
	freqSlider->setBounds(sliderArea.removeFromLeft(sliderArea.getWidth()));
    
    r.removeFromTop(20);    // Space
	
	//==============================================================================
	// COMBO BOX AREAS
	Rectangle<int> filterArea(r.removeFromTop(30));

	// Waveform ComboBox Area
	waveformSelection.setBounds(filterArea.removeFromLeft(100));
	waveformSelection.setBounds(filterArea.removeFromLeft(filterArea.getWidth()/2 - 50));
	
	// Filter ComboBox Area
	filterSelection.setBounds(filterArea.removeFromLeft(100));
	filterSelection.setBounds(filterArea.removeFromLeft(filterArea.getWidth()));
    r.removeFromTop(10);    // Space
    
	//==============================================================================
    // MIDI DISPLAY TEXTBOXES
    midiMessagesBox.setBounds (r.removeFromBottom(20));           // TextEditor for MIDI Display
    midiMessagesBox.setBounds (r.reduced (2));
    
	getProcessor().lastUIWidth = getWidth();
	getProcessor().lastUIHeight = getHeight();
}
//==============================================================================
// BEGIN MIDI DISPLAY METHODS

 // These methods handle callbacks from the midi device
void JuceDemoPluginAudioProcessorEditor::handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message)
{
    const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
    midiKeyboardState.processNextMidiEvent (message);
    postMessageToList (message, source->getName());
}

 // Begin Audio Post Message
String JuceDemoPluginAudioProcessorEditor::getMidiMessageDescription (const MidiMessage& m)
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

// Log MIDI Message for Display
void JuceDemoPluginAudioProcessorEditor::logMessage (const String& m)
{
    midiMessagesBox.moveCaretToEnd();
    midiMessagesBox.insertTextAtCaret (m + newLine);
}

// Compile MIDI Message Info and Log
void JuceDemoPluginAudioProcessorEditor::addMessageToList (const MidiMessage& message, const String& source)
{
    const String description (getMidiMessageDescription (message));
    const String midiMessageString (description + " (" + source + ")"); // [7]
    logMessage (midiMessageString);
}

// Prepare MIDI Message for Log
void JuceDemoPluginAudioProcessorEditor::postMessageToList (const MidiMessage& message, const String& source)
{
    (new IncomingMessageCallback (this, message, source))->post();
}

// Log NoteOn Messages
void JuceDemoPluginAudioProcessorEditor::handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (! isAddingFromMidiInput)
    {
        MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
        postMessageToList (m, "On-Screen Keyboard");
    }
}

// Log NoteOff Messages
void JuceDemoPluginAudioProcessorEditor::handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*floatvelocity*/)
{
    if (! isAddingFromMidiInput)
    {
        MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber));
        postMessageToList (m, "On-Screen Keyboard");
    }
}

// Poly Toggle Button
void JuceDemoPluginAudioProcessorEditor::buttonClicked (Button* button) // Poly Toggle Button
{
    // Get button state
    int buttonState = button->getToggleState();
    
    // Check Button State and change synth accordingly
    if (buttonState == 0)
    {
        // The button is untoggled
        this->getProcessor().initialiseSynth(1, this->getProcessor().getWaveform());
    }
    else
    {
        // Is toggled
        this->getProcessor().initialiseSynth(8, this->getProcessor().getWaveform());
    }
}
// ComboBox Change Action
void JuceDemoPluginAudioProcessorEditor::comboBoxChanged (ComboBox* box)
{
    if (box == &midiInputList)
    {
        setMidiInput (midiInputList.getSelectedItemIndex());
    }    
    else if (box == &waveformSelection)
    {
        cout << waveformSelection.getSelectedId();
        if (waveformSelection.getSelectedId() == 1)
        {
            // Sine Wave
            this->getProcessor().initialiseSynth(this->getProcessor().getVoiceNum(), 1);
        }
        
        if (waveformSelection.getSelectedId() == 2)
        {
            // Triangle Wave
            this->getProcessor().initialiseSynth(this->getProcessor().getVoiceNum(), 2);
        }
        
        if (waveformSelection.getSelectedId() == 3)
        {
            // Square Wave
            this->getProcessor().initialiseSynth(this->getProcessor().getVoiceNum(), 3);
        }
        
        if (waveformSelection.getSelectedId() == 4)
        {
            // Sawtooth
            this->getProcessor().initialiseSynth(this->getProcessor().getVoiceNum(), 4);
        }
        
    }
    else if (box == &filterSelection)
    {
        if (filterSelection.getSelectedId() == 1)
        {
            // None
        }
        
        if (filterSelection.getSelectedId() == 2)
        {
            // LP
        }
        
        if (filterSelection.getSelectedId() == 3)
        {
            // HP
        }
        
        if (filterSelection.getSelectedId() == 4)
        {
            // BP
        }
    }
}

// Starts listening to a MIDI input device, enabling it if necessary.
void JuceDemoPluginAudioProcessorEditor::setMidiInput (int index)
{
    const StringArray list (MidiInput::getDevices());
    
    deviceManager.removeMidiInputCallback (list[lastInputIndex], this);
    
    const String newInput (list[index]);
    
    if (! deviceManager.isMidiInputEnabled (newInput))
        deviceManager.setMidiInputEnabled (newInput, true);
    
    deviceManager.addMidiInputCallback (newInput, this);
    midiInputList.setSelectedId (index + 1, dontSendNotification);
    
    lastInputIndex = index;
}

// END MIDI DISPLAY METHODS
//==============================================================================
