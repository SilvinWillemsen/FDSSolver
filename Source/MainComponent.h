/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Calculator.h"
#include "CoefficientList.h"
#include "FDSsolver.h"
#include "AddCoefficient.h"
#include "Object1D.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

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
    
    bool createPhysicalModel();
    bool editPhysicalModel();
    
    void addCoefficient();
    
    void refresh();
    
    void timerCallback() override;
    double clip (double output, double min = -1.0, double max = 1.0);
    
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged (bool isKeyDown, Component* originatingComponent) override;
    
    void changeAppState (ApplicationState applicationState);
    
private:
    //==============================================================================
    // Your private member variables go here...
    ScopedPointer<Calculator> calculator;
    
    ScopedPointer<FDSsolver> fdsSolver = nullptr;
    ScopedPointer<Equation> eq = nullptr;
    double fs;
    int bufferSize;
    
    OwnedArray<TextButton> coeffButtons;
    OwnedArray<Slider> coeffSliders;

    ScopedPointer<AddCoefficient> addCoeffWindow;
    
    CoefficientList coefficientList;

    OwnedArray<Label> coeffLabels;
    std::vector<bool> coeffDynamic;

    NamedValueSet coefficients;
    
    OwnedArray<Object1D> objects;
    
    ApplicationState appState;
    CoeffPopupState coeffPopupState = normalCoeffState;
    
    Object1D* editingObject = nullptr;
    Object1D* currentlySelectedObject = nullptr;
    bool repaintFlag = false;
    
    ScopedPointer<TextButton> newButton;
    ScopedPointer<TextButton> muteButton;
    Label cpuUsage;
    Label graphicsLabel;
    Slider graphicsSlider;
    
    bool mute = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
