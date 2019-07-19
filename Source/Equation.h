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

// Class that contains the stencil and upon which can be performed operations

class Equation
{
    
public:
    Equation (int amountOfTimeSteps, int amountOfGridPoints, bool createULN = false);
    
    // OPERATORS //
    
    Equation operator* (double val) {
        
        for (int i = 0; i < uCoeffs.size(); ++i)
            for (int j = 0; j < uCoeffs[i].size(); ++j )
                uCoeffs[i][j] *= val;
        return *this;
    };
    
    Equation operator/ (double val) {
        
        for (int i = 0; i < uCoeffs.size(); ++i)
            for (int j = 0; j < uCoeffs[i].size(); ++j )
                uCoeffs[i][j] /= val;
        
        return *this;
    };
    
    Equation operator+ (Equation& eq) {
        
        if (!checkOperation(eq))
            return *this;
        
        for (int i = 0; i < uCoeffs.size(); ++i)
            for (int j = 0; j < uCoeffs[i].size(); ++j )
                uCoeffs[i][j] += eq.getUCoeffs(i)[j];
        
        return *this;
    };
    
    Equation operator- (Equation& eq) {
        
        if (!checkOperation(eq))
            return *this;
        
        for (int i = 0; i < uCoeffs.size(); ++i)
            for (int j = 0; j < uCoeffs[i].size(); ++j)
                uCoeffs[i][j] -= eq.getUCoeffs(i)[j];
        return *this;
    };
    
    double* operator[] (int idx)
    {
        return getUCoeffs (idx);
    };
    
    
    bool check(int idx);
    
    bool checkOperation (Equation& eq);
    
    double* getUCoeffs (int timeIdx) { return &uCoeffs[timeIdx][0]; };
    
    double getUCoeffAt (int timeIdx, int spaceIdx) { return uCoeffs[timeIdx][spaceIdx]; }
    
    void showStencil();
    
    int getStencilWidth() { return static_cast<int> (uCoeffs[0].size()); };
    int getTimeSteps() { return static_cast<int> (uCoeffs.size()); };
    int getNumPoints() { return N; };
    void setNumPointsFromGridSpacing (double h) { N = floor(1.0/h); };
    
private:
    
    std::vector<std::vector<double>> uCoeffs;
    
    int N = 0;
};

