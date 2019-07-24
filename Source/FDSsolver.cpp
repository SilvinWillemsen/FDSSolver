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

FDSsolver::FDSsolver (CoefficientList* coefficientList, double k) : coefficientList (coefficientList), k (k)
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

bool FDSsolver::solve (String& equationString, Equation& eq, NamedValueSet* coeffValues, Array<var>& coefficientTermIndex, std::vector<Equation>& terms)
{
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1);
    
    // check syntax if not debugging
    if (!GUIDefines::debug)
        if (!checkSyntax (tokens))
            return false;
    
    // calculate number of terms
    int numTerms = 1;
    for (int i = 0; i < tokens.size(); ++i)
    {
        String firstChar = tokens[i].substring(0, 1);
        int firstInt = firstChar.getIntValue();
        
        if (firstInt == 1)
        {
            ++numTerms;
        }
    }
    
    int equalsSignIdx = 0;
    bool newTermFlag = false;
    
    terms.clear();
    terms.reserve(numTerms);
    
    std::vector<double> tmpCoeffsPerTerm (numTerms, 1);
    coeffsPerTerm = tmpCoeffsPerTerm;
    
    // fill the coefficients with 1's
    Array<var> tmpArray;
    tmpArray.add (1);
    
    //clear the coefficientTermIndex and initialise with 1's
    
    for (int i = 0; i < numTerms; ++i)
        coefficientTermIndex.add (tmpArray);
    
    std::vector<int> operatorVect (numTerms - 1, 0);
    
    terms.push_back(Equation (eq.getTimeSteps(), eq.getStencilWidth(), true));
    
    int idx = 0;
    
    // calculate h
    if (GUIDefines::debug)
        h = 1;
    else
        calculateGridSpacing (eq, coeffValues->getValueAt(0));
    
    eq.setNumPointsFromGridSpacing (h);
    h = 1.0 / (static_cast<double> (eq.getNumPoints()));
    for (int i = 0; i < tokens.size(); ++i)
    {
        String firstChar = tokens[i].substring(0, 1);
        
        const char* test = static_cast<const char*> (firstChar.toUTF8());
        int firstInt = std::isdigit(*test) ? firstChar.getIntValue() : 9;
        
        
        if (newTermFlag)
        {
            terms.push_back (Equation (eq.getTimeSteps(), eq.getStencilWidth(), true));
            ++idx;
            newTermFlag = false;
        }
        switch (firstInt)
        {
            case 1:
                operatorVect[idx-1] = tokens[i].substring(2, 3).getIntValue();
                if (tokens[i].getIntValue() == 100)
                {
                    equalsSignIdx = idx;
                }
                break;
            case 2:

                switch(tokens[i].getIntValue())
                {
                    case 200:
                        forwDiffT (terms[idx]);
                        break;
                    case 201:
                        backDiffT (terms[idx]);
                        break;
                    case 202:
                        centDiffT (terms[idx]);
                        break;
                    case 203:
                        secondOrderT (terms[idx]);
                        break;
                    case 204:
                        forwDiffX (terms[idx]);
                        break;
                    case 205:
                        backDiffX (terms[idx]);
                        break;
                    case 206:
                        centDiffX (terms[idx]);
                        break;
                    case 207:
                        secondOrderX (terms[idx]);
                        break;
                    case 208:
                        fourthOrderX (terms[idx]);
                        break;
                }
                break;
            case 3:
                newTermFlag = true;
                break;
            case 9:
                
                if (tokens[i].getIntValue() == 901)
                {
                    coefficientTermIndex[idx][0] = -1;
                    coeffsPerTerm[idx] *= -1.0;
                } else {
                    if (static_cast<int> (coefficientTermIndex[idx][0]) == 1)
                        coefficientTermIndex[idx][0] = tokens[i].upToFirstOccurrenceOf ("-", false, true);
                    else
                        coefficientTermIndex[idx].append (tokens[i].upToFirstOccurrenceOf ("-", false, true));
                    
                    auto valuePtr =  coeffValues->getVarPointer (tokens[i].upToFirstOccurrenceOf ("-", false, true));
                    if (valuePtr == NULL)
                    {
                        std::cout << "Coefficient was deleted" << std::endl;
                        return false;
                    }
                    coeffsPerTerm[idx] *= static_cast<double> (*valuePtr);
                }
        }
    }
    
    for (int i = 0; i < coefficientTermIndex.size(); ++i)
    {
        for (int j = 0; j < coefficientTermIndex[i].size(); ++j)
        {
            String typeString = coefficientTermIndex[i][j].isInt() ? "integer" : "String";
            std::cout << "Entry is " << typeString << ". Value = " << coefficientTermIndex[i][j].toString() << std::endl;
        }
    }
    
    for (int i = 0; i < terms.size(); ++i)
    {
        int operatorMult = 1;
        
        // no operator before first term
        if (i > 0 && operatorVect[i - 1] == 2)
        {
            operatorMult = -1;
        }
        if (i < equalsSignIdx)
            terms[i] = terms[i] * operatorMult;
        else
            terms[i] = terms[i] * (operatorMult * -1);
        Equation term = terms[i] * coeffsPerTerm[i];
        std::cout << coeffsPerTerm[i] << std::endl;
        eq = eq + term;
    }
    
    int uNextIdx = (eq.getStencilWidth() - 1) / 2.0;
    
    if (eq.getUCoeffs(0)[uNextIdx] == 0)
    {
        std::cout << "No u^{n+1}" << std::endl;
        return false;
    }
    
    std::cout << "Stencil before division" << std::endl;
    eq.showStencil();
    
    if (!GUIDefines::debug)
        eq = eq / (eq.getUCoeffs(0)[uNextIdx]);
    
    std::cout << "Stencil after division" << std::endl;
    eq.showStencil();
    return true;
}

StringArray FDSsolver::getUsedCoeffs (String& equationString)
{
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1);
    
    StringArray usedCoeffs;
    for (auto token : tokens)
        if (!std::isdigit (*static_cast<const char*> (token.substring(0, 1).toUTF8())))
            usedCoeffs.add (token.upToFirstOccurrenceOf("-", false, true));
    
    return usedCoeffs;
}

double FDSsolver::calculateGridSpacing (Equation& eq, double coeff)
{
    // stability analysis here //
    
    
    
//    if (eq.getStencilWidth() == 5)
//        h = sqrt(2 * static_cast<double>(coeff) * k);
//    else if (eq.getStencilWidth() == 3)
        h = sqrt (static_cast<double> (coeff)) * k;
    
    return h;
}

bool FDSsolver::checkSyntax (StringArray& tokens)
{
    int prevTermType = 0;
    bool hasEqualsSign = false;
    
    if (!checkAllowedCharacters (prevTermType, tokens, hasEqualsSign))
        return false;
    
    if (!hasEqualsSign)
    {
        std::cout << "No equals sign" << std::endl;
        return false;
    }
    return true;
}

// Function is maybe obsolete
bool FDSsolver::checkAllowedCharacters (int prevTermType, StringArray& tokens, bool& hasEqualsSign)
{
    for (int i = 0; i < tokens.size(); ++i)
    {
        
        if (tokens[i] == "100")
        {
            hasEqualsSign = true;
        }
        
        String firstChar = tokens[i].substring(0, 1);
        
        const char* test = static_cast<const char*> (firstChar.toUTF8());
        
        int firstInt;
        
        if (std::isdigit(*test))
        {
            firstInt = firstChar.getIntValue();
        } else {
            String coeff = tokens[i].upToFirstOccurrenceOf ("-", false, true);
            if (!checkIfCoefficientExists (coeff))
            {
                std::cout << coeff << " was deleted. Please remove " << coeff << " from the equation." << std::endl;
                return false;
            }
            firstInt = 9;
        }
        
        if (i == tokens.size() - 1 && firstInt != 3)
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
                notAllowedCharacters.push_back(9);
                break;
            case 3:
                notAllowedCharacters.push_back(2);
                notAllowedCharacters.push_back(3);
                notAllowedCharacters.push_back(9);
                break;
            case 9:
                break;
        }
        
        // if the character can't be after the previous one, give an error
        if (std::find(notAllowedCharacters.begin(), notAllowedCharacters.end(), firstInt) != notAllowedCharacters.end())
        {
            std::cout << "Syntax error: " << firstInt << std::endl;
            return false;
        }
        // set the type of the previous character
        prevTermType = firstInt;
    }
    return true;
}
void FDSsolver::applyOperator(Equation *equation, void (*)(Equation *))
{
    
}

// OPERATORS //

Equation& FDSsolver::forwDiffX(Equation& eq)
{
    for (int i = 0; i < eq.getTimeSteps(); ++i)
        for (int j = eq.getStencilWidth() - 2; j >= 0; --j)
            eq.getUCoeffs(i)[j + 1] = eq.getUCoeffs(i)[j] - eq.getUCoeffs(i)[j + 1];
    
    if (!stabilityFlag)
        eq = eq * (1.0 / h);
    
    return eq;
}

Equation& FDSsolver::backDiffX (Equation& eq)
{
    for (int i = 0; i < eq.getTimeSteps(); ++i)
        for (int j = 1; j < eq.getStencilWidth(); ++j)
            eq.getUCoeffs(i)[j - 1] -= eq.getUCoeffs(i)[j];
    
    if (!stabilityFlag)
        eq = eq * (1.0 / h);
    
    return eq;
}

Equation& FDSsolver::centDiffX (Equation& eq)
{
    for (int i = 0; i < eq.getTimeSteps(); ++i)
    {
        std::vector<double> tmpSpace (eq.getStencilWidth(), 0);
        for (int j = 0; j < eq.getStencilWidth(); ++j)
        {
            tmpSpace[j] = eq.getUCoeffs(i)[j];
        }
        for (int j = 0; j < eq.getStencilWidth() - 1; ++j)
        {
            eq.getUCoeffs(i)[j + 1] += tmpSpace[j];
            eq.getUCoeffs(i)[j - 1] -= tmpSpace[j];
            eq.getUCoeffs(i)[j] -= tmpSpace[j];
        }
    }
    if (!stabilityFlag)
        eq = eq * (1.0 / (2 * h));
    
    return eq;
//    std::vector<double> tmpUNext (eq->getStencilWidth(), 0);
//    std::vector<double> tmpU (eq->getStencilWidth(), 0);
//    std::vector<double> tmpUPrev (eq->getStencilWidth(), 0);
//
//    for (int i = 0; i < eq->getStencilWidth(); ++i)
//    {
//        tmpUNext[i] = eq->getUNextCoeffs()[i];
//        tmpU[i] = eq->getUCoeffs()[i];
//        tmpUPrev[i] = eq->getUPrevCoeffs()[i];
//    }
//
//
//    for (int i = 0; i < eq->getStencilWidth(); ++i)
//    {
//        bool leftbound = i == 0 ? true : false;
//        bool rightbound = i == eq->getStencilWidth() - 1 ? true : false;
//        tmpUNext[i] = tmpUNext[i] + (rightbound ? 0 : eq->getUNextCoeffs()[i+1] ) - (leftbound ? 0 : eq->getUNextCoeffs()[i-1]) - eq->getUNextCoeffs()[i];
//        tmpU[i] = tmpU[i] + (rightbound ? 0 : eq->getUCoeffs()[i+1]) - (leftbound ? 0 : eq->getUCoeffs()[i-1]) - eq->getUCoeffs()[i];
//        tmpUPrev[i] = tmpUPrev[i] + (rightbound ? 0 : eq->getUPrevCoeffs()[i+1]) - (leftbound ? 0 : eq->getUPrevCoeffs()[i-1]) - eq->getUPrevCoeffs()[i];
//    }
//
//    for (int i = 0; i < eq->getStencilWidth(); ++i)
//    {
//        eq->getUNextCoeffs()[i] = tmpUNext[i];
//        eq->getUCoeffs()[i] = tmpU[i];
//        eq->getUPrevCoeffs()[i] = tmpUPrev[i];
//    }
//
//    *eq = (*eq) * (1.0 / (2*h));
//
//    return eq;
}

Equation& FDSsolver::forwDiffT (Equation& eq)
{
    for (int i = 1; i < eq.getTimeSteps(); ++i)
        for (int j = 0; j < eq.getStencilWidth(); ++j)
            eq.getUCoeffs(i - 1)[j] = eq.getUCoeffs(i)[j] - eq.getUCoeffs(i - 1)[j];

    if (!stabilityFlag)
        eq = eq * (1.0 / k);
    
    return eq;
}

Equation& FDSsolver::backDiffT (Equation& eq)
{
    for (int i = eq.getTimeSteps() - 2; i >= 0; --i)
        for (int j = 0; j < eq.getStencilWidth(); ++j)
            eq.getUCoeffs(i + 1)[j] -= eq.getUCoeffs(i)[j];
    
    if (!stabilityFlag)
        eq = eq * (1.0 / k);
    
    return eq;
}

Equation& FDSsolver::centDiffT (Equation& eq)
{
    for (int j = 0; j < eq.getStencilWidth(); ++j)
    {
        std::vector<double> tmpTime (eq.getTimeSteps(), 0);
        for (int i = 0; i < eq.getTimeSteps(); ++i)
        {
            tmpTime[i] = eq.getUCoeffs(i)[j];
        }
        for (int i = 1; i < eq.getTimeSteps() - 1; ++i)
        {
            eq.getUCoeffs(i - 1)[j] += tmpTime[i];
            eq.getUCoeffs(i + 1)[j] -= tmpTime[i];
            eq.getUCoeffs(i)[j] -= tmpTime[i];
        }
    }
    
    if (!stabilityFlag)
        eq = eq * (1.0 / (2 * k));
    
    return eq;
}

std::vector<std::vector<double>> FDSsolver::getStencil (Equation& eq)
{
    std::vector<std::vector<double>> stencil (3, std::vector<double> (eq.getStencilWidth(), 0));
    
    for (int i = 0; i < eq.getTimeSteps(); ++i)
        for (int j = 0; j < eq.getStencilWidth(); ++i)
            stencil[i][j] = eq.getUCoeffs(i)[j];
    
    return stencil;
};

int FDSsolver::getStencilWidth (String& equationString, bool checkSpace)
{
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1);
    
    bool derivFlag = false;
    int stencilSize = 1;
    int posTmpStencilSize = 0;
    int negTmpStencilSize = 0;
    
    for (int i = 0; i < tokens.size(); ++i)
    {
        if (!checkSpace)
        {
            switch (tokens[i].getIntValue())
            {
                case 200:
                    derivFlag = true;
                    ++posTmpStencilSize;
                    break;
                case 201:
                    derivFlag = true;
                    ++negTmpStencilSize;
                    break;
                case 202:
                case 203:
                    derivFlag = true;
                    ++posTmpStencilSize;
                    ++negTmpStencilSize;
                    break;
            }
        } else {
            switch (tokens[i].getIntValue())
            {
                case 204:
                    derivFlag = true;
                    ++posTmpStencilSize;
                    break;
                case 205:
                    derivFlag = true;
                    ++negTmpStencilSize;
                    break;
                case 206:
                case 207:
                    derivFlag = true;
                    ++posTmpStencilSize;
                    ++negTmpStencilSize;
                    break;
            }
        }
        if (tokens[i].substring(0, 1).getIntValue() != 2)
        {
            derivFlag = false;
            int tmpStencilSize = 2 * std::max (posTmpStencilSize, negTmpStencilSize) + 1;
            if (tmpStencilSize > stencilSize)
            {
                stencilSize = tmpStencilSize;
            }
            posTmpStencilSize = 0;
            negTmpStencilSize = 0;
        }
    }
    
    return stencilSize;
}

bool FDSsolver::checkIfCoefficientExists (String& coeff)
{
    for (auto coefficient : coefficientList->getCoefficients())
        if (coefficient->getName() == coeff)
            return true;
    return false;
}
