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
/*
    The FDSsolver class can be seen as a function base used by the MainComponent to solve whatever equation its been given.
 
 Glossary:
    - An "equation" is of type String and consists of numbers (206), letters + dashes (T--) and underscores (_). Every token (see below) has four characters: three numbers or three letters + dashes and an underscore. Letters + dashes represent coefficients, of which the name is a maximum 3-character word (the rest is filled in with dashes to keep the length per token at 4) and numbers represent the rest. Underscores help divide the equation into tokens.
    - "Tokens" are parts of the equation and can be a mathematical operator, difference operator, state variable u or a coefficient (see GUIDefines.h for more info)
    - "Terms" are tokens grouped together in terms (mathematically) separated by operators (for example, in d_{tt}u = Td_{xx}u, d_{tt}u and Td_{xx}u are two separate terms). Their type is Equation and thus have a stencil.
 */

class FDSsolver
{
public:
    FDSsolver (CoefficientList* coefficientList, double k);
    ~FDSsolver();
    
    // Main function for solving the equation and returning the stencil and the equation terms
    bool solve (String& equationString, Equation& eq, NamedValueSet* coefficients, Array<var>& coefficientTermIndex, std::vector<Equation>& terms);
    
    // Checking whether the syntax of the equation is correct
    bool checkSyntax (StringArray& tokens);
    
    // Stability analysis
    double calculateGridSpacing (StringArray& tokens, NamedValueSet* coefficients, Equation& eq, int numTerms);
    
    // Function to quickly retrieve the names of the coefficients in an equation
    StringArray getUsedCoeffs (String& equationString);
    
    // Spatial difference operators
    Equation& forwDiffX (Equation& equation);
    Equation& backDiffX (Equation& equation);
    Equation& centDiffX (Equation& equation);
    
    Equation& secondOrderX (Equation& equation) { forwDiffX (equation); backDiffX(equation); return equation; }
    Equation& fourthOrderX (Equation& equation) { secondOrderX (equation); secondOrderX(equation); return equation;  }
    
    // 2D
    Equation& forwDiffY (Equation& equation);
    Equation& backDiffY (Equation& equation);
    Equation& centDiffY (Equation& equation);
    
    Equation& secondOrderY (Equation& equation) { forwDiffY (equation); backDiffY (equation); return equation; };
    Equation& secondOrder2D (Equation& equation) {
        Equation tmpEq = equation;
        secondOrderX (equation);
        secondOrderY (tmpEq);
        equation = equation + tmpEq;
        return equation;
    };
    
    // Temporal difference operators
    Equation& forwDiffT (Equation& equation);
    Equation& backDiffT (Equation& equation);
    Equation& centDiffT (Equation& equation);
    
    Equation& secondOrderT (Equation& equation) { forwDiffT (equation); backDiffT (equation); return equation; }
    
    // Function to retrieve the stencilwidth from an equation string (if checkSpatial is true, function returns the number of spatial gridpoints of the stencil, otherwise it returns the number of time steps)
    int getStencilWidthFromEqString (String& equationString, bool checkSpatial);
    
    bool checkIfCoefficientExists (String& coeff);
    int checkDimension (String equationString);
    
    void checkNotAllowedCharacters (int firstInt, std::vector<int>& notAllowedCharacters);
    
private:
    
    // pointer to list of coefficients to see whether they still exist or have been deleted
    CoefficientList* coefficientList;
    
    /*
        This array holds the names of the coefficients multiplied onto terms. The indices in this array refer to what term these are multiplied onto (coefficientTermIndex[1] is multiplied onto terms[1]). The values stored at every index are arrays as well, as every term can contain multiple coefficients (coefficientTermIndex[1][0] = -1, coefficientTermIndex[1][1] = "K")
     */
    Array<var> coefficientTermIndex;
    
    // This vector holds the resulting value of the coefficients after being multiplied together (-1.0 * K = -20.0)
    std::vector<double> coeffsPerTerm;
    
    // Grid spacing
    double h;
    
    // Time step
    double k;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDSsolver)
};
