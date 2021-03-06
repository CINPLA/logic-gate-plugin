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


#include <stdio.h>

#include "LogicGate.h"
#include "LogicGateEditor.h"


LogicGate::LogicGate()
    : GenericProcessor ("Logic Gate"),
      m_logicOp(0),
      m_outputChan(0),
      m_window(DEF_WINDOW),
      m_input1(-1),
      m_input2(-1),
      m_input1gate(false),
      m_input2gate(false),
      m_pulseDuration(2)
{
    setProcessorType (PROCESSOR_TYPE_FILTER);
}

LogicGate::~LogicGate()
{
}

AudioProcessorEditor* LogicGate::createEditor()
{
    editor = new LogicGateEditor (this, true);
    return editor;
}

void LogicGate::createEventChannels()
{
    EventChannel* ev = new EventChannel(EventChannel::TTL, 8, 1, CoreServices::getGlobalSampleRate(), this);
    ev->setName("Logic Gate TTL output" );
    ev->setDescription("Triggers when logic operator is satisfied.");
    ev->setIdentifier ("dataderived.logicgate.trigger");
    eventChannelArray.add (ev);
}

void LogicGate::handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum)
{
    if (Event::getEventType(event) == EventChannel::TTL)
    {
        TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, eventInfo);
        const int state         = ttl->getState() ? 1 : 0;
        const int eventId       = ttl->getSourceIndex();
        const int sourceId      = ttl->getSourceID();
        const int eventChannel  = ttl->getChannel();

        if (m_input1 != -1)
        {
            EventSources s = m_sources.getReference (m_input1);
            if (eventId == s.eventIndex && sourceId == s.sourceId
                    && eventChannel == s.channel && state)
            {
                std::cout << "Received A " << std::endl;
                A = true;

                if ((m_input1gate) || (!m_input1gate && !m_input2gate))
                    m_previousTime = Time::currentTimeMillis();
            }
        }

        if (m_input2 != -1)
        {
            EventSources s = m_sources.getReference (m_input2);
            if (eventId == s.eventIndex && sourceId == s.sourceId
                    && eventChannel == s.channel && state)
            {
                std::cout << "Received B " << std::endl;
                B = true;
                if ((m_input2gate) || (!m_input1gate && !m_input2gate))
                    m_previousTime = Time::currentTimeMillis();
            }
        }
    }
}


void LogicGate::setInput1(int i1)
{
    m_input1 = i1;
}
void LogicGate::setInput2(int i2)
{
    m_input2 = i2;
}
void LogicGate::setGate1(bool set)
{
    m_input1gate = set;
}
void LogicGate::setGate2(bool set)
{
    m_input2gate = set;
}
void LogicGate::setLogicOp(int op)
{
    m_logicOp = op;
}
void LogicGate::setOutput(int out)
{
    m_outputChan = out;
}
void LogicGate::setWindow(int win)
{
    m_window = win;
}
void LogicGate::setTtlDuration(int dur)
{
    m_pulseDuration = dur;
}

int LogicGate::getInput1()
{
    return m_input1;
}
int LogicGate::getInput2()
{
    return m_input2;
}
bool LogicGate::getGate1()
{
    return m_input1gate;
}
bool LogicGate::getGate2()
{
    return m_input2gate;
}
int LogicGate::getLogicOp()
{
    return m_logicOp;
}
int LogicGate::getOutput()
{
    return m_outputChan;
}
int LogicGate::getWindow()
{
    return m_window;
}
int LogicGate::getTtlDuration()
{
    return m_pulseDuration;
}

void LogicGate::process (AudioSampleBuffer& buffer)
{
    checkForEvents ();
    // implement logic
    m_currentTime = Time::currentTimeMillis();
    m_timePassed = float(m_currentTime - m_previousTime);

    switch (m_logicOp)
    {
    case 0:
        //AND: as soon as AND is true send TTL output
        if (m_timePassed < m_window)
        {
            if (A && B)
            {
                std::cout << "AND condition satisfied ";
                triggerEvent();

                if ((m_input1gate == m_input2gate))
                {
                    std::cout << "resetting input" << std::endl;
                    A = false;
                    B = false;
                }
                else if (!m_input1gate)
                {
                    std::cout << "resetting A" << std::endl;
                    A = false;
                }
                else if (!m_input2gate)
                {
                    std::cout << "resetting B" << std::endl;
                    B = false;
                }
            }
            m_previousTime = Time::currentTimeMillis();
        }
        else
        {
            A = false;
            B = false;
        }
        break;

    case 1:
        //OR: if OR is true send TTL at the end of thw window
        if (m_timePassed > m_window)
        {
            if (A || B)
            {
                std::cout << "OR condition satisfied: resetting input" << std::endl;
                triggerEvent();

                A = false;
                B = false;
            }
            else
            {
                A = false;
                B = false;
            }
            m_previousTime = Time::currentTimeMillis();
        }
        break;

    case 2:
        //XOR: if XOR is true send TTL at the end of thw window
        if (m_timePassed > m_window)
        {
            if (A != B)
            {
                std::cout << "XOR condition satisfied: resetting input" << std::endl;
                triggerEvent();

                A = false;
                B = false;
            }
            else if ((A == B) && (A))
            {
                std::cout << "XOR condition NOT satisfied: resetting input" << std::endl;
                A = false;
                B = false;
            }
            else
            {
                A = false;
                B = false;
            }
            m_previousTime = Time::currentTimeMillis();
        }
        break;

    case 3: //DELAY
        if (m_timePassed > m_window)
        {
            if (A)
            {
                std::cout << "DELAY A" << std::endl;
                triggerEvent();
                A = false;
            }
            m_previousTime = Time::currentTimeMillis();
        }
        break;
    }
}

void LogicGate::triggerEvent()
{
    int64 timestamp = CoreServices::getGlobalTimestamp();
    setTimestampAndSamples(timestamp, 0);
    uint8 ttlData = 1 << m_outputChan;
    const EventChannel* chan = getEventChannel(getEventChannelIndex(0, getNodeId()));
    TTLEventPtr event = TTLEvent::createTTLEvent(chan, timestamp, &ttlData, sizeof(uint8), m_outputChan);
    addEvent(chan, event, 0);

    int eventDurationSamp = static_cast<int>(ceil(m_pulseDuration / 1000.0f * getSampleRate()));
    uint8 ttlDataOff = 0;
    TTLEventPtr eventOff = TTLEvent::createTTLEvent(chan, timestamp + eventDurationSamp, &ttlDataOff, sizeof(uint8), m_outputChan);
    addEvent(chan, eventOff, 0);
}

void LogicGate::addEventSource(EventSources s)
{
    m_sources.add (s);
}

void LogicGate::clearEventSources()
{
    m_sources.clear();
}


void LogicGate::saveCustomParametersToXml(XmlElement *parentElement)
{
    XmlElement* mainNode = parentElement->createNewChildElement("LogicGate");

    mainNode->setAttribute("input1", m_input1);
    mainNode->setAttribute("input2", m_input2);
    mainNode->setAttribute("input1gate", m_input1gate);
    mainNode->setAttribute("input2gate", m_input2gate);
    mainNode->setAttribute("logicOp", m_logicOp);
    mainNode->setAttribute("outputChan", m_outputChan);
    mainNode->setAttribute("window", m_window);
    mainNode->setAttribute("duration", m_pulseDuration);

}

void LogicGate::loadCustomParametersFromXml ()
{
    if (parametersAsXml != nullptr)
    {
        forEachXmlChildElement (*parametersAsXml, mainNode)
        {
            if (mainNode->hasTagName ("LogicGate"))
            {
                m_input1 = mainNode->getIntAttribute("input1");
                m_input2 = mainNode->getIntAttribute("input2");
                m_input1gate = mainNode->getBoolAttribute("input1gate");
                m_input2gate = mainNode->getBoolAttribute("input2gate");
                m_logicOp = mainNode->getIntAttribute("logicOp");
                m_outputChan = mainNode->getIntAttribute("outputChan");
                m_window = mainNode->getIntAttribute("window");
                m_pulseDuration = mainNode->getIntAttribute("duration");

                editor->updateSettings();
            }
        }
    }
}
