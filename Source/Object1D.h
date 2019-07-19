/*
  ==============================================================================

    Object1D.h
    Created: 15 Jul 2019 9:52:47am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Equation.h"
//==============================================================================
/*
*/

enum BoundaryCondition
{
    clamped,
    simplySupported,
    freeBC,
};

class Object1D    : public Component,
                    public Button::Listener,
                    public ChangeBroadcaster
{
public:
    Object1D (String equationString, Equation stencil, double h);
    ~Object1D();

    void paint (Graphics&) override;
    void resized() override;
    
    String getEquationString() { return equationString; };
    
    Path visualiseState();
    void calculateFDS();
    
    void updateStates();
    
    void excite();
    double getOutput (double ratio) { int idx = floor (N * ratio); return u[1][idx]; };
    void mouseDown (const MouseEvent& e) override;
    
    void setCoefficient (String name, double value) { coefficients.set(name, value); };
    void setCoefficients (NamedValueSet* coeffs) { for (int i = 0; i < coeffs->size(); ++i) coefficients.set (coeffs->getName (i), coeffs->getValueAt (i)); };
    NamedValueSet* getCoefficientPtr() { return &coefficients; };
    
    void setCoefficientTermIndex (Array<var>& varray) { coefficientTermIndex = varray; };
    void refreshCoefficients();
    
    void buttonClicked (Button* button) override;
    
    Action getAction() { return action; };
    
    void setZero() { for (int i = 0; i < uVecs.size(); ++i) for (int j = 0; j < uVecs[i].size(); ++j) uVecs[i][j] = 0; setZFlag = false; }
    void setZeroFlag() { setZFlag = true; }
    
    bool needsToBeZero() { return setZFlag; };
private:
    String equationString;
    
    // pointers to the different states
    std::vector<double*> u;
    
    Equation stencil;
    int numTimeSteps;
    int N;
    double h;
    
    int uNextPtrIdx = 0;
    std::vector<std::vector<double>> uVecs;
    
    int stencilIdxStart = 0;
    
    BoundaryCondition leftBoundary = clamped;
    BoundaryCondition rightBoundary = clamped;
    
    bool excited = false;
    
    NamedValueSet coefficients;
    Array<var> coefficientTermIndex;
    
    TextButton removeButton;
    TextButton editButton;
    
    Action action = noAction;
    
    bool setZFlag = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object1D)
};
