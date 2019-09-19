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
    void update (bool dyn, double val, bool init = false);
    
    Action getAction() { return action; }
    
    double getSliderValue() { return slider.getValue(); };
    double getValue() { return value; };

    void clickCoeffButton() { buttonClicked (&coefficientButton); };
    
    void setApplicationState (ApplicationState applicationState, bool init);
    
private:
//    String name;
    double value;
    
    TextButton coefficientButton;
    TextButton editButton;
    TextButton removeButton;

    Slider slider;
    Label label;
    Label dynamicCoeffLabel;
    
    bool dynamic = false;
    
    Action action = noAction;
    
    ApplicationState appState;
//     Coefficient coeff (const String& name, double value, bool dynamic)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoefficientComponent)
};
