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
#include "FDSsolver.h"

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
    Calculator (FDSsolver* fdsSolver);
    ~Calculator();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked (Button* button) override;
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    
    // Refresh the calculator (especially enabling / disabling buttons)
    bool refresh();
    
    // Get a specific button
    std::shared_ptr<TextButton> getButton (String buttonName);
    
    void clickButton (String buttonName) { for (auto button : buttons) if (button->getName() == buttonName) buttonClicked (button.get()); }
    
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
    
    void setDimension (int dim) { curDim = dim; };
    int getDimension() { return curDim; };
    
    bool createPMalreadyClicked = false;
    bool returnKeyIsDown = false;
    
    void changeDimension (int dim = -1);
    
private:
    std::vector<std::shared_ptr<TextButton>> buttons;
    OwnedArray<Label> labels;
    String equationString;
    
    StringCode stringCode;
    
    std::unique_ptr<Label> textBox;
    
    std::shared_ptr<TextButton> createPM;
    std::shared_ptr<TextButton> clearEq;
    
    int startOfOperators;
    String deltaString;
    
    std::shared_ptr<TextButton> plus;
    std::shared_ptr<TextButton> minus;
    std::shared_ptr<TextButton> equals;
    
    std::shared_ptr<TextButton> deltaForwT;
    std::shared_ptr<TextButton> deltaBackT;
    std::shared_ptr<TextButton> deltaCentT;
    std::shared_ptr<TextButton> deltaTT;
    
    std::shared_ptr<TextButton> deltaForwX;
    std::shared_ptr<TextButton> deltaBackX;
    std::shared_ptr<TextButton> deltaCentX;
    std::shared_ptr<TextButton> deltaXX;
    
    std::shared_ptr<TextButton> minusSign;
    
    std::shared_ptr<TextButton> coeff;
    std::shared_ptr<TextButton> backSpace;
    std::shared_ptr<TextButton> changeDim;
    
    std::shared_ptr<TextButton> uLN;
    
    Action action = noAction;
    
    ApplicationState appState;
    
    FDSsolver* fdsSolver;
    
    int curDim = 1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Calculator)
};
