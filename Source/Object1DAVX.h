/*
  ==============================================================================

    Object1DAVX.h
    Created: 25 Jul 2019 2:30:44pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Object.h"

//==============================================================================
/*
*/
class Object1DAVX    : public Object
{
public:
    Object1DAVX (String equationString, Equation stencil, std::vector<Equation> terms, std::vector<BoundaryCondition> boundaries);
    ~Object1DAVX();

    void paint (Graphics& g) override;
    void resized() override;
    
    void calculateFDS() override;
    double getOutput (double ratio) override;
    void updateStates() override;
    
    void excite() override;
    void setZero() override;
    
    Path visualiseState();
    void changeBoundaryCondition();
    
private:
    // pointers to the different states
    std::vector<__m256d*> u;
    const __m256d zeroval = _mm256_set_pd (0.0, 0.0, 0.0, 0.0);
    
    // states
    std::vector<std::vector<__m256d>> uVecs;
    
    std::vector<std::vector<__m256d>> uVecsPlus1;
    std::vector<std::vector<__m256d>> uVecsMin1;
    
    int numAVX = 0; // amount of avx indices
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object1DAVX)
};
