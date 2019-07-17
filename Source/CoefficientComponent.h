/*
  ==============================================================================

    CoefficientComponent.h
    Created: 17 Jul 2019 12:50:58pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"

//==============================================================================
/*
*/

class CoefficientComponent    : public Component,
                                public Slider::Listener,
                                public Button::Listener,
                                public ChangeBroadcaster
{
public:
    CoefficientComponent (const String& name, double value, bool dynamic);
    ~CoefficientComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider* slider) override;
    void buttonClicked (Button* button) override;

    bool isDynamic() { return dynamic; };
    void update (bool dyn, double val);
    
    Action getAction() { return action; }
    
    String& getName() { return name; };
    
    double getSliderValue() { return slider.getValue(); };
    
    void clickCoeffButton() { buttonClicked (&coefficientButton); };
    
private:
    String name;
    double value;
    
    TextButton coefficientButton;
    TextButton editButton;
    TextButton removeButton;

    Slider slider;
    Label label;
    bool dynamic = false;
    
    Action action = noAction;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoefficientComponent)
};
