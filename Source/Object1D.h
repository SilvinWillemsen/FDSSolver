/*
  ==============================================================================

    Object1D.h
    Created: 24 Jul 2019 3:48:59pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Object1D.h"
#include "Object.h"
//==============================================================================
/*
*/
class Object1D    : public Object
{
public:
    Object1D (String equationString, Equation stencil, std::vector<Equation> terms, std::vector<BoundaryCondition> bounds);
    ~Object1D();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void calculateFDS() override;
    double getOutput (double ratio) override { int idx = floor (N * ratio); return u[1][idx]; };
    void updateStates() override;

    void excite() override;
    void setZero() override;
    
    Path visualiseState();
    void changeBoundaryCondition();
    
private:
    // pointers to the different states
    std::vector<double*> u;
    
    // states
    std::vector<std::vector<double>> uVecs;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object1D)
};
