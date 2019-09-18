/*
  ==============================================================================

    Object0D.h
    Created: 16 Sep 2019 1:18:30pm
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
class Object0D    : public Object
{
public:
    Object0D (String equationString, Equation stencil, std::vector<Equation> terms);
    ~Object0D();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void calculateFDS() override;
    double getOutput (double ratio) override { return u[0][0]; };
    void updateStates() override;
    void setZero() override;
    
    void excite() override;
    
private:
    // pointers to the different states
    std::vector<double*> u;
    
    // states
    std::vector<double> uVecs;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object0D)
};
