/*
  ==============================================================================

    Calculator.h
    Created: 19 Jul 2019 12:48:03pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Equation.h"

//==============================================================================
/*
    Class containing all the buttons and equation textbox to create FDSs and turn them into PMs
*/

class Calculator    : public Component,
                      public Button::Listener,
                      public KeyListener,
                      public ChangeBroadcaster

{
public:
    Calculator();
    ~Calculator();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked (Button* button) override;
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    bool refresh();
    TextButton* getButton (String buttonName);
    
    void clickButton (String buttonName) { for (auto button : buttons) if (button->getName() == buttonName) buttonClicked (button); }
    
    Action getAction() { return action; };
    
    // Equation string functions
    void setEquationString (String eqString) { equationString = eqString; textBox->setText (decoder (equationString), dontSendNotification); };
    String getEquationString() { return equationString; };
    void addToEquation (String stringToAdd) { equationString += stringToAdd; refresh(); };
    void clearEquation() { equationString = ""; refresh(); };
    
    // Use for one character
    String encoder (String equation);
    
    // Use for multiple characters
    String decoder (String equation);
    
    void setApplicationState (ApplicationState state);
    
    bool createPMalreadyClicked = false;
    bool returnKeyIsDown = false;
    
private:
    OwnedArray<TextButton> buttons;
    OwnedArray<Label> labels;
    String equationString;
    
    StringCode stringCode;
    
    ScopedPointer<Label> textBox = nullptr;
    
    TextButton* createPM = nullptr;
    TextButton* clearEq = nullptr;
    
    int startOfOperators;
    String deltaString;
    
    TextButton* plus = nullptr;
    TextButton* minus = nullptr;
    TextButton* equals = nullptr;
    
    TextButton* deltaForwT = nullptr;
    TextButton* deltaBackT = nullptr;
    TextButton* deltaCentT = nullptr;
    TextButton* deltaTT = nullptr;
    
    TextButton* deltaForwX = nullptr;
    TextButton* deltaBackX = nullptr;
    TextButton* deltaCentX = nullptr;
    TextButton* deltaXX = nullptr;
    
    TextButton* minusSign = nullptr;
    
    TextButton* coeff = nullptr;
    TextButton* backSpace = nullptr;
    
    TextButton* uLN = nullptr;
    
    Action action = noAction;
    
    ApplicationState appState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Calculator)
};
