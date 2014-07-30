/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, private HighResolutionTimer,
public ComboBox::Listener// to listen to menu
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
    
    
    
    void comboBoxChanged(ComboBox* box) override;
    
    MidiFile midiFile;

    void loadMidiFile(String fileLocation);
private:
    void startMidiPlayback();
    float playbackSpeed;
    //timer to play file - called every millisecond
    void hiResTimerCallback() override{
        updateMidiPlayPosition();
    }
    void updateMidiPlayPositionToMillis(float millisPosition);
    //menu for midi devices
    ComboBox midiOutputBox;
    ScopedPointer<MidiOutput> midiOutputDevice;

    int millisCounter;
    int midiPlayIndex;
    void updateMidiPlayPosition();
    MidiMessageSequence trackSequence;//pointer to track we will play
    MidiMessageSequence::MidiEventHolder* playEvent;//pointer to an individual midi event
    bool holdOn;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
