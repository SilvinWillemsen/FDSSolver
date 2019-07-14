/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "FDSsolver.h"
#include "AddCoefficient.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class MainComponent   : public AudioAppComponent,
                        public Button::Listener,
                        public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    
    void refresh();
    
    // Use for one character
    String encoder (String equation);
    
    // Use for multiple characters
    String decoder (String equation);
    
private:
    //==============================================================================
    // Your private member variables go here...
    ScopedPointer<FDSsolver> fdsSolver = nullptr;
    ScopedPointer<Equation> eq = nullptr;
    double fs;
    
    String deltaString;
    OwnedArray<TextButton> buttons;
    OwnedArray<TextButton> coeffButtons;
    OwnedArray<Slider> coeffSliders;
    OwnedArray<Label> labels;
    
    TextButton* createPM = nullptr;
    TextButton* clearEq = nullptr;
    
    int startOfOperators;
    
    TextButton* plus = nullptr;
    TextButton* minus = nullptr;
    TextButton* equals = nullptr;
    
    TextButton* deltaForwT = nullptr;
    TextButton* deltaBackT = nullptr;
    TextButton* deltaTT = nullptr;
    
    TextButton* deltaForwX = nullptr;
    TextButton* deltaBackX = nullptr;
    TextButton* deltaXX = nullptr;
    
    TextButton* minusSign = nullptr;
    
    TextButton* coeff = nullptr;
    TextButton* backSpace = nullptr;
    
    ScopedPointer<Label> textBox = nullptr;
    TextButton* uLN = nullptr;
    
    String equation;
    StringCode stringCode;

    AddCoefficient* addCoeffWindow;
    NamedValueSet coefficients;
    
    ScopedPointer<Label> coeffTopLabel;
    OwnedArray<Label> coeffLabels;
    std::vector<bool> coeffDynamic;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
