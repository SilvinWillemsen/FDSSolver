/*
  ==============================================================================

    FDSsolver.h
    Created: 11 Jul 2019 10:16:14am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Equation.h"
#include "CoefficientList.h"

//==============================================================================

class FDSsolver    : public Component
{
public:
    FDSsolver (CoefficientList* coefficientList, double k);
    ~FDSsolver();

    void paint (Graphics&) override;
    void resized() override;

    void setGridSpacing (double val) { h = val; };
    
    bool checkEquation (String& equation);
    bool checkSyntax (StringArray& tokens);
    
    // Main function for solving the equation and returning the stencil and the terms
    bool solve (String& equationString, Equation& eq, NamedValueSet* coefficients, Array<var>& coefficientTermIndex, std::vector<Equation>& terms);
   
    // Stability analysis
    double calculateGridSpacing (StringArray& tokens, NamedValueSet* coefficients, Equation& eq, int numTerms);
    
    StringArray getUsedCoeffs (String& equationString);
    
    // Spatial difference operators
    Equation& forwDiffX (Equation& equation);
    Equation& backDiffX (Equation& equation);
    Equation& centDiffX (Equation& equation);
    
    Equation& secondOrderX (Equation& equation) { forwDiffX (equation); backDiffX(equation); return equation; }
    Equation& fourthOrderX (Equation& equation) { secondOrderX (equation); secondOrderX(equation); return equation;  }
    
    // Temporal difference operators
    Equation& forwDiffT (Equation& equation);
    Equation& backDiffT (Equation& equation);
    Equation& centDiffT (Equation& equation);
    
    Equation& secondOrderT (Equation& equation) { forwDiffT (equation); backDiffT (equation); return equation; }
    
    bool checkAllowedCharacters (int prevTermType, StringArray& tokens, bool& hasEqualsSign);
    
    int getStencilWidth (String& equationString, bool checkSpace);
    
    Array<var>& getCoeffTermIndex() { return coefficientTermIndex; };
    
    bool checkIfCoefficientExists (String& coeff);
private:
    
    // pointer to list of coefficients to see whether they still exist or have been deleted
    CoefficientList* coefficientList;
    
    int numTerms = 0;
    std::vector<double> coeffsPerTerm;
    NamedValueSet coefficients;
    
    // every index in the vector is a term to which coefficients (stored in the StringArray) are applied to
    Array<var> coefficientTermIndex;
    
    double h;
    double k;
    
    int stencilSize;
    
    bool stabilityFlag = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDSsolver)
};
