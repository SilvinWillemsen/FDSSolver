/*
  ==============================================================================

    Object1DAVX.h
    Created: 25 Jul 2019 2:30:44pm
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
class Object1DAVX    : public Object
{
public:
    Object1DAVX (String equationString, Equation stencil, std::vector<Equation> terms, std::vector<__m256d> testVec);
    ~Object1DAVX();

    void paint (Graphics& g) override;
    void resized() override;
    
    void calculateFDS() override;
    double getOutput (double ratio, double unUsed) override;
    void updateStates() override;
    
    void excite() override;
    void setZero() override;
    
    Path visualiseState();
    void changeBoundaryCondition() override;
    
private:
    // pointers to the different states
    std::vector<__m256d*> u;
    
    const __m256d zeroval = _mm256_set_pd (0.0, 0.0, 0.0, 0.0);
    
    // states
    std::vector<std::vector<double>> uVecs;
    
    int numAVX = 0; // amount of avx indices
    
    std::vector<std::vector<__m256d>> uVecsAVX; // [temporal index][AVX index]
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object1DAVX)
};
