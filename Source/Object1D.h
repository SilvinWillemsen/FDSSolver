/*
  ==============================================================================

    Object1D.h
    Created: 15 Jul 2019 9:52:47am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/

enum BoundaryCondition
{
    clamped,
    simplySupported,
    freeBC,
};

class Object1D    : public Component
{
public:
    Object1D (std::vector<std::vector<double>> stencil, int N);
    ~Object1D();

    void paint (Graphics&) override;
    void resized() override;
    
    Path visualiseState();
    void calculateFDS();
    
    void updateStates();
    
    void excite();
    double getOutput (double ratio) { int idx = floor (N * ratio); return u[idx]; };
    void mouseDown (const MouseEvent& e) override;
    
    void setCoefficient (String name, double value) { coefficients.set(name, value); };
    NamedValueSet* getCoefficientPtr() { return &coefficients; };
    
    void setCoefficientTermIndex (Array<var>& varray) { coefficientTermIndex = varray; };
    void refreshCoefficients();
    
    
private:
    double* uNext;
    double* u;
    double* uPrev;
    
    std::vector<std::vector<double>> stencil;
    int N;
    int uNextPtrIdx = 0;
    std::vector<std::vector<double>> uVecs;
    
    int stencilIdxStart = 0;
    
    BoundaryCondition leftBoundary = clamped;
    BoundaryCondition rightBoundary = clamped;
    
    bool excited = false;
    
    NamedValueSet coefficients;
    Array<var> coefficientTermIndex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object1D)
};
