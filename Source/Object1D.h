/*
  ==============================================================================

    Object1D.h
    Created: 24 Jul 2019 3:48:59pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Object.h"
//==============================================================================
/*
*/
class Object1D    : public Object
{
public:
    Object1D (String equationString, Equation stencil, std::vector<Equation> terms);
    ~Object1D();
    
    void paint (Graphics& g) override;
    void resized() override;
    
#ifdef CREATECCODE
    void createUpdateEq() override;
#endif
    
    void calculateFDS() override;
    double getOutput (double ratio, double unUsed) override { int idx = floor (N * ratio); return u[1][idx]; };
    void updateStates() override;

    void excite() override;
    void setZero() override;
    
    Path visualiseState();
    void changeBoundaryCondition() override;

private:
    // pointers to the different states
    std::vector<double*> u;
    
    // states
    std::vector<std::vector<double>> uVecs;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object1D)
};
