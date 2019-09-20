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

#include "Object0D.h"
#ifdef AVX_SUPPORTED
    #include "Object1DAVX.h"
#else
    #include "Object1D.h"
#endif

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

    // Listen to objects returning a message
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    // Buttons and sliders at the bottom of the application
    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    
    // Create physical model function
    bool createPhysicalModel();
    
    // Add coefficient (arguments are for one-click model generation)
    void addCoefficient (bool automatic = false, String name = "", double val = 0.0, bool isDynamic = false);
    
    // Graphics timeer callback
    void timerCallback() override;
    
    // Output clipping
    double clip (double output, double min = -1.0, double max = 1.0);
    
    // Key listening functions
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged (bool isKeyDown, Component* originatingComponent) override;
    
    // Change application state (normalAppState / newObjectState / editObjectState)
    void changeAppState (ApplicationState applicationState);
    
    // Create a physical model with the click of a button
    void create (String model);
    
private:
    //==============================================================================
    
    // The calculator including equation textbox
    ScopedPointer<Calculator> calculator;
    
    // The FDS Solver (collection of functions)
    ScopedPointer<FDSsolver> fdsSolver = nullptr;
    
    OwnedArray<TextButton> modelButtons;
    OwnedArray<Slider> coeffSliders;

    ScopedPointer<AddCoefficient> addCoeffWindow;
    
    // List of coefficients containing coefficientcomponents
    CoefficientList coefficientList;

    OwnedArray<Label> coeffLabels;
    std::vector<bool> coeffDynamic;

    NamedValueSet coefficients;
    
    OwnedArray<Object> objects;
    
    ApplicationState appState;
    CoeffPopupState coeffPopupState = normalCoeffState;
    
    Object* editingObject = nullptr;
    Object* currentlySelectedObject = nullptr;
    bool repaintFlag = false;
    
    ScopedPointer<TextButton> newButton;
    ScopedPointer<TextButton> muteButton;
    Label cpuUsage;
    Label graphicsLabel;
    Slider graphicsSlider;
    
    bool mute = false;
    
    std::vector<__m256d> testVec {100};
    
    int numObject = 0;
    
    // Audio variables
    double fs;
    int bufferSize;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
