/*
  ==============================================================================

    AddCoefficient.h
    Created: 12 Jul 2019 2:54:45pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class AddCoefficient    : public Component, public Button::Listener
{
public:
    AddCoefficient();
    ~AddCoefficient();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked (Button* button) override;
    
    String getCoeffName() { return coefficientName; };
    double getValue() { return value; };
    
    bool isDynamic() { return dynamicBool; };
    void setDynamic (bool val) { dynamicBool = val; dynamic.setToggleState (val, dontSendNotification); };
    
private:
    TextEditor coeffTextBox;
    TextEditor valueTextBox;
    Label addCoeffLabel;
    Label valueLabel;
    TextButton addCoeff;
    ToggleButton dynamic;
    
    String coefficientName;
    double value;
    bool dynamicBool = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddCoefficient)
};
