/*
  ==============================================================================

    Equation.cpp
    Created: 12 Jul 2019 2:17:09pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "Equation.h"

Equation::Equation (int amountOfTimeSteps, int amountOfGridPoints, bool createULN)
{
    uCoeffs.resize (amountOfTimeSteps);
    std::vector<double> uTmp (amountOfGridPoints, 0);
    for (int i = 0; i < amountOfTimeSteps; ++i)
        uCoeffs[i] = uTmp;
    
    if (createULN)
    {
        int timeIdx = (amountOfTimeSteps - 1) / 2.0;
        int spaceIdx = (amountOfGridPoints - 1) / 2.0;
        
        uCoeffs[timeIdx][spaceIdx] = 1;
    }
}

bool Equation::check (int idx)
{
//    if (uNextCoeffs[idx] != 0 ||
//        uCoeffs[idx] != 0 ||
//        uPrevCoeffs[idx] != 0)
//    {
//        std::cout << "Size of the uCoeffs vector is incorect: " << std::endl;
//        std::cout << "Index " << idx << " should be 0." << std::endl;
//        return false;
//    }
    return true;
}

bool Equation::checkOperation (Equation& eq)
{
    if (getStencilWidth() != eq.getStencilWidth())
    {
        std::cout << "Can't perform operation. Equations don't have the same dimensions." << std::endl;
        return false;
    }
    return true;
}

void Equation::showStencil()
{
    for (int i = 0; i < uCoeffs.size(); ++i)
    {
        for (int j = 0; j < getStencilWidth(); ++j)
            std::cout << uCoeffs[i][j] << " ";
        std::cout << std::endl;
    }
    
}
