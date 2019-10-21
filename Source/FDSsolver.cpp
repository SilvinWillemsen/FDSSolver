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
}

FDSsolver::~FDSsolver()
{
}

bool FDSsolver::solve (String& equationString, Equation& stencil, NamedValueSet* coeffValues, Array<var>& coefficientTermIndex, std::vector<Equation>& terms)
{
    // Subdivide the equation string into tokens
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1); // remove last token as it is empty
    
    // check syntax if not debugging
    if (!GUIDefines::debug)
        if (!checkSyntax (tokens))
            return false;
    
    // calculate number of terms (parts of the equation between the operators)
    int numTerms = 1;
    for (int i = 0; i < tokens.size(); ++i)
    {
        String firstChar = tokens[i].substring(0, 1);
        int firstInt = firstChar.getIntValue();
        
        // if the token is an operator (+, -, =) add to the number of terms (simple as that :) )
        if (firstInt == 1)
        {
            ++numTerms;
        }
    }
    
    // variable to hold after what term the equals sign is located
    int equalsSignIdx = 0;
    
    // flag used in the loop below to determine whether a new term should be added or not
    bool newTermFlag = true;
    
    // reset the terms vector
    terms.reserve (numTerms);
    
    // clear the coefficientTermIndex and initialise with 1's
    coefficientTermIndex.clear();
    Array<var> tmpArray;
    tmpArray.add (1);
    
    for (int i = 0; i < numTerms; ++i)
        coefficientTermIndex.add (tmpArray);
    
    // clear the coeffsPerTerm and initialise with 1's
    coeffsPerTerm.clear();
    coeffsPerTerm.shrink_to_fit();
    coeffsPerTerm.resize (numTerms, 1);
    
    // vector holding if a term is added or subtracted. The first term is always positive (a negative-sign will be included in the coefficientTermIndex)
    
    std::vector<int> operatorMult (numTerms, 0);
    operatorMult[0] = 1;
    
    int termIdx = -1;
    
    // calculate h
    if (GUIDefines::debug)
        h = 1;
    else
        calculateGridSpacing (tokens, coeffValues, stencil, numTerms);
    
    stencil.setNumPointsFromGridSpacing (h);
    h = 1.0 / pow (static_cast<double> (stencil.getNumPoints()), 1.0 / stencil.getDimension());
    
    for (int i = 0; i < tokens.size(); ++i)
    {
        String firstChar = tokens[i].substring(0, 1);
        
        const char* test = static_cast<const char*> (firstChar.toUTF8());
        int firstInt = std::isdigit(*test) ? firstChar.getIntValue() : 9;
        
        
        if (newTermFlag)
        {
            // Add a new empty (u_l^n) term to the terms vector
            terms.push_back (Equation (stencil.getTimeSteps(), stencil.getStencilWidth(), true));
            terms[terms.size() - 1].setDimension (stencil.getDimension());
            // Increase the term index
            ++termIdx;
            newTermFlag = false;
        }
        switch (firstInt)
        {
            case 1:
                // Save whether a term is added or subtracted
                operatorMult[termIdx] = tokens[i].substring(2, 3).getIntValue() == 2 ? -1 : 1;
                
                // Set the term-index of the equals sign
                if (tokens[i].getIntValue() == 100)
                {
                    equalsSignIdx = termIdx;
                }
                break;
            case 2:
                // Apply difference operators to the current term
                switch(tokens[i].getIntValue())
                {
                    case 200:
                        forwDiffT (terms[termIdx]);
                        break;
                    case 201:
                        backDiffT (terms[termIdx]);
                        break;
                    case 202:
                        centDiffT (terms[termIdx]);
                        break;
                    case 203:
                        secondOrderT (terms[termIdx]);
                        break;
                    case 204:
                        forwDiffX (terms[termIdx]);
                        break;
                    case 205:
                        backDiffX (terms[termIdx]);
                        break;
                    case 206:
                        centDiffX (terms[termIdx]);
                        break;
                    case 207:
                        secondOrderX (terms[termIdx]);
                        break;
                    case 208:
                        secondOrderX (terms[termIdx]);
                        break;
                    case 209:
                        secondOrderY (terms[termIdx]);
                        break;
                    case 210:
                        secondOrder2D (terms[termIdx]);
                        break;
                }
                break;
            case 3:
                // if the token is the state variable u, a new term is coming up
                newTermFlag = true;
                break;
            case 9:
                //// Save the coefficient in the coefficientTermIndex array ////
                
                // if the token is a "negative sign", add -1 to the coefficientTermIndex and multiply the coeffsPerTerm by -1.0
                if (tokens[i].getIntValue() == 901)
                {
                    coefficientTermIndex[termIdx][0] = -1;
                    coeffsPerTerm[termIdx] *= -1.0;
                } else {
                    if (static_cast<int> (coefficientTermIndex[termIdx][0]) == 1)
                        coefficientTermIndex[termIdx][0] = tokens[i].upToFirstOccurrenceOf ("-", false, true);
                    else
                        coefficientTermIndex[termIdx].append (tokens[i].upToFirstOccurrenceOf ("-", false, true));
                  
                    // Get the value of the coeffValues NamedValueSet using the coefficient name
                    auto valuePtr =  coeffValues->getVarPointer (tokens[i].upToFirstOccurrenceOf ("-", false, true));
                    
                    if (valuePtr == NULL)
                    {
                        std::cout << "Coefficient was deleted" << std::endl;
                        return false;
                    }
                    
                    // Multiply the coeffsPerTerm with the value at the current coefficient
                    coeffsPerTerm[termIdx] *= static_cast<double> (*valuePtr);
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
        // If the terms are before the equals sign, add them
        if (i < equalsSignIdx)
            terms[i] = terms[i] * operatorMult[i];
        else // Otherwise, subtract them
            terms[i] = terms[i] * (operatorMult[i] * -1);
        
        // multiply the coefficients to their respective terms and add them to the stencil
        Equation term = terms[i] * coeffsPerTerm[i];
        stencil = stencil + term;
    }
    
    // Find the middle of the stencil (space-wise) at the next time-step
    int uNextIdx = (stencil.getStencilWidth() - 1) / 2.0;
    
    // If there is no uNext, don't continue
    if (stencil.getUCoeffs(0)[uNextIdx] == 0)
    {
        std::cout << "No u^{n+1}" << std::endl;
        return false;
    }
    
    //// Divide the stencil by whatever uNext is multiplied by (if not in debug mode) ////
    std::cout << "Stencil before division" << std::endl;
    stencil.showStencil();
    
    if (!GUIDefines::debug)
        stencil = stencil / (stencil.getUCoeffs(0)[uNextIdx]);
    
    std::cout << "Stencil after division" << std::endl;
    stencil.showStencil();
    return true;
}

StringArray FDSsolver::getUsedCoeffs (String& equationString)
{
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1);
    
    StringArray usedCoeffs;
    
    // for all tokens
    for (auto token : tokens)
        
        // if the first character is not a digit
        if (!std::isdigit (*static_cast<const char*> (token.substring(0, 1).toUTF8())))
            
            // add the letters (not the dashes) to the usedCoeffs vector
            usedCoeffs.add (token.upToFirstOccurrenceOf ("-", false, true));
    
    return usedCoeffs;
}

double FDSsolver::calculateGridSpacing (StringArray& tokens, NamedValueSet* coeffValues, Equation& stencil, int numTerms)
{
//    // stability analysis here //
//    int idx = -1;
//    bool newTermFlag = true;
//    Equation zCoeffs (eq.getTimeSteps(), 1);
//    std::vector<Equation> terms;
//    terms.reserve (numTerms);
//    int equalsSignIdx;
//
//    std::vector<int> operatorVect (numTerms - 1, 0);
//
//    for (int i = 0; i < tokens.size(); ++i)
//    {
//        String firstChar = tokens[i].substring(0, 1);
//
//        const char* test = static_cast<const char*> (firstChar.toUTF8());
//        int firstInt = std::isdigit(*test) ? firstChar.getIntValue() : 9;
//
//        if (newTermFlag)
//        {
//            terms.push_back (Equation (eq.getTimeSteps(), eq.getStencilWidth(), true));
//            ++idx;
//            newTermFlag = false;
//        }
//        switch (firstInt)
//        {
//            case 1:
//                operatorVect[idx-1] = tokens[i].substring(2, 3).getIntValue();
//                if (tokens[i].getIntValue() == 100)
//                {
//                    equalsSignIdx = idx;
//                }
//                break;
//            case 2:
//
//                switch(tokens[i].getIntValue())
//            {
//                case 200:
//                    ++zCoeffs.getUCoeffs (0)[0];
//                    --zCoeffs.getUCoeffs (1)[0];
//                    break;
//                case 201:
//                    ++zCoeffs.getUCoeffs (1)[0];
//                    --zCoeffs.getUCoeffs (2)[0];
//                    break;
//                case 202:
//                    ++zCoeffs.getUCoeffs (0)[0];
//                    --zCoeffs.getUCoeffs (2)[0];
//                    break;
//                case 203:
//                    ++zCoeffs.getUCoeffs (0)[0];
//                    zCoeffs.getUCoeffs (1)[0] -= 2;
//                    ++zCoeffs.getUCoeffs (2)[0];
//                    break;
//                case 204:
//                    forwDiffX (terms[idx]);
//                    break;
//                case 205:
//                    backDiffX (terms[idx]);
//                    break;
//                case 206:
//                    centDiffX (terms[idx]);
//                    break;
//                case 207:
//                    secondOrderX (terms[idx]);
//                    break;
//                case 208:
//                    fourthOrderX (terms[idx]);
//                    break;
//            }
//                break;
//            case 3:
//                newTermFlag = true;
//                break;
//            case 9:
//
//                if (tokens[i].getIntValue() == 901)
//                {
//                    coefficientTermIndex[idx][0] = -1;
//                    coeffsPerTerm[idx] *= -1.0;
//                } else {
//                    if (static_cast<int> (coefficientTermIndex[idx][0]) == 1)
//                        coefficientTermIndex[idx][0] = tokens[i].upToFirstOccurrenceOf ("-", false, true);
//                    else
//                        coefficientTermIndex[idx].append (tokens[i].upToFirstOccurrenceOf ("-", false, true));
//
//                    auto valuePtr =  coeffValues->getVarPointer (tokens[i].upToFirstOccurrenceOf ("-", false, true));
//                    if (valuePtr == NULL)
//                    {
//                        std::cout << "Coefficient was deleted" << std::endl;
//                        return false;
//                    }
//                    coeffsPerTerm[idx] *= static_cast<double> (*valuePtr);
//                }
//        }
//    }
//
//    for (int i = 0; i < coefficientTermIndex.size(); ++i)
//    {
//        for (int j = 0; j < coefficientTermIndex[i].size(); ++j)
//        {
//            String typeString = coefficientTermIndex[i][j].isInt() ? "integer" : "String";
//            std::cout << "Entry is " << typeString << ". Value = " << coefficientTermIndex[i][j].toString() << std::endl;
//        }
//    }
//
//    for (int i = 0; i < terms.size(); ++i)
//    {
//        int operatorMult = 1;
//
//        // no operator before first term
//        if (i > 0 && operatorVect[i - 1] == 2)
//        {
//            operatorMult = -1;
//        }
//        if (i < equalsSignIdx)
//            terms[i] = terms[i] * operatorMult;
//        else
//            terms[i] = terms[i] * (operatorMult * -1);
//        Equation term = terms[i] * coeffsPerTerm[i];
//        std::cout << coeffsPerTerm[i] << std::endl;
//        eq = eq + term;
//    }
//
//
//    if (eq.getStencilWidth() == 5)
//        h = sqrt(2 * static_cast<double>(coeff) * k);
//        else if (eq.getStencilWidth() == 3)
    if (stencil.getDimension() == 2)
    {
        if (coeffValues[0].size() <= 2)
        {
            h = 2 * sqrt (k * sqrt(static_cast<double>(coeffValues[0].getValueAt (0))));
        } else {
            double sig1 = static_cast<double> (coeffValues[0].getValueAt(2)) * 0.5;
            h = 2 * sqrt (k * (sig1 * sig1 + sqrt(static_cast<double>(coeffValues[0].getValueAt (0)) + sig1 * sig1)));
        }
    }
    else if (stencil.getStencilWidth() == 3)
    {
        h = sqrt (static_cast<double> (coeffValues[0].getValueAt (0))) * k;
    }
    else
    {
    //    h = sqrt (sqrt(static_cast<double>(coeffValues[0].getValueAt (0))) * k * 2);
        double kappaSq = coeffValues[0].getValueAt (0);
        if (coeffValues[0].size() <= 2)
        {
            h = sqrt (sqrt(static_cast<double>(coeffValues[0].getValueAt (0))) * k * 2);
        } else {
            double sig1 = static_cast<double> (coeffValues[0].getValueAt(2)) * 0.5;
            h = sqrt((4.0f * sig1 * k + sqrt(pow(4.0f * sig1 * k, 2) + 16.0 * kappaSq * k * k)) / 2.0f);
        }
    }
    return h;
}

bool FDSsolver::checkSyntax (StringArray& tokens)
{
    // Variable holding the first digit of the previous term to check what terms can legally come after
    int prevTermType = 0;
    
    bool hasEqualsSign = false;
    
    // For all tokens
    for (int i = 0; i < tokens.size(); ++i)
    {
        // if the token is an equals sign, set the flag to true
        if (tokens[i] == "100")
        {
            hasEqualsSign = true;
        }
        
        // Retrieve the first character of the token
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
        checkNotAllowedCharacters (prevTermType, notAllowedCharacters);
        
        // if the character can't be after the previous one, give an error
        if (std::find(notAllowedCharacters.begin(), notAllowedCharacters.end(), firstInt) != notAllowedCharacters.end())
        {
            std::cout << "Syntax error: " << firstInt << std::endl;
            return false;
        }
        // set the type of the previous character
        prevTermType = firstInt;
    }
    
    if (!hasEqualsSign)
    {
        std::cout << "No equals sign" << std::endl;
        return false;
    }
    return true;
}

void FDSsolver::checkNotAllowedCharacters (int firstInt, std::vector<int>& notAllowedCharacters)
{
    switch (firstInt)
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
}

// OPERATORS //

Equation& FDSsolver::forwDiffX (Equation& eq)
{
    for (int i = 0; i < eq.getTimeSteps(); ++i)
        for (int j = eq.getStencilWidth() - 2; j >= 0; --j)
            eq.getUCoeffs(i)[j + 1] = eq.getUCoeffs(i)[j] - eq.getUCoeffs(i)[j + 1];
    
    eq = eq * (1.0 / h);
    
    return eq;
}

Equation& FDSsolver::backDiffX (Equation& eq)
{
    for (int i = 0; i < eq.getTimeSteps(); ++i)
        for (int j = 1; j < eq.getStencilWidth(); ++j)
            eq.getUCoeffs(i)[j - 1] -= eq.getUCoeffs(i)[j];
    
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
    eq = eq * (1.0 / (2 * h));
    
    return eq;
}

Equation& FDSsolver::forwDiffY (Equation& eq)
{
    int horStencilWidth = pow(eq.getStencilWidth(), 1.0 / eq.getDimension());
    for (int i = 0; i < eq.getTimeSteps(); ++i)
        for (int j = eq.getStencilWidth() - horStencilWidth; j >= 0; --j)
            eq.getUCoeffs(i)[j + horStencilWidth] = eq.getUCoeffs(i)[j] - eq.getUCoeffs(i)[j + horStencilWidth];
    
    eq = eq * (1.0 / h);
    
    return eq;
}

Equation& FDSsolver::backDiffY (Equation& eq)
{
    int horStencilWidth = pow(eq.getStencilWidth(), 1.0 / (eq.getDimension()));
    for (int i = 0; i < eq.getTimeSteps(); ++i)
        for (int j = horStencilWidth; j < eq.getStencilWidth(); ++j)
            eq.getUCoeffs(i)[j - horStencilWidth] -= eq.getUCoeffs(i)[j];

    eq = eq * (1.0 / h);

    return eq;
}
//
Equation& FDSsolver::centDiffY (Equation& eq)
{
//    for (int i = 0; i < eq.getTimeSteps(); ++i)
//    {
//        std::vector<double> tmpSpace (eq.getStencilWidth(), 0);
//        for (int j = 0; j < eq.getStencilWidth(); ++j)
//        {
//            tmpSpace[j] = eq.getUCoeffs(i)[j];
//        }
//        for (int j = 0; j < eq.getStencilWidth() - 1; ++j)
//        {
//            eq.getUCoeffs(i)[j + 1] += tmpSpace[j];
//            eq.getUCoeffs(i)[j - 1] -= tmpSpace[j];
//            eq.getUCoeffs(i)[j] -= tmpSpace[j];
//        }
//    }
//    eq = eq * (1.0 / (2 * h));
//
//    return eq;
}



Equation& FDSsolver::forwDiffT (Equation& eq)
{
    for (int i = 1; i < eq.getTimeSteps(); ++i)
        for (int j = 0; j < eq.getStencilWidth(); ++j)
            eq.getUCoeffs(i - 1)[j] = eq.getUCoeffs(i)[j] - eq.getUCoeffs(i - 1)[j];

    eq = eq * (1.0 / k);
    
    return eq;
}

Equation& FDSsolver::backDiffT (Equation& eq)
{
    for (int i = eq.getTimeSteps() - 2; i >= 0; --i)
        for (int j = 0; j < eq.getStencilWidth(); ++j)
            eq.getUCoeffs(i + 1)[j] -= eq.getUCoeffs(i)[j];
    
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
    
    eq = eq * (1.0 / (2 * k));
    
    return eq;
}

int FDSsolver::getStencilWidthFromEqString (String& equationString, bool checkSpatial)
{
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1);
    
    // start with a stencilsize of 1
    int stencilSize = 1;
    
    int posTmpStencilSize = 0;
    int negTmpStencilSize = 0;
    
    // If the model is 2D...
    bool twoD = false;
    
    // check stencilsize per term
    for (int i = 0; i < tokens.size(); ++i)
    {
        if (!checkSpatial)
        {
            switch (tokens[i].getIntValue())
            {
                case 200: // d_t+
                    ++posTmpStencilSize;
                    break;
                case 201: // d_t-
                    ++negTmpStencilSize;
                    break;
                case 202: // d_{t\dot}
                case 203: // d_{tt}
                    ++posTmpStencilSize;
                    ++negTmpStencilSize;
                    break;
            }
        } else {
            switch (tokens[i].getIntValue())
            {
                case 204: // d_x+
                    ++posTmpStencilSize;
                    break;
                case 205: // d_x-
                    ++negTmpStencilSize;
                    break;
                case 206: // d_{x\dot}
                case 207: // d_{xx}
                    ++posTmpStencilSize;
                    ++negTmpStencilSize;
                    break;
                case 208:
                case 209:
                case 210:
                    ++posTmpStencilSize;
                    ++negTmpStencilSize;
                    twoD = true;
                    break;
            }
        }
        
        // check if the current term has a wider (temporal/spatial) stencil than what has already been saved
        if (tokens[i].substring(0, 1).getIntValue() != 2)
        {
//            derivFlag = false;
            int tmpStencilSize = 2 * std::max (posTmpStencilSize, negTmpStencilSize) + 1;
            
            // if the term has a wider stencil than the saved stencilsize, update the stencilSize variable
            if (tmpStencilSize > stencilSize)
            {
                stencilSize = tmpStencilSize;
            }
            
            // reset stencil
            posTmpStencilSize = 0;
            negTmpStencilSize = 0;
        }
    }
    
    // If the stencil is two-dimensional, square the stencil size (note: this is all assuming that the stencil is symmetric in all spatial directions)
    if (twoD)
        stencilSize = stencilSize * stencilSize;

    return stencilSize;
}

bool FDSsolver::checkIfCoefficientExists (String& coeff)
{
    for (auto coefficient : coefficientList->getCoefficients())
        if (coefficient->getName() == coeff)
            return true;
    return false;
}

int FDSsolver::checkDimension (String equationString)
{
    StringArray tokens;
    tokens.addTokens (equationString, "_", "\"");
    tokens.remove (tokens.size() - 1);
    for (int i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i].getIntValue() >= 204 && tokens[i].getIntValue() < 208)
            return 1;
        else if (tokens[i].getIntValue() >= 208 && tokens[i].getIntValue() < 212)
            return 2;
    }
    return 0;
}
