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
    addCoeffLabel.setColour (Label::textColourId, Colours::white);
    addCoeffLabel.setText ("Coefficient Name", dontSendNotification);
    addAndMakeVisible (addCoeffLabel);

    addAndMakeVisible (valueTextBox);
    valueLabel.setColour (Label::textColourId, Colours::white);
    valueLabel.setText ("Value", dontSendNotification);
    addAndMakeVisible (valueLabel);

    coeffTextBox.setInputRestrictions (3, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    valueTextBox.setInputRestrictions (0, "0123456789.eE-");
    
    addCoeff.setButtonText ("Add Coefficient");
    addAndMakeVisible (addCoeff);
    addCoeff.addListener (this);
    
    dynamic.setButtonText ("Dynamic");
    dynamic.setColour (ToggleButton::textColourId, Colours::white);
    dynamic.setColour (ToggleButton::tickColourId, Colours::white);
    dynamic.setColour (ToggleButton::tickDisabledColourId, Colours::white);
    addAndMakeVisible (dynamic);
    dynamic.addListener (this);
    
    coeffTextBox.setEscapeAndReturnKeysConsumed (false);
    valueTextBox.setEscapeAndReturnKeysConsumed (false);
    
    coeffTextBox.addListener (this);
    valueTextBox.addListener (this);
    
    setSize (500, 10);
}

AddCoefficient::~AddCoefficient()
{
}

void AddCoefficient::paint (Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    if (initKeyboardFocus)
    {
        valueLabel.setText ("Value", dontSendNotification);
        if (coeffPopupState == normalCoeffState)
        {
            coeffTextBox.setEnabled (true);
            addCoeff.setButtonText ("Add Coefficient");
            coeffTextBox.grabKeyboardFocus();
        }
        else if (coeffPopupState == editingCoeff)
        {
            coeffTextBox.setEnabled (false);
            addCoeff.setButtonText ("Edit Coefficient");
            valueTextBox.grabKeyboardFocus();
        }
        initKeyboardFocus = false;
    }
}

void AddCoefficient::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    Rectangle<int> labelArea = getLocalBounds().removeFromTop(getHeight() / 2.0 - GUIDefines::buttonHeight / 2.0);
    labelArea.removeFromTop (labelArea.getHeight() - GUIDefines::buttonHeight);
    int labelStartMargin = 5;
    labelArea.removeFromLeft (GUIDefines::margin - labelStartMargin);
    int totalBoxWidth = labelArea.getWidth() - GUIDefines::margin * 3;
    
    addCoeffLabel.setBounds (labelArea.removeFromLeft (0.3 * totalBoxWidth));
    labelArea.removeFromLeft (GUIDefines::margin);
    valueLabel.setBounds (labelArea.removeFromLeft (0.2 * totalBoxWidth));
    
    Rectangle<int> drawArea (0, getHeight() / 2.0 - GUIDefines::buttonHeight / 2.0, getWidth(), GUIDefines::buttonHeight);
    drawArea.removeFromLeft (GUIDefines::margin);
    drawArea.removeFromRight (GUIDefines::margin);
    
    coeffTextBox.setBounds (drawArea.removeFromLeft (0.3 * totalBoxWidth));
    drawArea.removeFromLeft (GUIDefines::margin);
    valueTextBox.setBounds (drawArea.removeFromLeft (0.2 * totalBoxWidth));
    drawArea.removeFromLeft (GUIDefines::margin);
    dynamic.setBounds (drawArea.removeFromLeft (0.2 * totalBoxWidth));
    drawArea.removeFromLeft (GUIDefines::margin);
    addCoeff.setBounds (drawArea);

}

void AddCoefficient::buttonClicked (Button* button)
{
    if (button == &addCoeff)
    {
        DialogWindow* dw = this->findParentComponentOfClass<DialogWindow>();
        if (coeffTextBox.isEmpty())
        {
            std::cout << "No coefficient given" << std::endl;
            dw->exitModalState (2);
        }
        else if (valueTextBox.isEmpty())
        {
            std::cout << "No value given" << std::endl;
            dw->exitModalState (3);
        }
        else if (valueTextBox.getText().getDoubleValue() < 0)
        {
            std::cout << "No negative coefficients" << std::endl;
            dw->exitModalState (4);
        }
        
        coefficientName = coeffTextBox.getText();
        value = valueTextBox.getText().getDoubleValue();
        
        coeffTextBox.setText("");
        valueTextBox.setText("");
        dw->exitModalState (1);
    }
    else if (button == &dynamic)
    {
        coeffTextBox.focusGained (focusChangedDirectly);
        dynamicBool = !dynamicBool;
        valueLabel.setText (dynamicBool ? "Max value" : "Value", dontSendNotification);
    }
}

void AddCoefficient::setCoeffValue (var value)
{
    if (static_cast<int> (value) == -1)
        valueTextBox.setText("");
    else
        valueTextBox.setText (String(static_cast<double> (value)));
    
}
void AddCoefficient::textEditorReturnKeyPressed (TextEditor& textBox)
{
    buttonClicked (&addCoeff);
}
