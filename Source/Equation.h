/*
  ==============================================================================

    Equation.h
    Created: 12 Jul 2019 2:17:09pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

// Class that contains the stencil and upon which can be performed operations

class Equation
{
    
public:
    Equation (int amountOfTimeSteps, int amountOfGridPoints, bool createULN = false);
    
    // OPERATORS //
    
    friend Equation operator* (Equation eq, const double& val) {
        for (int i = 0; i < eq.getTimeSteps(); ++i)
            for (int j = 0; j < eq.getStencilWidth(); ++j )
                eq.getUCoeffs(i)[j] *= val;
        return eq;
    };
    
    friend Equation operator/ (Equation eq, const double& val) {
        
        for (int i = 0; i < eq.getTimeSteps(); ++i)
            for (int j = 0; j < eq.getStencilWidth(); ++j )
                eq.getUCoeffs(i)[j] /= val;
        
        return eq;
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
    void setNumPointsFromGridSpacing (double hToSet) { h = hToSet; N = floor(1.0 / hToSet); };
    
    double getGridSpacing() { return h; };
    
    void incPowerOfhDivision() { ++powerOfhDivision; };
    
    bool isSymmetric();
    
private:
    
    std::vector<std::vector<double>> uCoeffs;
    
    int N = 0;
    double h = 1;
    
    double powerOfhDivision = 0;
};

