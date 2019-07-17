/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "CoefficientComponent.h"
#include "FDSsolver.h"
#include "AddCoefficient.h"
#include "Object1D.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
enum ApplicationState
{
    normalAppState,
    editObjectState,
};

enum CoeffPopupState
{
    normalCoeffState,
    editingCoeff,
};

class MainComponent   : public AudioAppComponent,
                        public Button::Listener,
                        public Slider::Listener,
                        public Timer,
                        public KeyListener,
                        public ChangeListener
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

    void changeListenerCallback (ChangeBroadcaster* source) override;
    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    
    
    void refresh();
    
    // Use for one character
    String encoder (String equation);
    
    // Use for multiple characters
    String decoder (String equation);
    
    void timerCallback() override;
    double clip (double output, double min = -1.0, double max = 1.0);
    
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    
private:
    //==============================================================================
    // Your private member variables go here...
    ScopedPointer<FDSsolver> fdsSolver = nullptr;
    ScopedPointer<Equation> eq = nullptr;
    double fs;
    int bufferSize;
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
    TextButton* deltaCentT = nullptr;
    TextButton* deltaTT = nullptr;
    
    TextButton* deltaForwX = nullptr;
    TextButton* deltaBackX = nullptr;
    TextButton* deltaCentX = nullptr;
    TextButton* deltaXX = nullptr;
    
    TextButton* minusSign = nullptr;
    
    TextButton* coeff = nullptr;
    TextButton* backSpace = nullptr;
    
    ScopedPointer<Label> textBox = nullptr;
    TextButton* uLN = nullptr;
    
    String equation;
    StringCode stringCode;

    ScopedPointer<AddCoefficient> addCoeffWindow;
    NamedValueSet coefficients;
    
    OwnedArray<CoefficientComponent> coefficientComponents;
    
    ScopedPointer<Label> coeffTopLabel;
    OwnedArray<Label> coeffLabels;
    std::vector<bool> coeffDynamic;

    OwnedArray<Object1D> objects;
    
    ApplicationState appState = normalAppState;
    CoeffPopupState coeffPopupState = normalCoeffState;
    
    Object1D* editingObject;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
