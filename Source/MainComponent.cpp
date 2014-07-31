/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent(): playbackSpeed(0.75)
{
    setSize (500, 400);
    
    
    addAndMakeVisible (midiOutputBox);
	midiOutputBox.setTextWhenNoChoicesAvailable ("No MIDI Output Enabled");
	
	const StringArray midiOutputs (MidiOutput::getDevices());
	midiOutputBox.addItemList (midiOutputs, 1);
	midiOutputBox.addListener (this);
	
	midiOutputDevice = NULL;
    
    
    String location = "../../../../exampleMidiFiles/midiScale.mid";
    loadMidiFile(location);
    
    
    
}

MainContentComponent::~MainContentComponent()
{
    stopTimer();
    
    midiOutputBox.removeListener(this);
    //midiOutputDevice = NULL;
    //delete midiOutputDevice;
    //using scoped pointer instead
}



void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xffeeddff));

    g.setFont (Font (16.0f));
    g.setColour (Colours::black);
    g.drawText ("choose a midi port and file will play", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    midiOutputBox.setBoundsRelative(0.1,0.05,0.7,0.05);
}

//midi device

void MainContentComponent::comboBoxChanged(ComboBox* box)//override
{
	if (box == &midiOutputBox){
		midiOutputDevice = MidiOutput::openDevice(midiOutputBox.getSelectedItemIndex());
		//midiOutputDevice->sendMessageNow(MidiMessage(144, 60, 100));
        startMidiPlayback();
	}
}


void MainContentComponent::startMidiPlayback(){
    millisCounter = 0;
    midiPlayIndex = -1;
    startTimer(1);
}

void MainContentComponent::updateMidiPlayPosition(){
    
   
    millisCounter++;
    
    updateMidiPlayPositionToMillis(millisCounter*playbackSpeed);
    
}

void MainContentComponent::updateMidiPlayPositionToMillis(float millisPosition){
    /*
     //this not needed - we already have index from the last time
     //could be good check though?
     int tmpCounter = (int)(millisCounter * playbackSpeed);
     while (midiPlayIndex < numEvents && trackSequence.getEventTime(midiPlayIndex) < tmpCounter){
     //we have caught up to where we are, maybe we should be optimising this though from last time it was called?
     midiPlayIndex++;
     }
     */
    
    int numEvents = trackSequence.getNumEvents();
    int useCount = (int)millisPosition;//(int)(millisCounter * playbackSpeed);//relative to 1ms = 1 tick
    
    MidiMessageSequence::MidiEventHolder* outputEvent;
    
    while (midiPlayIndex < numEvents && trackSequence.getEventTime(midiPlayIndex) < useCount){
        //we have caught up to where we are
        // index++;
        //std::cout << "play index " << midiPlayIndex << " at time " << trackSequence.getEventTime(midiPlayIndex) << std::endl;
        
        if (midiPlayIndex >= 0){

            outputEvent = trackSequence.getEventPointer(midiPlayIndex);
            
            if (outputEvent->message.isNoteOnOrOff()){
                midiOutputDevice->sendMessageNow(outputEvent->message);
                if (outputEvent->message.isNoteOn()){
                    int tmp = trackSequence.getIndexOfMatchingKeyUp(midiPlayIndex);
                    std::cout << "NOTE ON,  index " << midiPlayIndex << " has up key " << tmp << std::endl;
                } else {
                    std::cout << "NOTE Off, index " << midiPlayIndex << std::endl;
                }
            }
            else
                std::cout << "event " << midiPlayIndex << " is not note on/off" << std::endl;
            
        }
        
        midiPlayIndex++;
        
        int64 timenow = juce::Time::currentTimeMillis();
        std::cout << "timer callback " << timenow << std::endl;
        
    }
    
    outputEvent = NULL;
    delete outputEvent;

}

void MainContentComponent::loadMidiFile(String fileLocation){
    
    //either add exmaplemidiFiles to your project or change this location
    File file(fileLocation);
    
    //TO DO: add some kind of check whether file exists!
    
    if (!file.isDirectory())
    {
        FileInputStream fileInput(file);
        

        midiFile.readFrom(fileInput);
        //num tracks returns an int
        std::cout << "num tracks " << midiFile.getNumTracks() << std::endl;
        
        int ppq = midiFile.getTimeFormat();
        std::cout << "time format " << ppq << std::endl;
        
        
        
        
        std::cout << "Last Timestamp " << midiFile.getLastTimestamp() << std::endl;
        
        int trackIndex = 0;
        
        if (trackIndex < midiFile.getNumTracks()){
            
            //midi message sequence is pointer to a midi track, as loaded from a file
            trackSequence = *midiFile.getTrack(trackIndex);//replaces empty holder with track 0
            
            std::cout << "track 0 has " << trackSequence.getNumEvents() << " events" << std::endl;
            
            MidiMessageSequence::MidiEventHolder* event;//pointer to an individual midi event
            
            for (int i = 0; i < trackSequence.getNumEvents(); i++){
                //can get this time info from the track sequence
                double eventTime = trackSequence.getEventTime(i);
                std::cout << "\nevent " << i << " is at time " << eventTime << std::endl;
                
                //or parse through the events themselves
                event = trackSequence.getEventPointer(i);
                
                double tmp = event->message.getTimeStamp();
                std::cout << "event " << i << " timestamp " << tmp << std::endl;
                
                const uint8* data = event->message.getRawData();
                
                std::cout << "size is " <<  event->message.getRawDataSize() << std::endl;
                
                if (event->message.isNoteOn())
                    std::cout << "note on" << std::endl;
                else if (event->message.isNoteOff())
                    std::cout << "note off" << std::endl;
                else
                    std::cout << "other " << std::endl;
                
                std::cout << "data [0] " << (int)data[0] << std::endl;
                std::cout << "data [1] " << (int)data[1] << std::endl;
                std::cout << "data [2] " << (int)data[2] << std::endl;
                
                if (data[0] == 255){
                    for (int i = 3; i < event->message.getRawDataSize(); i++){
                        std::cout << "character[" << i-3 << "] :'" << data[i] << "'" << std::endl;
                    }
                }
                
            }//end for
            
            trackSequence.updateMatchedPairs();
            
        }//end if
        
    }//end main if not dir
    
}