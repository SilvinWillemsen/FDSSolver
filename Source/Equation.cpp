/*
  ==============================================================================

    Equation.cpp
    Created: 12 Jul 2019 2:17:09pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "Equation.h"

Equation::Equation(int max, bool createULN)
{
    uNextCoeffs.resize(max);
    uCoeffs.resize(max);
    uPrevCoeffs.resize(max);
    
    if (createULN)
    {
        int idx = (max - 1) / 2.0;
        uCoeffs[idx] = 1;
    }
}

bool Equation::check (int idx)
{
    if (uNextCoeffs[idx] != 0 ||
        uCoeffs[idx] != 0 ||
        uPrevCoeffs[idx] != 0)
    {
        std::cout << "Size of the uCoeffs vector is incorect: " << std::endl;
        std::cout << "Index " << idx << " should be 0." << std::endl;
        return false;
    }
    return true;
}

bool Equation::checkOperation (Equation& eq)
{
    if (getCoeffsSize() != eq.getCoeffsSize())
    {
        std::cout << "Can't perform operation. Equations don't have the same dimensions." << std::endl;
        return false;
    }
    return true;
}

void Equation::showStencil()
{
    for (int i = 0; i < getCoeffsSize(); ++i)
        std::cout << uNextCoeffs[i] << " ";
    std::cout << std::endl;
    
    for (int i = 0; i < getCoeffsSize(); ++i)
        std::cout << uCoeffs[i] << " ";
    std::cout << std::endl;
    
    for (int i = 0; i < getCoeffsSize(); ++i)
        std::cout << uPrevCoeffs[i] << " ";
    std::cout << std::endl;
}
