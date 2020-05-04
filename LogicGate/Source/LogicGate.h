/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2016 Open Ephys

    Modified by:

    Alessio Buccino     alessiob@ifi.uio.no
    Mikkel Lepperod
    Svenn-Arne Dragly

    Center for Integrated Neuroplasticity CINPLA
    Department of Biosciences
    University of Oslo
    Norway


    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __LOGICGATE_H_A8BF66D6__
#define __LOGICGATE_H_A8BF66D6__

#include <ProcessorHeaders.h>

using namespace std;

/**
 * @brief The EventSources struct contains information about incoming Event
 * channels to be displayed in the trigger and gate comboboxes
 */
struct EventSources
{
    unsigned int eventIndex;
    unsigned int sourceId;
    unsigned int channel;
};

/**
    Allows the user to set all Pulse Pal (Sanworks - www.sanworks.io) parameters and to trigger
    and gate Pulse Pal stimulation in response to TTL events.

    @see GenericProcessor, LogicGateEditor, LogicGateCanvas, PulsePal
*/
class LogicGate : public GenericProcessor
{
public:
    /** The class constructor, used to connect to PulsePal initialize any members. */
    LogicGate();
    /** The class destructor, used to deallocate memory */
    ~LogicGate();
    /**
     * @brief GenericProcessor member functions:
     * createEditor(): creates LogicGateEditor
     * process(): checks for events
     * handleEvent(): triggers or gates Pulse Pal stimulation when received TTL events
     *                correspond to trigger or gate selected channels
     * saveCustomParameterToXml(): saves all Pulse Pal setting to settings
     * loadCustomParameterFromXml(): loads all Pulse Pal setting from settings
     */
    AudioProcessorEditor* createEditor() override;
    void process (AudioSampleBuffer& buffer) override;
    void handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum) override;
    void saveCustomParametersToXml(XmlElement *parentElement);
    void loadCustomParametersFromXml();

    /**
     * @brief addEventSource adds a TTLevent source to the sources array
     * @param s: new source to add to the sources array
     */
    void addEventSource(EventSources s);
    /**
     * @brief clearEventSources clears sources array
     */
    void clearEventSources();

    void setInput1(int i1);
    void setInput2(int i2);
    void setGate1(bool set);
    void setGate2(bool set);
    void setLogicOp(int op);
    void setOutput(int out);
    void setWindow(int win);
    void setTtlDuration(int dur);

    int getInput1();
    int getInput2();
    bool getGate1();
    bool getGate2();
    int getLogicOp();
    int getOutput();
    int getWindow();
    int getTtlDuration();

protected:
    void createEventChannels() override;

private:
    int m_input1;
    int m_input2;
    bool m_input1gate;
    bool m_input2gate;
    int m_logicOp;
    int m_outputChan;
    int m_window;
    Array<EventSources> m_sources;
    EventChannel* eventChannelPtr;

    // Time
    float m_timePassed;
    int64 m_previousTime;
    int64 m_currentTime;
    int64 m_timeFromTrigger;
    int m_pulseDuration;

    // Conditions
    bool A;
    bool B;

    // Save TS
    int64 A_ts;
    int64 B_ts;


    void triggerEvent();
    TTLEventPtr turnOffEvent;
    int nSamples;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LogicGate);
};
#endif  // __LOGICGATE_H_A8BF66D6__
