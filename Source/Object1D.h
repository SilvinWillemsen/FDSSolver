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
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object1D)
};
