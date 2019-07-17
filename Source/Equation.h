/*
  ==============================================================================

    Equation.h
    Created: 12 Jul 2019 2:17:09pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

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
        "206",
        "207",
        "300",
        "901"
    };
    
    const StringArray decoded = {
        " = ",
        " + ",
        " " + String(CharPointer_UTF8 ("\xe2\x80\x93")) + " ",
        String(CharPointer_UTF8 ("\xce\xb4")) + "t+",
        String(CharPointer_UTF8 ("\xce\xb4")) + "t-",
        String(CharPointer_UTF8 ("\xce\xb4")) + "t" + String (CharPointer_UTF8 ("\xc2\xb7")),
        String(CharPointer_UTF8 ("\xce\xb4")) + "tt",
        String(CharPointer_UTF8 ("\xce\xb4")) + "x+",
        String(CharPointer_UTF8 ("\xce\xb4")) + "x-",
        String(CharPointer_UTF8 ("\xce\xb4")) + "x" + String (CharPointer_UTF8 ("\xc2\xb7")),
        String(CharPointer_UTF8 ("\xce\xb4")) + "xx",
        "u",
        "-"
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
    
    Equation operator* (double val) {
        
        for(int i = 0; i < uCoeffs.size(); ++i)
        {
            uNextCoeffs[i] *= val;
            uCoeffs[i] *= val;
            uPrevCoeffs[i] *= val;
        }
        return *this;
    };
    
    Equation operator/ (double val) {
        
        for(int i = 0; i < uCoeffs.size(); ++i)
        {
            uNextCoeffs[i] /= val;
            uCoeffs[i] /= val;
            uPrevCoeffs[i] /= val;
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
    
//    int getCoeffsSize() { return static_cast<int> (uCoeffs.size()); };
    
    double* getUNextCoeffs() { return &uNextCoeffs[0]; };
    double* getUCoeffs() { return &uCoeffs[0]; };
    double* getUPrevCoeffs() { return &uPrevCoeffs[0]; };
    
    void showStencil();
    
    int getStencilWidth() { return static_cast<int> (uCoeffs.size()); };
    
    int getNumPoints() { return N; };
    void setNumPointsFromGridSpacing (double h) { N = floor(1.0/h); };
    
private:
    std::vector<double> uNextCoeffs;
    std::vector<double> uCoeffs;
    std::vector<double> uPrevCoeffs;
    
    int N = 0;
};

