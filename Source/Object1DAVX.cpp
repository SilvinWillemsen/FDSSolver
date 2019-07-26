/*
  ==============================================================================

    Object1DAVX.cpp
    Created: 25 Jul 2019 2:30:44pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Object1DAVX.h"

//==============================================================================
Object1DAVX::Object1DAVX (String equationString, Equation stencil, std::vector<Equation> terms, std::vector<BoundaryCondition> boundaries) : Object (equationString, stencil, terms, boundaries)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    numAVX = N / 4;
    if (N % 4 != 0)
        ++numAVX;
    
    int tmpNumAVX = numAVX;
    int tmpTimeSteps = stencil.getTimeSteps();
    std::vector<__m256d> zerovec;
    zerovec.resize (tmpNumAVX, zeroval);
    uVecs.resize (tmpTimeSteps, zerovec); //resize using zeroval
    uVecsPlus1.resize (tmpTimeSteps - 1, zerovec);
    uVecsMin1.resize (tmpTimeSteps - 1, zerovec);
    u.resize (numTimeSteps);
    
    for (int i = 0; i < u.size(); ++i)
        u[i] = &uVecs[i][0];
    
    for (int i = 0; i < 4; ++i)
        std::cout << ((double*)&u[0][0])[i] << std::endl;
}

Object1DAVX::~Object1DAVX()
{
}

void Object1DAVX::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::cyan);
    Path stringPath = visualiseState();
    g.strokePath (stringPath, PathStrokeType(2.0f));
}


void Object1DAVX::resized()
{
    if (showButtons)
    {
        Rectangle<int> buttonArea = getLocalBounds();
        buttonArea.removeFromLeft (GUIDefines::horStateArea * getWidth());
        buttonArea.removeFromLeft (GUIDefines::margin);
        
        muteButton->setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
        buttonArea.removeFromTop (GUIDefines::margin * 0.5);
        editButton->setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
        buttonArea.removeFromTop (GUIDefines::margin * 0.5);
        removeButton->setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
    }
    else if (appState != normalAppState)
    {
        Rectangle<int> boundButtonArea = getLocalBounds().removeFromBottom (GUIDefines::buttonHeight + GUIDefines::margin);
        boundButtonArea.removeFromBottom (GUIDefines::margin);
        boundaryButtons[0]->setBounds (boundButtonArea.removeFromLeft (GUIDefines::buttonWidth / 2.0));
        boundaryButtons[1]->setBounds (boundButtonArea.removeFromRight (GUIDefines::buttonWidth / 2.0));
    }
}

void Object1DAVX::calculateFDS()
{
    int lowerBound = boundaryConditions[0] == simplySupported ? 1 : 2;
    int upperBound = boundaryConditions[1] == simplySupported ? N : N - 1;
    
    double extraValPlus;
    double extraValMin;
    for (int i = 0; i < uVecsPlus1.size(); ++i)
        for (int l = 0; l < numAVX; ++l)
        {
            if (l != numAVX - 1)
            {
                extraValPlus = ((double*)&u[i+1][l+1])[3];
            } else {
                extraValPlus = 0.0;
            }
            
            if (l != 0)
                extraValMin = ((double*)&u[i+1][l-1])[0];
            else
                extraValMin = 0.0;
            uVecsPlus1[i][l] = _mm256_set_pd (((double*)&u[i+1][l])[2],
                                              ((double*)&u[i+1][l])[1],
                                              ((double*)&u[i+1][l])[0],
                                              extraValPlus);
            uVecsMin1[i][l] = _mm256_set_pd (extraValMin,
                                             ((double*)&u[i+1][l])[3],
                                             ((double*)&u[i+1][l])[2],
                                             ((double*)&u[i+1][l])[1]);
        }
    
    for (int l = 0; l < numAVX; ++l)
    {
        if (l == 0)
        {
            //stencil[0:lowerBound] = 0;
        }
        else if (l == numAVX)
        {
            //stencil[upperBound:end] = 0;
        }
        for (int j = 0; j < stencil.getStencilWidth(); ++j)
        {
            u[0][l] = u[0][l] - stencil[1][j] * u[1][l - stencilIdxStart + j] - stencil[2][j] * u[2][l - stencilIdxStart + j];
        }
    }
//    for (int l = lowerBound; l < upperBound; ++l)
//    {
//        uNext[l] = -(A1 * u[l] + A2 * (u[l-1] + u[l+1]) + A3 * (u[l-2] + u[l+2]) + B * uPrev);
//    }
}

double Object1DAVX::getOutput (double ratio)
{
    int idx = floor (N * ratio);
    int vecIdx = idx / 4;
    int valIdx = idx % 4;
    
    return ((double*)&u[1][vecIdx])[valIdx];
}

void Object1DAVX::updateStates()
{
    int lengthUVec = static_cast<int> (u.size());
    for (int i = lengthUVec - 1; i > 0; --i)
        u[i] = u[i - 1];
    uNextPtrIdx = (uNextPtrIdx + (lengthUVec - 1)) % lengthUVec;
    u[0] = &uVecs[uNextPtrIdx][0];
    for (int i = 0; i < numAVX; ++i)
        u[0][i] = zeroval;
}

void Object1DAVX::excite()
{
    if (excited)
    {
        excited = false;
        double width = floor((N * 2.0) / 5.0) / 2.0;
        int loc = floor(N / 2.0);
        int startIdx = loc - width / 2.0;
        for (int i = 0; i < width; ++i)
        {
            double val = (1 - cos (2 * double_Pi * i / width)) * 0.5;
            for (int j = 1; j < u.size(); ++j)
                u[j][startIdx + i] = u[j][startIdx + i] + val;
        }
    }
}

void Object1DAVX::setZero()
{
    for (int i = 0; i < uVecs.size(); ++i)
        for (int j = 0; j < numAVX; ++j)
            uVecs[i][j] = zeroval; setZFlag = false;
    
}

Path Object1DAVX::visualiseState()
{
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
//        float newY = u[0][y] * visualScaling + stringBounds;
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
}

void Object1DAVX::changeBoundaryCondition()
{
    BoundaryCondition& boundaryToChange = boundaryConditions[boundaryChangeIdx];
    
    if (boundaryToChange == simplySupported)
        for (int i = 0; i < uVecs.size(); ++i)
        {
            if (boundaryToChange == boundaryConditions[0])
                uVecs[i][0] = _mm256_set_pd (((double*)&uVecs[i][0])[3], ((double*)&uVecs[i][0])[2], 0, 0);
            else
                uVecs[i][numAVX - 1] = _mm256_set_pd (0, 0, ((double*)&uVecs[i][numAVX-1])[1], ((double*)&uVecs[i][numAVX-1])[0]);
            
        }
    
    boundaryToChange = (boundaryToChange == clamped) ? simplySupported : clamped;
    
    BCChangeFlag = false;
    boundaryChangeIdx = -1;
}
