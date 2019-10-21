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
Object1DAVX::Object1DAVX (String equationString,
                          Equation stencil,
                          std::vector<Equation> terms,
                          std::vector<__m256d> testVec) : Object (equationString, stencil, terms, 2),
                                                          numAVX ((N + GUIDefines::AVXnum - 1) / static_cast<double> (GUIDefines::AVXnum))
{
    uVecsAVX = std::vector<std::vector<__m256d>> (std::vector<std::vector<__m256d>>(stencil.getTimeSteps(), testVec));
    
    uVecs.assign (stencil.getTimeSteps(), std::vector<double> (N, 0)); //
    
    u.assign (stencil.getTimeSteps(), nullptr);
    for (int i = 0; i < stencil.getStencilWidth(); ++i)
        for (int j = 0; j < stencil.getTimeSteps(); ++j)
            u[j] = &uVecsAVX[j][0];

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

void Object1DAVX::calculateFDS()
{

    // fill up AVX vector
    int startIdx;

    for (int k = 0; k < numAVX; ++k)
    {
        // zero out the next vectors
        uVecsAVX[uNextPtrIdx][k] = _mm256_set_pd (0.0, 0.0, 0.0, 0.0);
    }

    for (int l = 0; l < numAVX; ++l)
    {
        for (int j = 0; j < stencil.getStencilWidth(); ++j)
        {
            startIdx = j - stencilIdxStart + 4 * l;
            u[0][l] = _mm256_sub_pd (_mm256_sub_pd (u[0][l],
                                     stencil[1][j] *
                                     _mm256_setr_pd ((l == 0 ? 0 : (*u[1])[startIdx]),
                                                     (*u[1])[startIdx + 1],
                                                     (*u[1])[startIdx + 2],
                                                     (l == numAVX - 1 ? 0 : (*(u[1]))[startIdx + 3]))),
                                     stencil[2][j] *
                                     _mm256_setr_pd ((l == 0 ? 0 : (*(u[2]))[startIdx]),
                                                     (*u[2])[startIdx + 1],
                                                     (*u[2])[startIdx + 2],
                                                     (l == numAVX - 1 ? 0 : (*(u[2]))[startIdx + 3])));
            
        }
        if (l == 0)
        {
            u[0][l] = _mm256_setr_pd (0.0, 0.0, (*u[0])[2], (*u[0])[3]);
        }
        
        if (N % 4 == 1 && l == numAVX - 2)
        {
            u[0][l] = _mm256_setr_pd((*(u[0]))[N - 3], (*u[0])[N - 2], (*u[0])[N - 1], 0.0);
            u[0][l + 1] = _mm256_setr_pd(0.0, 0.0, 0.0, 0.0);
        }
        else if (N % 4 == 2 && l == numAVX - 1)
        {
            u[0][l] = _mm256_setr_pd(0.0, 0.0, 0.0, 0.0);
        }
        else if (N % 4 == 3 && l == numAVX - 1)
        {
            u[0][l] = _mm256_setr_pd((*u[0])[N - 1], 0.0, 0.0, 0.0);
        }
        else if (N % 4 == 0 && l == numAVX - 1)
        {
            u[0][l] = _mm256_setr_pd((*u[0])[N - 2], (*u[0])[N - 1], 0.0, 0.0);
        }
    }
}

double Object1DAVX::getOutput (double ratio, double unUsed)
{
    int idx = floor (N * ratio);
    return (*u[1])[idx];
}

void Object1DAVX::updateStates()
{
    int lengthUVec = static_cast<int> (u.size());
    uNextPtrIdx = (uNextPtrIdx + (lengthUVec - 1)) % lengthUVec;

    for (int i = lengthUVec - 1; i > 0; --i)
    {
        u[i] = u[i - 1];
    }
    u[0] = &uVecsAVX[uNextPtrIdx][0];
}

void Object1DAVX::excite()
{
    if (excited)
    {
        excited = false;
        int width = floor((N * 2.0) / 5.0) / 2.0;
        int loc = floor(N * static_cast<float>(getXLoc()) / static_cast<float>(getWidth()));
        int startIdx = clamp (loc - width / 2.0, simplySupported ? 1 : 2, simplySupported ? N-1-width : N-2-width);
        int vectIdx = startIdx / 4;
        int inVectIdx = startIdx % 4 - 1;
        std::vector<double> AVXvect (4, 0.0);
        for (int i = 0; i < width; ++i)
        {
            int idx = (i + inVectIdx) % 4;
            AVXvect[idx] = (1 - cos (2 * double_Pi * i / width)) * 0.5;
            if (idx == 3 || i == width - 1)
            {
                for (int j = 1; j < stencil.getTimeSteps(); ++j)
                {
                    uVecsAVX[j][vectIdx] = _mm256_setr_pd(AVXvect[0], AVXvect[1], AVXvect[2], AVXvect[3]);
                }
                AVXvect = std::vector<double> (4, 0.0);
                ++vectIdx;
            }
        }
        std::cout << "wait" << std::endl;
    }
    
}

void Object1DAVX::setZero()
{
    for (int j = 0; j < stencil.getTimeSteps(); ++j)
        for (int k = 0; k < numAVX; ++k)
            uVecsAVX[j][k] = _mm256_set_pd(0.0, 0.0, 0.0, 0.0); setZFlag = false;
    
    std::cout << "Should be zero" << std::endl;
    
}

Path Object1DAVX::visualiseState()
{
    auto stringBounds = getHeight() / 2.0;
    Path stringPath;
    stringPath.startNewSubPath (0, stringBounds);
    int stateWidth = (showButtons ? GUIDefines::horStateArea : 1.0) * getWidth();
    auto spacing = stateWidth / static_cast<double>(N - 2);
    auto x = spacing;

    for (int y = 1; y < N - 1; y++) //needs to be different with free conditions
    {
        double visualScaling = 10;
        float newY = (*u[1])[y] * visualScaling + stringBounds;

        if (isnan(x) || isinf(abs(x) || isnan(newY) || isinf(abs(newY))))
        {
            std::cout << "Wait" << std::endl;
        }
        
        if (isnan(newY))
            newY = 0;
        stringPath.lineTo(x, newY);
        x += spacing;
    }
    stringPath.lineTo(stateWidth, stringBounds);
    return stringPath;
}

void Object1DAVX::changeBoundaryCondition()
{
    BoundaryCondition& boundaryToChange = boundaryConditions[boundaryChangeIdx];
    
    if (boundaryToChange == simplySupported)
        for (int i = 0; i < uVecs.size(); ++i)
        {
            if (boundaryToChange == boundaryConditions[0])
                uVecs[i][0] = 0;
            else
                uVecs[i][numAVX - 1] = 0;
            
        }
    
    boundaryToChange = (boundaryToChange == clamped) ? simplySupported : clamped;
    
    BCChangeFlag = false;
    boundaryChangeIdx = -1;
}
