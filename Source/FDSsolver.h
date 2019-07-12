/*
  ==============================================================================

    FDSsolver.h
    Created: 11 Jul 2019 10:16:14am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
//template <int N>

class StringCode
{
    
public:
    const StringArray encoded = {
        "100",
        "101",
        "102",
        "200",
        "201",
        "202",
        "203",
        "204",
        "205",
        "301"
    };
    
    const StringArray decoded = {
        " = ",
        " + ",
        " - ",
        String(CharPointer_UTF8 ("\xce\xb4")) + "t+",
        String(CharPointer_UTF8 ("\xce\xb4")) + "t-",
        String(CharPointer_UTF8 ("\xce\xb4")) + "tt",
        String(CharPointer_UTF8 ("\xce\xb4")) + "x+",
        String(CharPointer_UTF8 ("\xce\xb4")) + "x-",
        String(CharPointer_UTF8 ("\xce\xb4")) + "xx",
        "u"
    };
    
    
    
    int getStringArraySize() {
        if (encoded.size() != decoded.size())
        {
            std::cout << "StringArrays are not the same size." << std::endl;
            return 0;
        } else {
            return encoded.size();
        }
        
    };
};


class Equation
{
    
public:
    Equation (int max = 0, bool createULN = false);
    
//    Equation operator= (Equation& eq){
//        
//        if (!checkOperation(eq))
//            return *this;
//        
//        for(int i = 0; i < uCoeffs.size(); ++i)
//        {
//            uNextCoeffs[i] = eq.getUNextCoeffs()[i];
//            uCoeffs[i] = eq.getUCoeffs()[i];
//            uPrevCoeffs[i] = eq.getUPrevCoeffs()[i];
//        }
//    }
    
    Equation operator* (double val) {
        
        for(int i = 0; i < uCoeffs.size(); ++i)
        {
            uNextCoeffs[i] *= val;
            uCoeffs[i] *= val;
            uPrevCoeffs[i] *= val;
        }
        return *this;
    };
    
    Equation operator+ (Equation& eq) {
        
        if (!checkOperation(eq))
            return *this;
        
        for(int i = 0; i < uCoeffs.size(); ++i)
        {
            uNextCoeffs[i] += eq.getUNextCoeffs()[i];
            uCoeffs[i] += eq.getUCoeffs()[i];
            uPrevCoeffs[i] += eq.getUPrevCoeffs()[i];
        }
        return *this;
    };
    
    Equation operator- (Equation& eq) {
        
        if (!checkOperation(eq))
            return *this;
        
        for(int i = 0; i < uCoeffs.size(); ++i)
        {
            uNextCoeffs[i] -= eq.getUNextCoeffs()[i];
            uCoeffs[i] -= eq.getUCoeffs()[i];
            uPrevCoeffs[i] -= eq.getUPrevCoeffs()[i];
        }
        return *this;
    };
    
    bool check(int idx);
    
    bool checkOperation (Equation& eq);
    
    int getCoeffsSize() { return static_cast<int> (uCoeffs.size()); };
    
    double* getUNextCoeffs() { return &uNextCoeffs[0]; };
    double* getUCoeffs() { return &uCoeffs[0]; };
    double* getUPrevCoeffs() { return &uPrevCoeffs[0]; };
    
private:
    std::vector<double> uNextCoeffs;
    std::vector<double> uCoeffs;
    std::vector<double> uPrevCoeffs;
};

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
    
    void solve (String& equation);
    
    void decodeStringToEquation (String equation);
    
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
    
    int getEquation();
    
    void applyOperator (Equation* equation, void(*)(Equation*));
    
private:
    StringCode* stringCode;
    Equation eq;
    
    int numTerms = 0;
    
    double h;
    double k;
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDSsolver)
};
