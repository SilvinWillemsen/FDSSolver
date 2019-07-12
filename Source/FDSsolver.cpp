/*
  ==============================================================================

    FDSsolver.cpp
    Created: 11 Jul 2019 10:16:14am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FDSsolver.h"

//==============================================================================

FDSsolver::FDSsolver(StringCode* stringCode, double k) : stringCode (stringCode), k (k)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

FDSsolver::~FDSsolver()
{
}

void FDSsolver::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("FDSsolver", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void FDSsolver::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

bool FDSsolver::checkEquation(String& equationString)
{
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1);
    
    if (!checkSyntax (tokens))
    {
        return false;
    }

    return true;
}

void FDSsolver::solve (String& equationString, Equation* eq)
{
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1);
    
    int numTerms = 0;
    for (int i = 0; i < tokens.size(); ++i)
    {
        String firstChar = tokens[i].substring(0, 1);
        int firstInt = firstChar.getIntValue();
        
        if (firstInt == 3)
        {
            ++numTerms;
        }
    }
    
    int equalsSignIdx = 0;
    bool newTermFlag = false;
    std::vector<Equation> terms;
    terms.reserve(numTerms);
    
    std::vector<Equation> coeffs;
    terms.reserve(numTerms);
    
    terms.push_back(Equation (eq->getCoeffsSize(), true));
    
    int idx = 0;
    Equation* eqPtr = &terms[idx];
    
    for (int i = 0; i < tokens.size(); ++i)
    {
        String firstChar = tokens[i].substring(0, 1);
        int firstInt = firstChar.getIntValue();
        int prevOperator = 0;
        
        if (newTermFlag)
        {
            terms.push_back(Equation (7, true));
            ++idx;
            eqPtr = &terms[idx];
            newTermFlag = false;
        }
        switch (firstInt)
        {
            case 1:
                prevOperator = tokens[i].getIntValue();
                break;
            case 2:
                switch(tokens[i].getIntValue())
                {
                    case 200:
                        forwDiffT (eqPtr);
                        break;
                    case 201:
                        backDiffT (eqPtr);
                        break;
                    case 202:
                        secondOrderT (eqPtr);
                        break;
                    case 203:
                        forwDiffX (eqPtr);
                        break;
                    case 204:
                        backDiffX (eqPtr);
                        break;
                    case 205:
                        secondOrderX (eqPtr);
                        break;
                    case 206:
                        fourthOrderX (eqPtr);
                        break;
                }
                break;
            case 3:
                if (tokens[i].getIntValue() == 100)
                {
                    equalsSignIdx = idx + 1;
                }
                newTermFlag = true;
                break;
            case 4:
                coeffs[idx];
        }
    }

    for (int i = 0; i < terms.size(); ++i)
    {
        if (i < equalsSignIdx)
            *eq = (*eq) + terms[i];
        else
            *eq = (*eq) - terms[i];
    }
    int uNextIdx = (eq->getCoeffsSize() - 1) / 2.0;
    *eq = (*eq) / (eq->getUNextCoeffs()[uNextIdx]);
    eq->showStencil();
}

bool FDSsolver::checkSyntax (StringArray& tokens)
{
    int prevTermType = 0;
    bool hasEqualsSign = false;
    for (int i = 0; i < tokens.size(); ++i)
    {
        std::cout << tokens[i] << std::endl;
        if (tokens[i] == "100")
        {
            hasEqualsSign = true;
        }
        String firstChar = tokens[i].substring(0, 1);
        
        int firstInt = firstChar.getIntValue();
        if (i == tokens.size() && firstInt != 3)
        {
            std::cout << "FDS doesn't end in u" << std::endl;
            return false;
        }
        
        std::vector<int> notAllowedCharacters;
        switch (prevTermType)
        {
            case 0:
                notAllowedCharacters.push_back(1);
                break;
            case 1:
                notAllowedCharacters.push_back(1); //except a minus!!
                break;
            case 2:
                notAllowedCharacters.push_back(1);
                break;
            case 3:
                notAllowedCharacters.push_back(2);
                notAllowedCharacters.push_back(3);
                break;
        }
        
        // if the character can't be after the previous one, give an error
        if (std::find(notAllowedCharacters.begin(), notAllowedCharacters.end(), firstChar.getIntValue()) != notAllowedCharacters.end())
        {
            std::cout << "Syntax error" << std::endl;
            return false;
        }
        
        // set the type of the previous character
        prevTermType = firstChar.getIntValue();
    }
    if (!hasEqualsSign)
    {
        std::cout << "No equals sign" << std::endl;
        return false;
    }
    return true;
}

void FDSsolver::applyOperator(Equation *equation, void (*)(Equation *))
{
    
}

// OPERATORS //

Equation* FDSsolver::forwDiffX(Equation* eq)
{
    bool res = eq->check(eq->getCoeffsSize() - 1);
    if (!res)
    {
        return eq;
    }
        
    for (int i = eq->getCoeffsSize() - 2; i >= 0; --i)
    {
        eq->getUNextCoeffs()[i + 1] = eq->getUNextCoeffs()[i] - eq->getUNextCoeffs()[i + 1];
        eq->getUCoeffs()[i + 1] = eq->getUCoeffs()[i] - eq->getUCoeffs()[i + 1];
        eq->getUPrevCoeffs()[i + 1] = eq->getUPrevCoeffs()[i] - eq->getUPrevCoeffs()[i + 1];
    }
    *eq = (*eq) * (1.0/h);
    return eq;
}

Equation* FDSsolver::backDiffX(Equation* eq)
{
    bool res = eq->check(0);
    if (!res)
    {
        return eq;
    }
    
    for (int i = 1; i < eq->getCoeffsSize(); ++i)
    {
        eq->getUNextCoeffs()[i - 1] -= eq->getUNextCoeffs()[i];
        eq->getUCoeffs()[i - 1] -= eq->getUCoeffs()[i];
        eq->getUPrevCoeffs()[i - 1] -= eq->getUPrevCoeffs()[i];
    }
    *eq = (*eq) * (1.0/h);
    
    return eq;
}

Equation* FDSsolver::centDiffX (Equation* eq)
{
    bool res = eq->check(0);
    if (res)
        res = eq->check(eq->getCoeffsSize() - 1);
    
    if (!res)
    {
        return eq;
    }
    
    std::vector<double> tmpUNext (eq->getCoeffsSize(), 0);
    std::vector<double> tmpU (eq->getCoeffsSize(), 0);
    std::vector<double> tmpUPrev (eq->getCoeffsSize(), 0);
    
    for (int i = 0; i < eq->getCoeffsSize(); ++i)
    {
        tmpUNext[i] = eq->getUNextCoeffs()[i];
        tmpU[i] = eq->getUCoeffs()[i];
        tmpUPrev[i] = eq->getUPrevCoeffs()[i];
    }
    
    for (int i = 1; i < eq->getCoeffsSize() - 1; ++i)
    {
        tmpUNext[i] = tmpUNext[i] + eq->getUNextCoeffs()[i+1] - eq->getUNextCoeffs()[i-1] - eq->getUNextCoeffs()[i];
        tmpU[i] = tmpU[i] + eq->getUCoeffs()[i+1] - eq->getUCoeffs()[i-1] - eq->getUCoeffs()[i];
        tmpUPrev[i] = tmpUPrev[i] + eq->getUPrevCoeffs()[i+1] - eq->getUPrevCoeffs()[i-1] - eq->getUPrevCoeffs()[i];
    }
    
    for (int i = 0; i < eq->getCoeffsSize(); ++i)
    {
        eq->getUNextCoeffs()[i] = tmpUNext[i];
        eq->getUCoeffs()[i] = tmpU[i];
        eq->getUPrevCoeffs()[i] = tmpUPrev[i];
    }
    
    *eq = (*eq) * (1.0 / (2*h));
    
    return eq;
}

Equation* FDSsolver::forwDiffT (Equation* eq)
{
    for (int i = 0; i < eq->getCoeffsSize(); ++i)
    {
        eq->getUNextCoeffs()[i] += eq->getUCoeffs()[i];
        eq->getUCoeffs()[i] -= (2*eq->getUCoeffs()[i] - eq->getUPrevCoeffs()[i]);
        eq->getUPrevCoeffs()[i] -= 2*eq->getUPrevCoeffs()[i];
    }
    *eq = (*eq) * (1.0 / k);
    
    return eq;
}

Equation* FDSsolver::backDiffT (Equation* eq)
{
    for (int i = 0; i < eq->getCoeffsSize(); ++i)
    {
        eq->getUPrevCoeffs()[i] -= eq->getUCoeffs()[i];
        eq->getUCoeffs()[i] -= eq->getUNextCoeffs()[i];
    }
    *eq = (*eq) * (1.0 / k);
    
    return eq;
}

Equation* FDSsolver::centDiffT (Equation* eq)
{
    for (int i = 0; i < eq->getCoeffsSize(); ++i)
    {
        eq->getUNextCoeffs()[i] += eq->getUCoeffs()[i];
        eq->getUPrevCoeffs()[i] -= eq->getUCoeffs()[i];
        eq->getUCoeffs()[i] -= eq->getUCoeffs()[i];
    }
    *eq = (*eq) * (1.0 / (2*k));
    
    return eq;
}
