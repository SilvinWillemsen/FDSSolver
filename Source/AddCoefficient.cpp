/*
  ==============================================================================

    AddCoefficient.cpp
    Created: 12 Jul 2019 2:54:45pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "AddCoefficient.h"
#include "GUIDefines.h"
//==============================================================================
AddCoefficient::AddCoefficient()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible (coeffTextBox);
    addCoeffLabel.setColour (Label::textColourId, Colours::black);
    addCoeffLabel.setText ("Coefficient Name", dontSendNotification);
    addAndMakeVisible (addCoeffLabel);

    addAndMakeVisible (valueTextBox);
    valueLabel.setColour (Label::textColourId, Colours::black);
    valueLabel.setText ("Value", dontSendNotification);
    addAndMakeVisible (valueLabel);

    coeffTextBox.setInputRestrictions (3, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    valueTextBox.setInputRestrictions (0, "0123456789.");
    
    addCoeff.setButtonText ("Add Coefficient");
    addAndMakeVisible (addCoeff);
    addCoeff.addListener (this);
    setSize (300, 10);
}

AddCoefficient::~AddCoefficient()
{
}

void AddCoefficient::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
}

void AddCoefficient::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    Rectangle<int> labelArea = getLocalBounds().removeFromTop(getHeight() / 2.0 - GUIDefines::buttonHeight / 2.0);
    labelArea.removeFromTop (labelArea.getHeight() - GUIDefines::buttonHeight);
    int labelStartMargin = 5;
    labelArea.removeFromLeft (GUIDefines::margin - labelStartMargin);
    int totalBoxWidth = labelArea.getWidth() - GUIDefines::margin * 2;
    
    addCoeffLabel.setBounds (labelArea.removeFromLeft (0.5 * totalBoxWidth));
    labelArea.removeFromLeft (GUIDefines::margin);
    valueLabel.setBounds (labelArea.removeFromLeft (0.25 * totalBoxWidth));
    
    Rectangle<int> drawArea (0, getHeight() / 2.0 - GUIDefines::buttonHeight / 2.0, getWidth(), GUIDefines::buttonHeight);
    drawArea.removeFromLeft (GUIDefines::margin);
    drawArea.removeFromRight (GUIDefines::margin);
    
    coeffTextBox.setBounds (drawArea.removeFromLeft (0.5 * totalBoxWidth));
    drawArea.removeFromLeft (GUIDefines::margin);
    valueTextBox.setBounds (drawArea.removeFromLeft (0.25 * totalBoxWidth));
    drawArea.removeFromLeft (GUIDefines::margin);
    addCoeff.setBounds (drawArea);
}

void AddCoefficient::buttonClicked (Button* button)
{
    if (coeffTextBox.isEmpty())
    {
        std::cout << "No coefficient given" << std::endl;
    }
    else if (valueTextBox.isEmpty())
    {
        std::cout << "No value given" << std::endl;
    }
    
    coefficientName = coeffTextBox.getText();
    value = valueTextBox.getText().getDoubleValue();
//    std::cout << coefficientName << std::endl;
    
    coeffTextBox.setText("");
    valueTextBox.setText("");
    
    if (DialogWindow* dw = this->findParentComponentOfClass<DialogWindow>())
        dw->exitModalState (0);
    
}
