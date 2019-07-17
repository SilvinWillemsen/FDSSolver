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
CoefficientComponent::CoefficientComponent (const String& name, double val, bool dyn) : name (name),
                                                                                        value (val),
                                                                                        slider(Slider::LinearHorizontal, Slider::NoTextBox),
                                                                                        dynamic (dyn)
{
    // slider
    slider.setName (name);
    slider.setPopupDisplayEnabled (true, true, getParentComponent(), -1);
    slider.addListener (this);
    addAndMakeVisible (slider);
    
    // label
    label.setFont (Font("Latin Modern Math", "Regular", 16.0));
    label.setColour (Label::textColourId, Colours::white);
    addAndMakeVisible (label);
    
    update (dynamic, value);
    
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
}

void CoefficientComponent::paint (Graphics& g)
{

}

void CoefficientComponent::resized()
{
    Rectangle<int> totalArea = getLocalBounds();
    
    coefficientButton.setBounds (totalArea.removeFromLeft (GUIDefines::buttonWidth));
    removeButton.setBounds (totalArea.removeFromRight (GUIDefines::buttonHeight));
    totalArea.removeFromRight (GUIDefines::margin);
    editButton.setBounds (totalArea.removeFromRight (GUIDefines::buttonHeight));
    totalArea.removeFromLeft (GUIDefines::margin);
    totalArea.removeFromRight (GUIDefines::margin);
    
    if (dynamic)
    {
        slider.setBounds (totalArea);
    } else {
        label.setBounds (totalArea);
    }
    
    totalArea.removeFromLeft (GUIDefines::margin);
}

void CoefficientComponent::sliderValueChanged (Slider* slider)
{
    value = slider->getValue();
    action = sliderMoved;
    sendChangeMessage();
}

void CoefficientComponent::buttonClicked (Button* button)
{
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

void CoefficientComponent::update (bool dyn, double val)
{
    // if changed from static to dynamic or vice versa
    if (dyn != dynamic)
    {
        slider.setVisible (dyn);
        label.setVisible (!dyn);
    }
    
    dynamic = dyn;
    
    if (dynamic)
    {
        slider.setRange(0.0, val);
        slider.setValue (val);
    } else {
        label.setText(" = " + String (val), dontSendNotification);
    }
    
    resized();
}
