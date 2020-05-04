/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2014 Open Ephys

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

#include "LogicGateEditor.h"
#include "LogicGate.h"

LogicGateEditor::LogicGateEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
    : GenericEditor(parentNode, useDefaultParameterEditors)
    , m_input1Selected(1)
    , m_input2Selected(1)
    , m_logicOp(1)
    , m_outputChan(1)
{
    tabText = "LogicGate";
    desiredWidth = 300;

    input1Selector = new ComboBox();
    input1Selector->setBounds(20,30,160,20);
    input1Selector->addListener(this);

    addAndMakeVisible(input1Selector);

    input2Selector = new ComboBox();
    input2Selector->setBounds(20,60,160,20);
    input2Selector->addListener(this);

    addAndMakeVisible(input2Selector);

    logicSelector = new ComboBox();
    logicSelector->setBounds(220,50,60,20);
    logicSelector->addListener(this);

    logic_op.add("AND");
    logic_op.add("OR");
    logic_op.add("XOR");
    logic_op.add("DELAY");

    for (int i = 0; i < 4; i++)
        logicSelector->addItem(logic_op[i], i+1);
    logicSelector->setSelectedId(m_logicOp, dontSendNotification);

    addAndMakeVisible(logicSelector);

    outputChans = new ComboBox("Output");

    outputChans->setEditableText(false);
    outputChans->setBounds(220,110,60,20);
    outputChans->addListener(this);

    for (int i=1; i<9; i++)
        outputChans->addItem(String(i), i);

    outputChans->setSelectedId(m_outputChan, dontSendNotification);
    addAndMakeVisible(outputChans);


    gate1Button = new UtilityButton("o", titleFont);
    gate1Button->addListener(this);
    gate1Button->setRadius(3.0f);
    gate1Button->setBounds(190,30,20,20);
    gate1Button->setClickingTogglesState(true);

    addAndMakeVisible(gate1Button);

    gate2Button = new UtilityButton("o", titleFont);
    gate2Button->addListener(this);
    gate2Button->setRadius(3.0f);
    gate2Button->setBounds(190,60,20,20);
    gate2Button->setClickingTogglesState(true);

    addAndMakeVisible(gate2Button);

    input1Label = new Label ("i1", "A");
    input1Label->setBounds (0,30,20,20);
    addAndMakeVisible (input1Label);

    input2Label = new Label ("i2", "B");
    input2Label->setBounds (0,60,20,20);
    addAndMakeVisible (input2Label);

    logicLabel = new Label ("log", "OPERATOR");
    logicLabel->setBounds (220,30,70,20);
    addAndMakeVisible (logicLabel);

    outputLabel = new Label ("out", "OUTPUT");
    outputLabel->setBounds (220,90,70,20);
    addAndMakeVisible (outputLabel);

    windowLabel = new Label ("window", "Window (ms):");
    windowLabel->setBounds (20,85,115,20);
    addAndMakeVisible (windowLabel);

    windowEditLabel = new Label ("window_edit", String(DEF_WINDOW));
    windowEditLabel->setBounds (140,85,70,20);
    windowEditLabel->setFont (Font ("Default", 15, Font::plain));
    windowEditLabel->setColour (Label::textColourId, Colours::white);
    windowEditLabel->setColour (Label::backgroundColourId, Colours::grey);
    windowEditLabel->setEditable (true);
    windowEditLabel->addListener (this);
    addAndMakeVisible (windowEditLabel);

    durationLabel = new Label ("duration", "Duration (ms):");
    durationLabel->setBounds (20,110,115,20);
    addAndMakeVisible (durationLabel);

    durationEditLabel = new Label ("duration_edit", String(2));
    durationEditLabel->setBounds (140,110,70,20);
    durationEditLabel->setFont (Font ("Default", 15, Font::plain));
    durationEditLabel->setColour (Label::textColourId, Colours::white);
    durationEditLabel->setColour (Label::backgroundColourId, Colours::grey);
    durationEditLabel->setEditable (true);
    durationEditLabel->addListener (this);
    addAndMakeVisible (durationEditLabel);
}


LogicGateEditor::~LogicGateEditor()
{
}

void LogicGateEditor::updateSettings()
{
    EventSources s;
    String name;
    LogicGate* processor = (LogicGate*) getProcessor();
    processor->clearEventSources();
    input1Selector->clear();
    input2Selector->clear();
    input1Selector->addItem("Select", 1);
    input2Selector->addItem("Select", 1);
    int nextItem1 = 2;
    int nextItem2 = 2;
    int nEvents = processor->getTotalEventChannels();
    for (int i = 0; i < nEvents; i++)
    {
        const EventChannel* event = processor->getEventChannel(i);

        if (event->getSourceNodeID() != processor->getNodeId())
        {
            // Count how many processors of same type
            int n = 0;
            for (int j = 0; j < i; j++)
            {
                const EventChannel* event1 = processor->getEventChannel(j);
                if (!event->getSourceName().compare(event1->getSourceName()))
                    n++;
            }

            if (event->getChannelType() == EventChannel::TTL)
            {
                s.eventIndex = event->getSourceIndex();
                s.sourceId = event->getSourceNodeID();
                int nChans = event->getNumChannels();
                for (int c = 0; c < nChans; c++)
                {
                    s.channel = c;
                    name = event->getSourceName() + " " + String(event->getSourceIndex() + n + 1) + " (TTL" + String(c+1) + ")";
                    processor->addEventSource(s);
                    input1Selector->addItem(name, nextItem1++);
                    input2Selector->addItem(name, nextItem2++);
                }
            }
        }
    }

    LogicGate* p = (LogicGate*) getProcessor();
    m_input1Selected = p->getInput1() + 2; // first is select
    m_input2Selected = p->getInput2() + 2;
    m_logicOp = p->getLogicOp() + 1;
    m_outputChan = p->getOutput() + 1;

    windowEditLabel->setText(String(p->getWindow()), dontSendNotification);
    durationEditLabel->setText(String(p->getTtlDuration()), dontSendNotification);

    if (p->getGate1())
        if (!gate1Button->getState())
            gate1Button->triggerClick();
    if (p->getGate2())
        if (!gate2Button->getState())
            gate2Button->triggerClick();

    if (m_input1Selected > input1Selector->getNumItems())
        m_input1Selected = input1Selector->getNumItems();
    input1Selector->setSelectedId(m_input1Selected);

    if (m_input2Selected > input2Selector->getNumItems())
        m_input2Selected = input2Selector->getNumItems();
    input2Selector->setSelectedId(m_input2Selected);

    if (m_logicOp > logicSelector->getNumItems())
        m_logicOp = logicSelector->getNumItems();
    logicSelector->setSelectedId(m_logicOp);

    if (m_outputChan > outputChans->getNumItems())
        m_outputChan = outputChans->getNumItems();
    outputChans->setSelectedId(m_outputChan);
}

void LogicGateEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    LogicGate* processor = (LogicGate*) getProcessor();
    if (comboBoxThatHasChanged == input1Selector)
    {
        processor->setInput1(comboBoxThatHasChanged->getSelectedId() - 2);
        if (comboBoxThatHasChanged->getSelectedId() > 0)
            m_input1Selected = comboBoxThatHasChanged->getSelectedId();
        else
            m_input1Selected = 1;
    }
    else if (comboBoxThatHasChanged == input2Selector)
    {
        processor->setInput2(comboBoxThatHasChanged->getSelectedId() - 2);
        if (comboBoxThatHasChanged->getSelectedId() > 0)
            m_input2Selected = comboBoxThatHasChanged->getSelectedId();
        else
            m_input2Selected = 1;
    }
    else if (comboBoxThatHasChanged == logicSelector)
    {
        m_logicOp = comboBoxThatHasChanged->getSelectedId() - 1;
        processor->setLogicOp(m_logicOp);

        if (m_logicOp == 3)
        {
            input2Selector->setVisible(false);
            input2Label->setVisible(false);
            gate2Button->setVisible(false);
        }
        else
        {
            input2Selector->setVisible(true);
            input2Label->setVisible(true);
            gate2Button->setVisible(true);
        }
    }
    else if (comboBoxThatHasChanged == outputChans)
    {
        m_outputChan = comboBoxThatHasChanged->getSelectedId() - 1;
        processor->setOutput((int) m_outputChan);
    }
}

void LogicGateEditor::labelTextChanged (Label* labelThatHasChanged)
{
    if (labelThatHasChanged == windowEditLabel)
    {
        Value val = labelThatHasChanged->getTextValue();
        int value = int(val.getValue()); //only multiple of 100us
        if (value>=0)
        {
            LogicGate* processor = (LogicGate*) getProcessor();
            processor->setWindow(value);
            labelThatHasChanged->setText(String(value), dontSendNotification);
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be greater or equal than 0!");
            labelThatHasChanged->setText("", dontSendNotification);
        }
    }
    else if (labelThatHasChanged == durationEditLabel)
    {
        Value val = labelThatHasChanged->getTextValue();
        int value = int(val.getValue()); //only multiple of 100us
        if (value>=0)
        {
            LogicGate* processor = (LogicGate*) getProcessor();
            processor->setTtlDuration(value);
            labelThatHasChanged->setText(String(value), dontSendNotification);
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be greater or equal than 0!");
            labelThatHasChanged->setText("", dontSendNotification);
        }
    }

}

void LogicGateEditor::buttonEvent(Button* button)
{
    LogicGate* processor = (LogicGate*) getProcessor();
    if (button == gate1Button)
    {
        if (button->getToggleState()==true)
            processor->setGate1(true);
        else
            processor->setGate1(false);
    }
    else if (button == gate2Button)
    {
        if (button->getToggleState()==true)
            processor->setGate2(true);
        else
            processor->setGate2(false);
    }

}

void LogicGateEditor::saveCustomParameters(XmlElement* xml)
{

}


void LogicGateEditor::loadCustomParameters(XmlElement* xml)
{

}
