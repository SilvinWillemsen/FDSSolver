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

    void paint (Graphics&) override;
    void resized() override;
    
    String getEquationString() { return equationString; };
    
    Path visualiseState();
    void calculateFDS();
    
    void updateStates();
    
    void excite();
    double getOutput (double ratio) { int idx = floor (N * ratio); return u[1][idx]; };
    void mouseDown (const MouseEvent& e) override;
    void mouseMove (const MouseEvent& e) override;
    
    void timerCallback() override;
    
    void setCoefficient (String name, double value) { coefficients.set(name, value); refreshCoefficientsFlag = true; };
    
    void setCoefficients (NamedValueSet& coeffs) {
        for (int i = 0; i < coeffs.size(); ++i)
        {
            String name = coeffs.getName (i).toString();
            var val = coeffs.getValueAt (i);
//            coefficients.set (coeffs.getName (i), coeffs.getValueAt (i));
            coefficients.set (name, val);
        }
    };
    
    NamedValueSet* getCoefficientPtr() { return &coefficients; };
    
    void setCoefficientTermIndex (Array<var>& varray) { coefficientTermIndex = varray; };
    void refreshCoefficients();
    
    void buttonClicked (Button* button) override;
    
    Action getAction() { return action; };
    
    void setZero();
    
    void setZeroFlag() { setZFlag = true; }
    
    bool needsToBeZero() { return setZFlag; };
    
    void setCoefficientComponent (std::shared_ptr<CoefficientComponent> coeffComp) { coefficientComponents.push_back (coeffComp); };
    
    std::vector<std::shared_ptr<CoefficientComponent>>& getCoefficientComponents() { return coefficientComponents; };
    
    void setApplicationState (ApplicationState applicationState);
    
    bool needsCoefficientsRefreshed() { return refreshCoefficientsFlag; };
    
    // boundaries
    BoundaryCondition getBoundary (bool left) { return left ? leftBoundary : rightBoundary; }
    void changeBoundaryCondition();
    bool hasBoundaryChanged() { return BCChangeFlag; };
    
    
    
private:
    String equationString;
    
    std::vector<Equation> terms;
    
    // pointers to the different states
    std::vector<double*> u;
    
    Equation stencil;
    int numTimeSteps;
    double h;
    int N;
    
    int uNextPtrIdx = 0;
    
    std::vector<std::vector<double>> uVecs;
    
    int stencilIdxStart = 0;
    
    BoundaryCondition leftBoundary = clamped;
    BoundaryCondition rightBoundary = clamped;
    
    bool excited = false;
    
    NamedValueSet coefficients;
    Array<var> coefficientTermIndex;
    
    OwnedArray<TextButton> buttons;
    
    TextButton* removeButton;
    TextButton* editButton;
    TextButton* muteButton;
    
    OwnedArray<TextButton> boundaryButtons;
    
    TextButton* leftBoundButton;
    TextButton* rightBoundButton;

    bool showButtons = false;
    
    Action action = noAction;
    
    bool setZFlag = false;
    
    std::vector<std::shared_ptr<CoefficientComponent>> coefficientComponents;
    
    ApplicationState appState;
    int prevMouseX = 0;
    int prevMouseY = 0;
    
    bool refreshCoefficientsFlag = false;
    
    bool BCChangeFlag = false;
    bool left = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object)
};
