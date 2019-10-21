#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Object.h"
//==============================================================================
/*
 */
class Object2D    : public Object
{
public:
    Object2D (String equationString, Equation stencil, std::vector<Equation> terms, double aspectRatio = 1);
    ~Object2D();
    
    void paint (Graphics& g) override;
    void resized() override;
    
#ifdef CREATECCODE
    void createUpdateEq() override;
#endif
    
    void calculateFDS() override;
    double getOutput (double ratioX, double ratioY) override {
#ifdef CREATECCODE
        int idx = floor (Nx * ratioX) + floor (ratioY * Ny * Nx);
        return u[1][idx];
#else
        int idX = floor(Nx * ratioX);
        int idY = floor(Ny * ratioY);
        return u[1][idX][idY];
#endif
        
    };
    void updateStates() override;
    
    void excite() override;
    void setZero() override;
    
//    Path visualiseState();
    void changeBoundaryCondition() override;
    
    void setAspectRatio (double aspRat) { aspectRatio = aspRat; }; //probably to some state-interpolation or refreshing...
    
private:
#ifdef CREATECCODE
    // pointers to the different states
    std::vector<double*> u;
    // states
    std::vector<std::vector<double>> uVecs;
#else
    // pointers to the different states
    std::vector<std::vector<double>*> u;
    // states
    std::vector<std::vector<std::vector<double>>> uVecs;
#endif
    double aspectRatio;
    int Nx;
    int Ny;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object2D)
};
