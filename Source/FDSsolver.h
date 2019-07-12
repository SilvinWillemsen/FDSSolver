/*
  ==============================================================================

    FDSsolver.h
    Created: 11 Jul 2019 10:16:14am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Equation.h"

//==============================================================================

class FDSsolver    : public Component
{
public:
    FDSsolver (StringCode* stringCode, double k);
    ~FDSsolver();

    void paint (Graphics&) override;
    void resized() override;

    void setGridSpacing (double val) { h = val; };
    
    bool checkEquation (String& equation);
    bool checkSyntax (StringArray& tokens);
    
    void solve (String& equationString, Equation* eq);
    
    // Spatial differences
    Equation* forwDiffX (Equation* equation);
    Equation* backDiffX (Equation* equation);
    Equation* centDiffX (Equation* equation);
    
    Equation* secondOrderX (Equation* equation) { forwDiffX(equation); backDiffX(equation); return equation; }
    Equation* fourthOrderX (Equation* equation) { secondOrderX(equation); secondOrderX(equation); return equation;  }
    
    Equation* forwDiffT (Equation* equation);
    Equation* backDiffT (Equation* equation);
    Equation* centDiffT (Equation* equation);
    
    Equation* secondOrderT (Equation* equation) { forwDiffT(equation); backDiffT(equation); return equation; }
    
    void applyOperator (Equation* equation, void(*)(Equation*));
    
private:
    StringCode* stringCode;
    Equation eq;
    
    int numTerms = 0;
    
    double h;
    double k;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDSsolver)
};
