/*
 ==============================================================================
 
 Object2D.cpp
 Created: 24 Jul 2019 3:48:59pm
 Author:  Silvin Willemsen
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "Object2D.h"

//==============================================================================
Object2D::Object2D (String equationString, Equation stencil, std::vector<Equation> terms, double aspectRatio) : Object (equationString, stencil, terms, 2), aspectRatio (aspectRatio)
{
    uVecs.reserve (stencil.getTimeSteps()); //resize according to amount of vectors in stencil
    Nx = floor(sqrt (aspectRatio) / h);
    Ny = floor(1.0/(sqrt (aspectRatio) * h));
    
#ifdef CREATECCODE
    for (int i = 0; i < stencil.getTimeSteps(); ++i)
        uVecs.push_back (std::vector<double> (N, 0));
    u.resize (stencil.getTimeSteps());
    for (int i = 0; i < u.size(); ++i)
        u[i] = &uVecs[i][0];
#else
    for (int i = 0; i < stencil.getTimeSteps(); ++i)
        uVecs.push_back (std::vector<std::vector<double>> (Ny, std::vector<double> (Nx, 0)));
    
    u.resize (stencil.getTimeSteps());
    for (int i = 0; i < u.size(); ++i)
        u[i] = &uVecs[i][0];
#endif

}

Object2D::~Object2D()
{
}

void Object2D::paint (Graphics& g)
{
    float stateWidth = getWidth() / static_cast<double> (Nx - 4);
    float stateHeight = getHeight() / static_cast<double> (Ny - 4);
    int scaling = 10000;
    for (int x = 2; x < Nx - 2; ++x)
    {
        for (int y = 2; y < Ny - 2; ++y)
        {
#ifdef CREATECCODE
            int cVal = clamp (255 * 0.5 * (u[1][x + Nx * y] * scaling + 1), 0, 255);
#else
            int cVal = clamp (255 * 0.5 * (u[1][x][y] * scaling + 1), 0, 255);
#endif
            g.setColour(Colour::fromRGBA (cVal, cVal, cVal, 127));
            g.fillRect((x - 2) * stateWidth, (y - 2) * stateHeight, stateWidth, stateHeight);
        }
    }
}

void Object2D::resized()
{
    if (showButtons)
    {
        Rectangle<int> buttonArea = getLocalBounds();
        buttonArea.removeFromLeft (GUIDefines::horStateArea * getWidth());
        buttonArea.removeFromLeft (GUIDefines::margin);
        
        buttons[0]->setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
        buttonArea.removeFromTop (GUIDefines::margin * 0.5);
        buttons[1]->setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
        buttonArea.removeFromTop (GUIDefines::margin * 0.5);
        buttons[2]->setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
    }
    else if (appState != normalAppState)
    {
        Rectangle<int> boundButtonArea = getLocalBounds().removeFromBottom (GUIDefines::buttonHeight + GUIDefines::margin);
        boundButtonArea.removeFromBottom (GUIDefines::margin);
        boundaryButtons[0]->setBounds (boundButtonArea.removeFromLeft (GUIDefines::buttonWidth / 2.0));
        boundaryButtons[1]->setBounds (boundButtonArea.removeFromRight (GUIDefines::buttonWidth / 2.0));
    }
}

#ifdef CREATECCODE
void Object2D::createUpdateEq()
{
    // initialise update equation blocks struct
    UEB ueb;
    
    // start code with "uNext[l] = :
    String code = ueb.u(2, 0) + " =";
    
    // calculate half the stencil width
    int halfStencilWidth = (pow(stencil.getStencilWidth(), 1.0 / stencil.getDimension()) - 1) / 2.0;
    int horStencilWidth = pow (stencil.getStencilWidth(), 1.0 / stencil.getDimension());
    // check if stencil is symmetric (almost always the case..)
    if (isSymmetric)
    {
        // for the current to the last time step...
        for (int j = 1; j < stencil.getTimeSteps(); ++j)
        {
            // for the left part of the middle the stencil...
            for (int l = 0; l <= halfStencilWidth; ++l)
            {
                // for the left part of the middle the stencil..
                for (int m = 0; m < halfStencilWidth; ++m)
                {
                    // if the value isn't 0...
                    if (stencil.getUCoeffAt(j, l + m * horStencilWidth) != 0)
                    {
                        // add to the codestring
                        code += " - coeffs[" + String(l + m * horStencilWidth + (j - 1) * stencil.getStencilWidth()) + "] * ("
                            + ueb.u(2, j, l - halfStencilWidth, m - halfStencilWidth) + " + " + ueb.u(2, j, -l + halfStencilWidth, -m + halfStencilWidth) + " + "
                            + ueb.u(2, j, -m + halfStencilWidth,  l - halfStencilWidth) + " + " + ueb.u(2, j, m - halfStencilWidth, -l + halfStencilWidth) + ")";
                    }
                }
            }
            // if it is the spatial center of the stencil, only add one term
            code += " - coeffs[" + String (halfStencilWidth + halfStencilWidth * horStencilWidth + (j - 1) * stencil.getStencilWidth()) + "] * " + ueb.u(2, j);
        }
        // if the stencil is not symmetric (for some reason)
    } else {
        
//        int horStencilWidth = pow (stencil.getStencilWidth(), 1.0 / stencil.getDimension());
//        // for the current to the last time step...
//        for (int j = 1; j < stencil.getTimeSteps(); ++j)
//        {
//            // for the entire stencil...
//            for (int l = 0; l < horStencilWidth; ++l)
//            {
//                // for the entire stencil...
//                for (int m = 0; m < horStencilWidth; ++m)
//                {
//                    // if the value isn't 0...
//                    if (stencil.getUCoeffAt (j, l + m * Nx) != 0)
//                        // add to the codestring
//                        code += " - coeffs[" + String (l + m * Nx + (j - 1) * horStencilWidth) + "] * " + ueb.u(2, j, l - halfStencilWidth, m - halfStencilWidth);
//                }
//            }
    }
    
    // end codestring with a ;
    code += ";";
    
    // wrap update in a forloop
    String forLoop = ueb.forLoop2D (code, boundaryConditions[0] ? 1 : 2, Nx, Ny);
    
    // generate c-code
    updateEqGenerator (forLoop);
}
#endif

void Object2D::calculateFDS()
{
#ifdef CREATECCODE
    // input states and coefficients in vector form
    updateEq (u[0], u[1], u[2], &stencilVectorForm[0], Nx);
#else
//    int horStencilWidth = pow (stencil.getStencilWidth(), 1.0 / stencil.getDimension());
//    int halfHorStencilWidth = floor (horStencilWidth * 0.5);
//    for (int l = (boundaryConditions[1] == simplySupported ? 1 : 2);
//             l < (boundaryConditions[3] == simplySupported ? Nx - 1 : Nx - 2); ++l)
//    {
//        for (int m = (boundaryConditions[0] == simplySupported ? 1 : 2);
//             m < (boundaryConditions[2] == simplySupported ? Ny - 1 : Ny - 2); ++m)
//        {
//            u[0][l][m] = 0;
//            for (int j = 0; j < horStencilWidth; ++j)
//            {
//                u[0][l][m] = u[0][l][m] - (curTimeStep[j + halfHorStencilWidth * m] * u[1][l - halfHorStencilWidth + j][m - halfHorStencilWidth + j] + prevTimeStep[j + halfHorStencilWidth * m] * u[2][l - halfHorStencilWidth + j][m - halfHorStencilWidth] + j);
////                u[0][l + m * Nx] = - (curTimeStep[j] * u[1][l - stencilIdxStart + j] + prevTimeStep[j] * u[2][l - stencilIdxStart + j]);
////
////                u[0][l + m * Nx] = -(curTimeStep[j] * u[1][l - stencilIdxStart + j + ]) * (u[l-1 + (m-1) * Nx] + u[l+1 + (m+1) * Nx] + u[l+1 + (m-1) * Nx] + u[l-1 + (m+1) * Nx]) - coeffs[2] * (u[l + (m-2) * Nx] + u[l + (m+2) * Nx] + u[l+2 + m * Nx] + u[l-2 + m * Nx]) - coeffs[7] * (u[l + (m-1) * Nx] + u[l + (m+1) * Nx] + u[l+1 + m * Nx] + u[l-1 + m * Nx]) - coeffs[12] * u[l + m * Nx] - coeffs[37] * uPrev[l + m * Nx];
//            }
//        }
//    }
#endif
}

void Object2D::updateStates()
{
    int lengthUVec = static_cast<int>(u.size());
#ifdef CREATECCODE
    for (int i = lengthUVec - 1; i > 0; --i)
        u[i] = u[i - 1];
    uNextPtrIdx = (uNextPtrIdx + (lengthUVec - 1)) % lengthUVec;
    u[0] = &uVecs[uNextPtrIdx][0];
#else
    for (int i = lengthUVec - 1; i > 0; --i)
        u[i] = u[i - 1];
    uNextPtrIdx = (uNextPtrIdx + (lengthUVec - 1)) % lengthUVec;
    u[0] = &uVecs[uNextPtrIdx][0];
#endif
}


void Object2D::excite()
{
    if (excited)
    {
        int stateWidth = getWidth() / static_cast<double> (Nx - 4);
        int stateHeight = getHeight() / static_cast<double> (Ny - 4);
        int idX = xLoc / stateWidth + 2;
        int idY = yLoc / stateHeight + 2;
        
        excited = false;
        int width = floor ((Nx * 2.0) / 5.0) / 2.0;
//        int loc = floor (Nx * static_cast<float>(getXLoc()) / static_cast<float>(getWidth()));
//        int startIdx = clamp (loc - width / 2.0, simplySupported ? 1 : 2, simplySupported ? N-1-width : N-2-width);
        
        std::vector<std::vector<double>> excitationArea (width, std::vector<double> (width, 0));
        
        for (int i = 1; i < width; ++i)
        {
            for (int j = 1; j < width; ++j)
            {
                excitationArea[i][j] = 1.0 / (width * width) * 0.25 * (1 - cos(2.0 * double_Pi * i / static_cast<int>(width+1))) * (1 - cos(2.0 * double_Pi * j / static_cast<int>(width+1)));
            }
        }
        
        int startIdX = clamp(idX - width * 0.5, 2, Nx-3 - width);
        int startIdY = clamp(idY - width * 0.5, 2, Ny-3 - width);
        
        for (int i = 1; i < width; ++i)
        {
            for (int j = 1; j < width; ++j)
            {
#ifdef CREATECCODE
                u[1][static_cast<int> (i + startIdX + Nx * (j + startIdY))] += excitationArea[i][j];
                u[2][static_cast<int> (i + startIdX + Nx * (j + startIdY))] += excitationArea[i][j];
#else
                u[1][i + startIdX][j + startIdY] += excitationArea[i][j];
                u[2][i + startIdX][j + startIdY] += excitationArea[i][j];
#endif
            }
        }
        
//        for (int i = 0; i < width; ++i)
//        {
//            double val = (1 - cos (2 * double_Pi * i / width)) * 0.5;
//            for (int j = 1; j < u.size(); ++j)
//                u[j][startIdx + i] = u[j][startIdx + i] + val;
//        }
    }
}

void Object2D::setZero()
{
#ifdef CREATECCODE
    for (int i = 0; i < uVecs.size(); ++i)
        for (int j = 0; j < uVecs[i].size(); ++j)
            uVecs[i][j] = 0;
#else
    for (int i = 0; i < uVecs.size(); ++i)
        for (int j = 0; j < uVecs[i].size(); ++j)
            for (int k = 0; k < uVecs[j].size(); ++k)
            uVecs[i][j][k] = 0;
#endif
    
    setZFlag = false;
}

//Path Object2D::visualiseState()
//{
//    auto stringBounds = getHeight() / 2.0;
//    Path stringPath;
//    stringPath.startNewSubPath (0, stringBounds);
//    int stateWidth = (showButtons ? GUIDefines::horStateArea : 1.0) * getWidth();
//    auto spacing = stateWidth / static_cast<double>(N);
//    auto x = spacing;
//    
//    for (int y = 1; y < N; y++)
//    {
//        int visualScaling = 10;
//#ifdef CREATECCODE
//        float newY = u[0][y] * visualScaling + stringBounds;
//#else
//        float newY = u[0][y] * visualScaling + stringBounds;
//#endif
//        
//        if (isnan(x) || isinf(abs(x) || isnan(newY) || isinf(abs(newY))))
//        {
//            std::cout << "Wait" << std::endl;
//        };
//        
//        if (isnan(newY))
//            newY = 0;
//        stringPath.lineTo(x, newY);
//        x += spacing;
//    }
//    stringPath.lineTo(stateWidth, stringBounds);
//    return stringPath;
//}

void Object2D::changeBoundaryCondition()
{
//    BoundaryCondition& boundaryToChange = boundaryConditions[boundaryChangeIdx];
//    
//    if (boundaryToChange == simplySupported)
//        for (int i = 0; i < uVecs.size(); ++i)
//        {
//            int idx = boundaryConditions[0] ? 1 : N - 2;
//            uVecs[i][idx] = 0;
//        }
//    
//    boundaryToChange = (boundaryToChange == clamped) ? simplySupported : clamped;
//    
//    BCChangeFlag = false;
//    boundaryChangeIdx = -1;
}
