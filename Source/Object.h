/*
  ==============================================================================

    Object.h
    Created: 15 Jul 2019 9:52:47am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Equation.h"
#include "CoefficientComponent.h"
#include <immintrin.h>
//==============================================================================
/*
 Base class for all objects. Derived classes are currently:
 Object1D,
 Object1DAVX
*/


enum BoundaryCondition
{
    clamped,
    simplySupported,
    freeBC,
};

class Object    : public Component,
                    public Button::Listener,
                    public ChangeBroadcaster,
                    public Timer
{
public:
    Object (String equationString, Equation stencil, std::vector<Equation> terms, std::vector<BoundaryCondition> boundaries);
    ~Object();
    
    void buttonClicked (Button* button) override;
    
    // Mouse-event linked to exciting the PM
    void mouseDown (const MouseEvent& e) override;
    
    // Functions linked to showing and hiding buttons
    void mouseMove (const MouseEvent& e) override;
    void timerCallback() override;
    
    //// Functions that need to be implemented (overridden) by the derived classes ////
    
    virtual void calculateFDS() = 0;
    virtual double getOutput (double ratio) = 0;
    virtual void updateStates() = 0;
    
    virtual void excite() {};
    virtual void setZero() {};
    
    //// Functions valid for all objects ////
    
    // Return the equation
    String getEquationString() { return equationString; };
    
    // Set one coefficient
    void setCoefficient (String name, double value) { coefficients.set(name, value); refreshCoefficientsFlag = true; };
    
    // Set a group of coefficients
    void setCoefficients (NamedValueSet& coeffs) { for (int i = 0; i < coeffs.size(); ++i) coefficients.set (coeffs.getName (i).toString(), coeffs.getValueAt (i)); };
    
    void setCoefficientTermIndex (Array<var>& varray) { coefficientTermIndex = varray; };
    void refreshCoefficients();
    
    // Is needed to tell the main component the reason for the callback
    Action getAction() { return action; };
    
    // Functions and flags checked at audio rate (OUTSIDE of the bufferloop)
    void setZeroFlag() { setZFlag = true; }
    bool needsToBeZero() { return setZFlag; };
    bool needsCoefficientsRefreshed() { return refreshCoefficientsFlag; };
    
    void setCoefficientComponent (std::shared_ptr<CoefficientComponent> coeffComp) { coefficientComponents.push_back (coeffComp); };
    std::vector<std::shared_ptr<CoefficientComponent>>& getCoefficientComponents() { return coefficientComponents; };
    
    void setApplicationState (ApplicationState applicationState);
    
    // Boundaries
    BoundaryCondition getBoundary (int idx) { return boundaryConditions[idx]; }
    bool hasBoundaryChanged() { return BCChangeFlag; };
    
protected:
    std::vector<Equation> terms;
    std::vector<BoundaryCondition> boundaryConditions;

    String equationString;
    
    Equation stencil;
    int numTimeSteps;
    double h;
    int N;
    
    int uNextPtrIdx = 0;
    
    int stencilIdxStart = 0;
    
    bool excited = false;
    
    NamedValueSet coefficients;
    Array<var> coefficientTermIndex;
    
    OwnedArray<TextButton> buttons;
    
    TextButton* removeButton;
    TextButton* editButton;
    TextButton* muteButton;
    
    OwnedArray<TextButton> boundaryButtons;

    bool showButtons = false;
    
    Action action = noAction;
    
    bool setZFlag = false;
    
    std::vector<std::shared_ptr<CoefficientComponent>> coefficientComponents;
    
    ApplicationState appState;
    int prevMouseX = 0;
    int prevMouseY = 0;
    
    bool refreshCoefficientsFlag = false;
    
    bool BCChangeFlag = false;
    int boundaryChangeIdx = -1; // for 1D object left = 0, right = 1.
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object)
};
