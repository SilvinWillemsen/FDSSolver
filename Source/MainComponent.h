/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Calculator.h"
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
    
    void createPhysicalModel();
    void editPhysicalModel();
    
    void addCoefficient();
    
    void refresh();
    
    void timerCallback() override;
    double clip (double output, double min = -1.0, double max = 1.0);
    
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    
    int testFunc (int test);
private:
    //==============================================================================
    // Your private member variables go here...
    Calculator* calculator;
    
    ScopedPointer<FDSsolver> fdsSolver = nullptr;
    ScopedPointer<Equation> eq = nullptr;
    double fs;
    int bufferSize;
    
    OwnedArray<TextButton> coeffButtons;
    OwnedArray<Slider> coeffSliders;

    ScopedPointer<AddCoefficient> addCoeffWindow;
    
    OwnedArray<CoefficientComponent> coefficientComponents;
    
    ScopedPointer<Label> coeffTopLabel;
    OwnedArray<Label> coeffLabels;
    std::vector<bool> coeffDynamic;

    NamedValueSet coefficients;
    
    OwnedArray<Object1D> objects;
    
    ApplicationState appState = normalAppState;
    CoeffPopupState coeffPopupState = normalCoeffState;
    
    Object1D* editingObject = nullptr;
    bool repaintFlag = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
