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

#ifndef __LOGICGATEEDITOR_H_BB5F0ECC__
#define __LOGICGATEEDITOR_H_BB5F0ECC__

#include <EditorHeaders.h>
#include "LogicGate.h"

#define DEF_WINDOW 50
/**

  User interface for the LogicGate.

  @see LogicGate

*/

class LogicGateEditor : public GenericEditor,
        public ComboBox::Listener,
        public Label::Listener
{
public:
    LogicGateEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    virtual ~LogicGateEditor();
    void updateSettings();
    virtual void labelTextChanged (Label* labelThatHasChanged) override;
    void buttonEvent(Button* button);
    void comboBoxChanged(ComboBox* c);


private:
    Array<String> logic_op;

    int m_input1Selected;
    int m_input2Selected;
    int m_logicOp;
    int m_outputChan;

    ScopedPointer<ComboBox> logicSelector;
    ScopedPointer<ComboBox> input1Selector;
    ScopedPointer<ComboBox> input2Selector;
    ScopedPointer<ComboBox> outputChans;

    ScopedPointer<Label> input1Label;
    ScopedPointer<Label> input2Label;
    ScopedPointer<Label> logicLabel;
    ScopedPointer<Label> outputLabel;

    ScopedPointer<Label> windowLabel;
    ScopedPointer<Label> windowEditLabel;

    ScopedPointer<Label> durationLabel;
    ScopedPointer<Label> durationEditLabel;

    ScopedPointer<UtilityButton> gate1Button;
    ScopedPointer<UtilityButton> gate2Button;

    void saveCustomParameters(XmlElement* xml);
    void loadCustomParameters(XmlElement* xml);

    String tabText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogicGateEditor);
};


#endif  // __LogicGateEDITOR_H_BB5F0ECC__
