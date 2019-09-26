/*
  ==============================================================================

    BottomMenu.cpp
    Created: 25 Sep 2019 2:17:50pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BottomMenu.h"

//==============================================================================
BottomMenu::BottomMenu()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    newButton = new TextButton();
    newButton->setButtonText("New Model");
    newButton->addListener (this);
    addAndMakeVisible (newButton);
    
    muteButton = new ToggleButton();
    muteButton->setButtonText("Mute");
    muteButton->addListener (this);
    addAndMakeVisible (muteButton);
    
    cpuUsage.setColour(Label::textColourId, Colours::white);
    addAndMakeVisible (cpuUsage);
    
    graphicsLabel.setText ("Graphics (Hz): ", dontSendNotification);
    graphicsLabel.setColour (Label::textColourId, Colours::white);
    addAndMakeVisible (graphicsLabel);
    
    graphicsSlider.setRange (0.1, 60, 0.1);
    graphicsSlider.setValue (15);
    graphicsSlider.addListener (this);
    addAndMakeVisible (graphicsSlider);
    
    modelButtons.add (new TextButton());
    modelButtons[0]->setButtonText("Create String");
    addAndMakeVisible (modelButtons[0]);
    modelButtons[0]->addListener (this);
    
    modelButtons.add (new TextButton());
    modelButtons[1]->setButtonText("Create Bar");
    addAndMakeVisible (modelButtons[1]);
    modelButtons[1]->addListener (this);
    
}

BottomMenu::~BottomMenu()
{
}

void BottomMenu::paint (Graphics& g)
{
    g.setColour(Colours::grey);
    g.drawLine(0, 0, getWidth(), 0, 5.0f);
}

void BottomMenu::resized()
{
    Rectangle<int> lowerButtonArea = getLocalBounds();
    lowerButtonArea.reduce (GUIDefines::margin, GUIDefines::margin);
    newButton->setBounds (lowerButtonArea.removeFromLeft(100));
    lowerButtonArea.removeFromLeft (GUIDefines::margin);
    
    muteButton->setBounds (lowerButtonArea.removeFromRight(GUIDefines::buttonWidth));
    lowerButtonArea.removeFromRight (GUIDefines::margin);
    cpuUsage.setBounds (lowerButtonArea.removeFromLeft(75));
    
    
    graphicsSlider.setBounds (lowerButtonArea.removeFromRight (150));
    lowerButtonArea.removeFromRight (GUIDefines::margin);
    
    graphicsLabel.setBounds(lowerButtonArea.removeFromRight (75));
    int widthPerButton = (lowerButtonArea.getWidth() - GUIDefines::margin) / modelButtons.size();
    lowerButtonArea.removeFromLeft (GUIDefines::margin);
    
    for (auto modelButton : modelButtons)
    {
        modelButton->setBounds (lowerButtonArea.removeFromLeft(widthPerButton));
        lowerButtonArea.removeFromLeft (GUIDefines::margin);
    }
}


void BottomMenu::buttonClicked (Button* button)
{
    
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        button->setState (Button::ButtonState::buttonNormal);
        action = caughtReturnKey;
        sendChangeMessage();
        return;
    }

    if (button == newButton)
    {
        if (appState == normalAppState)
            action = newObjectAction;
        else if (appState == newObjectState) // new model has been cancelled
            action = cancelNewObjectAction;
    }
    
    if (button == muteButton)
    {
        action = muteAction;
    }
    
    if (button == modelButtons[0])
    {
        model = "string";
        action = createPredefinedModel;
    }
    
    if (button == modelButtons[1])
    {
        model = "bar";
        action = createPredefinedModel;
    }
    
    sendChangeMessage();
}

void BottomMenu::sliderValueChanged (Slider* slider)
{
    if (slider == &graphicsSlider)
    {
        action = sliderMoved;
    }
    sendChangeMessage();
}

void BottomMenu::setApplicationState (ApplicationState state)
{
    appState = state;
    switch (state) {
        case newObjectState:
            newButton->setEnabled (true);
            newButton->setButtonText ("CANCEL");
            for (auto modelButton : modelButtons)
                modelButton->setEnabled (false);
            break;
        case normalAppState:
            newButton->setEnabled (true);
            newButton->setButtonText ("New Model");
            for (auto modelButton : modelButtons)
                modelButton->setEnabled (true);
            break;
        case editObjectState:
            newButton->setEnabled (false);
            for (auto modelButton : modelButtons)
                modelButton->setEnabled (false);
            break;
        default:
            break;
    }
}

void BottomMenu::refresh (double cpuUsageFromDevice)
{
    double cpu = static_cast<int>(cpuUsageFromDevice * 1000.0) / 10.0;
    cpuUsage.setText ("CPU: " + String (cpu) + " %", dontSendNotification);
}
