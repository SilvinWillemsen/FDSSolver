/*
  ==============================================================================

    BottomMenu.h
    Created: 25 Sep 2019 2:17:50pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
//==============================================================================
/*
    Contains buttons and sliders located at the bottom of the application
*/
class BottomMenu    : public Component,
                      public Button::Listener,
                      public Slider::Listener,
                      public ChangeBroadcaster
{
public:
    BottomMenu();
    ~BottomMenu();

    void paint (Graphics&) override;
    void resized() override;
    
    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    
    Action getAction() { return action; };
    String getModel() { return model; };
//
    double getGraphicsSliderValue() { return graphicsSlider.getValue(); };

    void setApplicationState (ApplicationState state);
    
    void refresh (double cpuUsageFromDevice);
    
private:
    
    ScopedPointer<TextButton> newButton;
    Label cpuUsage;
    OwnedArray<TextButton> modelButtons;
    String model = "";
    OwnedArray<Slider> coeffSliders;
    
    ScopedPointer<ToggleButton> muteButton;
    
    Label graphicsLabel;
    Slider graphicsSlider;
    
    Action action = noAction;
    ApplicationState appState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomMenu)
};
