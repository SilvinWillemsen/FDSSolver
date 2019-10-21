/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Calculator.h"
#include "FDSsolver.h"
#include "CoefficientList.h"
#include "BottomMenu.h"
#include "AddCoefficient.h"

#include "Object0D.h"
#ifdef AVX_SUPPORTED
    #include "Object1DAVX.h"
#else
    #include "Object1D.h"
#endif

#include "Object2D.h"

//==============================================================================
/*
    The main component of the application. Handles all communication between objects through the changeListenerCallback(...) function.
Variables living here are mostly pointers to things on the heap:
    - Calculator
    - FDS Solver
    - Add-coefficient Window
    - The "menu" at the bottom of the screen
    - List of coefficients containing coefficientcomponents
    - Physical models (should be instrument which then contains physical models)
 
Furthermore, other variables:
    - State of the application
    - State of the add-coefficient pop-up
    - Pointers to the currently selected object and the currently edited object (if applicable)
 
 // Other buttons located at the bottom of the application
 OwnedArray<TextButton> modelButtons;
 OwnedArray<Slider> coeffSliders;
 ScopedPointer<TextButton> newButton;
 ScopedPointer<ToggleButton> muteButton;
 Label cpuUsage;
 Label graphicsLabel;
 Slider graphicsSlider;
 
 bool mute = false;
 
 std::vector<__m256d> testVec {100};
 
 int numObject = 0;
 
 // Audio variables
 double fs;
 int bufferSize;
 */

class MainComponent   : public AudioAppComponent,
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
    
    // Create physical model function
    bool createPhysicalModel();
    
    // Add coefficient (arguments are for one-click model generation)
    void addCoefficient (bool automatic = false, String name = "", double val = 0.0, bool isDynamic = false);
    
    // Graphics timeer callback
    void timerCallback() override;
    
    void changeGraphicsSpeed();
    
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
    std::unique_ptr<Calculator> calculator;
    
    // The FDS Solver (collection of functions)
    std::unique_ptr<FDSsolver> fdsSolver = nullptr;
    
    // Window for adding a coefficient
    std::unique_ptr<AddCoefficient> addCoeffWindow;
    
    // List of all coefficients in the application (even though not all are visible) containing coefficientcomponents
    CoefficientList coefficientList;

    // Window for adding a coefficient
    std::unique_ptr<BottomMenu> bottomMenu;
    
    // Physical models
    OwnedArray<Object> objects;
    
    // State of the application (normalAppState, newObjectState, editObjectState)
    ApplicationState appState;
    
    // State of the add-coefficient pop-up (normalCoeffState, editingCoeff)
    CoeffPopupState coeffPopupState = normalCoeffState;
    
    // Pointers to objects
    Object* editingObject = nullptr;
    Object* currentlySelectedObject = nullptr;
    
    bool mute = false;
    
    std::vector<__m256d> testVec {100};
    
    // Audio variables
    double fs;
    int bufferSize;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
