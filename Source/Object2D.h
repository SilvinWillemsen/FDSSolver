#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Object.h"
//==============================================================================
/*
 */
class Object2D    : public Object
{
public:
    Object2D (String equationString, Equation stencil, std::vector<Equation> terms);
    ~Object2D();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void createUpdateEq() override;
    void calculateFDS() override;
    double getOutput (double ratio) override { int idx = floor (N * ratio); return u[1][idx]; };
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object2D)
};
