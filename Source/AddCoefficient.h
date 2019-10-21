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

enum CoeffPopupState
{
    normalCoeffState,
    editingCoeff,
};


class AddCoefficient    : public Component,
                          public Button::Listener,
                          public TextEditor::Listener
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
    
    void setCoeffName (String name) { coeffTextBox.setText (name); };
    void setCoeffValue (var value = -1);
    
    TextEditor* getCoeffTextBoxPtr() { return &coeffTextBox; };
    
    void setCoeffPopupState (CoeffPopupState state) { coeffPopupState = state; };
    
    void setKeyboardFocus() { initKeyboardFocus = true; };

    void textEditorReturnKeyPressed (TextEditor& textBox) override;
    
    CoeffPopupState getCoeffPopupState() { return coeffPopupState; };
private:
    TextEditor coeffTextBox;
    TextEditor valueTextBox;
    
    int whichTextBox = 0;
    Label addCoeffLabel;
    Label valueLabel;
    TextButton addCoeff;
    ToggleButton dynamic;
    
    String coefficientName;
    double value;
    bool dynamicBool = false;
    bool initKeyboardFocus = true;
    
    CoeffPopupState coeffPopupState = normalCoeffState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddCoefficient)
};
