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
// (may move this class into the library itself at some point in the future..)
class JuceDemoPluginAudioProcessorEditor::ParameterSlider : public Slider,
	private Timer
{
public:
	ParameterSlider(AudioProcessorParameter& p)
		: Slider(p.getName(256)), param(p)
	{
		setRange(0.0, 1.0, 0.0);
		startTimerHz(30);
		updateSliderPos();
	}

	void valueChanged() override
	{
		if (isMouseButtonDown())
			param.setValueNotifyingHost((float)Slider::getValue());
		else
			param.setValue((float)Slider::getValue());
	}

	void timerCallback() override { updateSliderPos(); }

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
JuceDemoPluginAudioProcessorEditor::JuceDemoPluginAudioProcessorEditor(JuceDemoPluginAudioProcessor& owner)
	: AudioProcessorEditor(owner),
	midiKeyboard(owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard),       // Onscreen MIDI Keyboard
	timecodeDisplayLabel(String()),
	gainLabel(String(), "Gain:")//,                                                       // Throughput Level
	//delayLabel(String(), "Delay:")
{
	// add some sliders..
	addAndMakeVisible(gainSlider = new ParameterSlider(*owner.gainParam));              // Gain slider
	gainSlider->setSliderStyle(Slider::Rotary);                                         // Pot Slider

	//addAndMakeVisible(delaySlider = new ParameterSlider(*owner.delayParam));            // Delay slider
	//delaySlider->setSliderStyle(Slider::Rotary);                                        // Pot Slider
    
	// add some labels for the sliders..
	gainLabel.attachToComponent(gainSlider, false);
	gainLabel.setFont(Font(11.0f));

	//delayLabel.attachToComponent(delaySlider, false);
	//delayLabel.setFont(Font(11.0f));

	// add the midi keyboard component..
	addAndMakeVisible(midiKeyboard);
    owner.keyboardState.addListener(this);
    // Display a MIDI keyboard

	// add a label that will display the current timecode and status..
	//addAndMakeVisible(timecodeDisplayLabel);
	//timecodeDisplayLabel.setFont(Font(Font::getDefaultMonospacedFontName(), 15.0f, Font::plain));
    
    //MIDI Display
    /*
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
     */
    // End MIDI display
    
   
    
    
    // set resize limits for this plug-in
	setResizeLimits(400, 200, 800, 300);

	// set our component's initial size to be the last one that was stored in the filter's settings
	setSize(owner.lastUIWidth, owner.lastUIHeight);
    
    // midi display 2
    audioDeviceManager.setMidiInputEnabled("USB Axiom 49 Port 1", true);
    audioDeviceManager.addMidiInputCallback(String::empty, this);
    midiLabel.setText("midiText", sendNotification);
    addAndMakeVisible(midiLabel);
    // end midi display 2

	// start a timer which will keep our timecode display updated
	startTimerHz(30);
}

void JuceDemoPluginAudioProcessorEditor::handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message)
{
    keyboardState.processNextMidiEvent (message);
    postMessageToList (message, source->getName());
}

/*
// MIDI Messages
void JuceDemoPluginAudioProcessorEditor::handleIncomingMidiMessage (MidiInput*, const MidiMessage& message)
{
    DBG("MIDI Message Received\n");
    
    String midiText;
    
    if(message.isNoteOnOrOff())
    {
        midiText << "NoteOn: Channel " << message.getChannel();
        midiText << ":Number" << message.getNoteNumber();
        midiText << ":Velocity" << message.getVelocity();
    }
    midiLabel.setText(midiText, dontSendNotification);
    //midiLabel.getTextValue() = midiText;
    //midiLabel.getText() = midiText;
    //cout << midiText;
}
 */

JuceDemoPluginAudioProcessorEditor::~JuceDemoPluginAudioProcessorEditor()
{
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::paint(Graphics& g)
{
	g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	g.fillAll();
}

void JuceDemoPluginAudioProcessorEditor::resized()
{
	// This lays out our child components...

	Rectangle<int> r(getLocalBounds().reduced(8));

	//timecodeDisplayLabel.setBounds(r.removeFromTop(26));
	midiKeyboard.setBounds(r.removeFromBottom(70));
    
    //midiMessagesBox.setBounds (r.removeFromBottom(45));           // TextEditor for MIDI Display
    
    // MIDI Display2
    midiLabel.setBounds(r.removeFromBottom(45));

	r.removeFromTop(20);
	Rectangle<int> sliderArea(r.removeFromTop(45));
	gainSlider->setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth() / 2)));
    
	//delaySlider->setBounds(sliderArea.removeFromLeft(jmin(180, sliderArea.getWidth())));
    
    

	getProcessor().lastUIWidth = getWidth();
	getProcessor().lastUIHeight = getHeight();
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::timerCallback()
{
	updateTimecodeDisplay(getProcessor().lastPosInfo);
}

//==============================================================================

// quick-and-dirty function to format a timecode string
static String timeToTimecodeString(double seconds)
{
	const int millisecs = roundToInt(seconds * 1000.0);
	const int absMillisecs = std::abs(millisecs);

	return String::formatted("%02d:%02d:%02d.%03d",
		millisecs / 3600000,
		(absMillisecs / 60000) % 60,
		(absMillisecs / 1000) % 60,
		absMillisecs % 1000);
}

// quick-and-dirty function to format a bars/beats string
static String quarterNotePositionToBarsBeatsString(double quarterNotes, int numerator, int denominator)
{
	if (numerator == 0 || denominator == 0)
		return "1|1|000";

	const int quarterNotesPerBar = (numerator * 4 / denominator);
	const double beats = (fmod(quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * numerator;

	const int bar = ((int)quarterNotes) / quarterNotesPerBar + 1;
	const int beat = ((int)beats) + 1;
	const int ticks = ((int)(fmod(beats, 1.0) * 960.0 + 0.5));

	return String::formatted("%d|%d|%03d", bar, beat, ticks);
}

// Updates the text in our position label.
void JuceDemoPluginAudioProcessorEditor::updateTimecodeDisplay(AudioPlayHead::CurrentPositionInfo pos)
{
	MemoryOutputStream displayText;

	displayText << "[" << SystemStats::getJUCEVersion() << "]   "
		<< String(pos.bpm, 2) << " bpm, "
		<< pos.timeSigNumerator << '/' << pos.timeSigDenominator
		<< "  -  " << timeToTimecodeString(pos.timeInSeconds)
		<< "  -  " << quarterNotePositionToBarsBeatsString(pos.ppqPosition,
			pos.timeSigNumerator,
			pos.timeSigDenominator);

	if (pos.isRecording)
		displayText << "  (recording)";
	else if (pos.isPlaying)
		displayText << "  (playing)";

	timecodeDisplayLabel.setText(displayText.toString(), dontSendNotification);
}

