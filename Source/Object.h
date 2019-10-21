/*
  ==============================================================================

    Object.h
    Created: 15 Jul 2019 9:52:47am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUIDefines.h"
#include "Equation.h"
#include "CoefficientComponent.h"
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

//==============================================================================
/*
 Base class for all objects. Derived classes are currently:
 Object1D,
 Object1DAVX
*/


enum BoundaryCondition
{
    clamped,
    simplySupported,
    freeBC,
};

class Object    : public Component,
                    public Button::Listener,
                    public ChangeBroadcaster,
                    public Timer
{
public:
    Object (String equationString, Equation stencil, std::vector<Equation> terms, int numDim);
    ~Object();
    
    void buttonClicked (Button* button) override;
    
    // Mouse-event linked to exciting the PM
    void mouseDown (const MouseEvent& e) override;
    
    // Functions linked to showing and hiding buttons
    void mouseMove (const MouseEvent& e) override;
    void timerCallback() override;
    
    //// Functions that need to be implemented (overridden) by the derived classes ////
    
    virtual void createUpdateEq(){}; // = 0
    virtual void calculateFDS() = 0;
    
    virtual double getOutput (double ratioX, double ratioY = 0) = 0;
    virtual void updateStates() = 0;
    
    virtual void excite() {};
    virtual void setZero() {};
    
    //// Functions valid for all objects ////
    
    // Return the equation
    String getEquationString() { return equationString; };
    
    // Set one coefficient
    void setCoefficient (String name, double value) { coefficients.set(name, value); refreshCoefficientsFlag = true; };
    
    // Set a group of coefficients
    void setCoefficients (NamedValueSet& coeffs) { for (int i = 0; i < coeffs.size(); ++i) coefficients.set (coeffs.getName (i).toString(), coeffs.getValueAt (i)); };
    
    void setCoefficientTermIndex (Array<var>& varray) { coefficientTermIndex = varray; };
    void refreshCoefficients();
    
    // Is needed to tell the main component the reason for the callback
    Action getAction() { return action; };
    
    // Functions and flags checked at audio rate (OUTSIDE of the bufferloop)
    void setZeroFlag() { setZFlag = true; }
    bool needsToBeZero() { return setZFlag; };
    bool needsCoefficientsRefreshed() { return refreshCoefficientsFlag; };
    bool hasBoundaryChanged() { return BCChangeFlag; };
    bool setRepaintFlag() { return paintFlag = true; };
    bool needsToBeRepainted() { return paintFlag; };
    
    void setCoefficientComponent (std::shared_ptr<CoefficientComponent> coeffComp) { coefficientComponents.push_back (coeffComp); };
    std::vector<std::shared_ptr<CoefficientComponent>>& getCoefficientComponents() { return coefficientComponents; };
    
    void setApplicationState (ApplicationState applicationState);
    
    virtual void changeBoundaryCondition() {};
    std::vector<BoundaryCondition>& getBoundaries() { return boundaryConditions; }
    
    int getXLoc() { return xLoc; };
    int getYLoc() { return yLoc; };
    
    double clamp (double val, double min, double max)
    {
        if (val < min)
        {
            val = min;
            return val;
        }
        else if (val > max)
        {
            val = max;
            return val;
        }
        return val;
    }
    
#ifdef CREATECCODE
    const char* toConstChar (String string) { return static_cast<const char*> (string.toUTF8()); }
    unsigned long getCurName() { return curName; };
    void setCurName (unsigned long cN) { curName = cN; };
    
    void removeFiles (unsigned long cN)
    {
        String systemInstr = String ("rm " + String (curName) + ".so \n rm -R " + String (curName) + ".so.dSYM").toUTF8();
        system (toConstChar (systemInstr));
    }
#endif
    
    int getDimension() { return dim; };
    
protected:
    
    // Store different parts of the equation separately in terms. This includes (fx.) the 1/k^2 and 1/h^2 of the \delta_{tt} and \delta_{xx} operators, but not the coefficients as these can be dyamic
    std::vector<Equation> terms;
    
    // An array to store what coefficients are multiplied with what term. Indices correspond to the indices in the 'terms' variable
    Array<var> coefficientTermIndex;
    
    // Contains the added terms with their respective coefficients multiplied onto them for quick and easy use in the calculateFDS() function
    Equation stencil;
    bool isSymmetric;
    std::vector<double> stencilVectorForm;
    
    // Vector for easier access of stencilstuff
    std::vector<double> curTimeStep;
    std::vector<double> prevTimeStep;
    
    // Boundaries
    std::vector<BoundaryCondition> boundaryConditions;
    
    // Equation
    String equationString;
    
    // Grid spacing and number of points
    double h;
    int N;
    
    int uNextPtrIdx = 0;
    int uPtrIdx = 1;
    int uPrevPtrIdx = 2;
    
    int stencilIdxStart = 0;
    
    bool excited = false;
    
    NamedValueSet coefficients;
    
    //// GUI ////
    
    // Mouse
    int prevMouseX = 0;
    int prevMouseY = 0;
    
    // Buttons
    OwnedArray<TextButton> buttons;
    OwnedArray<TextButton> boundaryButtons;

    bool showButtons = false;
    
    // Is needed to tell the main component the reason for the callback
    Action action = noAction;
    
    // Flags checked at audio rate (OUTSIDE of the bufferloop)
    bool setZFlag = false;
    bool refreshCoefficientsFlag = false;
    bool BCChangeFlag = false;
    bool paintFlag = false;
    
    // Pointers to the coefficient components (shown in the coefficientlist)
    std::vector<std::shared_ptr<CoefficientComponent>> coefficientComponents;
    
    ApplicationState appState;
    
    int xLoc = 0;
    int yLoc = 0;
    
    int boundaryChangeIdx = -1; // for 1D object left = 0, right = 1.

#ifdef CREATECCODE
    void updateEqGenerator (String& eqString);
    void (*updateEq) (double* uNext, double* u, double* uPrev, double* parameters, int Nx);
    unsigned long curName = 0;
#endif
    int dim;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Object)
};
