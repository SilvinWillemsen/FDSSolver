/*
  ==============================================================================

    Object1D.cpp
    Created: 24 Jul 2019 3:48:59pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Object1D.h"

//==============================================================================
Object1D::Object1D (String equationString, Equation stencil, std::vector<Equation> terms, std::vector<BoundaryCondition> boundaries) : Object (equationString, stencil, terms, boundaries)
{
    uVecs.reserve (stencil.getTimeSteps()); //resize according to amount of vectors in stencil
    
    for (int i = 0; i < stencil.getTimeSteps(); ++i)
        uVecs.push_back (std::vector<double> (N, 0));
    
    u.resize (numTimeSteps);
    
    for (int i = 0; i < u.size(); ++i)
        u[i] = &uVecs[i][0];
}

Object1D::~Object1D()
{
}

void Object1D::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::cyan);
    Path stringPath = visualiseState();
    g.strokePath (stringPath, PathStrokeType(2.0f));
}

void Object1D::resized()
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

void Object1D::calculateFDS()
{
    int lowerBound = boundaryConditions[0] == simplySupported ? 1 : 2;
    int upperBound = boundaryConditions[1] == simplySupported ? N : N - 1;
    for (int l = lowerBound; l < upperBound; ++l)
    {
        u[0][l] = 0;
        for (int j = 0; j < stencil.getStencilWidth(); ++j)
        {
            u[0][l] = u[0][l] - stencil[1][j] * u[1][l - stencilIdxStart + j] - stencil[2][j] * u[2][l - stencilIdxStart + j];
        }
    }
}

void Object1D::updateStates()
{
    int lengthUVec = static_cast<int>(u.size());
    for (int i = lengthUVec - 1; i > 0; --i)
        u[i] = u[i - 1];
    uNextPtrIdx = (uNextPtrIdx + (lengthUVec - 1)) % lengthUVec;
    u[0] = &uVecs[uNextPtrIdx][0];
}


void Object1D::excite()
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

void Object1D::setZero()
{
    for (int i = 0; i < uVecs.size(); ++i)
        for (int j = 0; j < uVecs[i].size(); ++j)
            uVecs[i][j] = 0; setZFlag = false;
    
}

Path Object1D::visualiseState()
{
    auto stringBounds = getHeight() / 2.0;
    Path stringPath;
    stringPath.startNewSubPath (0, stringBounds);
    int stateWidth = (showButtons ? GUIDefines::horStateArea : 1.0) * getWidth();
    auto spacing = stateWidth / static_cast<double>(N);
    auto x = spacing;
    
    for (int y = 1; y < N; y++)
    {
        int visualScaling = 10;
        float newY = u[0][y] * visualScaling + stringBounds;
        
        if (isnan(x) || isinf(abs(x) || isnan(newY) || isinf(abs(newY))))
        {
            std::cout << "Wait" << std::endl;
        };
        
        if (isnan(newY))
            newY = 0;
        stringPath.lineTo(x, newY);
        x += spacing;
    }
    stringPath.lineTo(stateWidth, stringBounds);
    return stringPath;
}

void Object1D::changeBoundaryCondition()
{
    BoundaryCondition& boundaryToChange = boundaryConditions[boundaryChangeIdx];
    
    if (boundaryToChange == simplySupported)
        for (int i = 0; i < uVecs.size(); ++i)
        {
            int idx = boundaryConditions[0] ? 1 : N - 2;
            uVecs[i][idx] = 0;
        }
    
    boundaryToChange = (boundaryToChange == clamped) ? simplySupported : clamped;
    
    BCChangeFlag = false;
    boundaryChangeIdx = -1;
}
