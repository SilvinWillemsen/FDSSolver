/*
  ==============================================================================

    CoefficientComponent.cpp
    Created: 17 Jul 2019 12:50:58pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "CoefficientComponent.h"

//==============================================================================
CoefficientComponent::CoefficientComponent (const String& name, double val, bool dyn) : value (val),
                                                                            slider (Slider::LinearHorizontal, Slider::NoTextBox),
                                                                            dynamic (dyn)
{
    this->setName (name);
    // slider
    slider.setName (name);
    slider.setPopupDisplayEnabled (true, true, getParentComponent(), -1);
    slider.addListener (this);
    addAndMakeVisible (slider);

    // labels
    label.setFont (Font("Latin Modern Math", "Regular", 16.0));
    label.setColour (Label::textColourId, Colours::white);
    addAndMakeVisible (label);

    dynamicCoeffLabel.setFont (Font("Latin Modern Math", "Regular", 16.0));
    dynamicCoeffLabel.setColour (Label::textColourId, Colours::white);
    dynamicCoeffLabel.setText(getName() + " = ", dontSendNotification);
    addAndMakeVisible (dynamicCoeffLabel);
    
    update (dynamic, value, true);

    // buttons
    coefficientButton.setName (name);
    coefficientButton.setButtonText (name);
    coefficientButton.addListener (this);
    addAndMakeVisible (coefficientButton);

    editButton.setName (name);
    editButton.setButtonText ("E");
    editButton.addListener (this);
    addAndMakeVisible (editButton);

    removeButton.setName (name);
    removeButton.setButtonText (String (CharPointer_UTF8 ("\xc3\x97")));
    removeButton.addListener (this);
    addAndMakeVisible (removeButton);
    
}

CoefficientComponent::~CoefficientComponent()
{
    std::cout << getName() << " is removed from the heap." << std::endl;
}

void CoefficientComponent::paint (Graphics& g)
{

}

void CoefficientComponent::resized()
{
    Rectangle<int> totalArea = getLocalBounds();
    totalArea.reduce (GUIDefines::margin / 2.0, GUIDefines::margin / 2.0);
    
    if (appState != normalAppState)
    {
        coefficientButton.setBounds (totalArea.removeFromLeft (GUIDefines::buttonWidth));
        removeButton.setBounds (totalArea.removeFromRight (GUIDefines::buttonHeight));
        totalArea.removeFromRight (GUIDefines::margin);
        editButton.setBounds (totalArea.removeFromRight (GUIDefines::buttonHeight));
        totalArea.removeFromLeft (GUIDefines::margin);
        totalArea.removeFromRight (GUIDefines::margin);
    }
    else if (dynamic)
    {
        dynamicCoeffLabel.setBounds(totalArea.removeFromLeft (GUIDefines::buttonWidth));
    }
    
    if (dynamic)
    {
        slider.setBounds (totalArea);
    } else {
        label.setBounds (totalArea);
    }
    
}

void CoefficientComponent::sliderValueChanged (Slider* slider)
{
    value = slider->getValue();
    action = sliderMoved;
    sendChangeMessage();
}

void CoefficientComponent::buttonClicked (Button* button)
{
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        button->setState (Button::ButtonState::buttonNormal);
        action = caughtReturnKey;
        sendChangeMessage();
        return;
    }
    
    if (button == &coefficientButton)
    {
        action = insertCoeff;
    }
    if (button == &editButton)
    {
        action = editCoeff;
    }
    if (button == &removeButton)
    {
        action = removeCoeff;
    }
    
    sendChangeMessage();
}

void CoefficientComponent::update (bool dyn, double val, bool init)
{
    // if changed from static to dynamic or vice versa
    if (dyn != dynamic)
    {
        slider.setVisible (dyn);
        label.setVisible (!dyn);
    }
    
    dynamic = dyn;
    value = val;
    
    if (dynamic)
    {
        slider.setRange (0.0, val);
        slider.setValue (val);
    } else {
        label.setText ((appState == normalAppState ? getName() : "") + " = " + String (val), dontSendNotification);
    }
    
    if (appState == normalAppState)
    {
        coefficientButton.setVisible (false);
        editButton.setVisible (false);
        removeButton.setVisible (false);
    } else {
        coefficientButton.setVisible (true);
        editButton.setVisible (true);
        removeButton.setVisible (true);
    }
    
    resized();
}

void CoefficientComponent::setApplicationState (ApplicationState applicationState)
{
    appState = applicationState;
    update (dynamic, value);
}
