/*
  ==============================================================================

    Object1D.cpp
    Created: 15 Jul 2019 9:52:47am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Object1D.h"

//==============================================================================
Object1D::Object1D (String equationString, std::vector<std::vector<double>> stencil, NamedValueSet* coefficients, int N) : equationString (equationString),
                                                                                                                           stencil (stencil),
                                                                                                                           N (N),
                                                                                                                           coefficients (coefficients)
{
    
    uVecs.resize (3); //resize according to amount of vectors in stencil
    
    for (int i = 0; i < uVecs.size(); ++i)
        uVecs[i] = std::vector<double> (N, 0);
    
    uNext = &uVecs[0][0];
    u = &uVecs[1][0];
    uPrev = &uVecs[2][0];
    
    stencilIdxStart = (stencil[0].size() - 1) / 2.0; // should be an integer
    
    // GUI STUFF
    
    editButton.setName ("Edit");
    editButton.setButtonText ("Edit");
    editButton.addListener (this);
    addAndMakeVisible (editButton);
    
    removeButton.setName ("Remove");
    removeButton.setButtonText (String (CharPointer_UTF8 ("\xc3\x97")));
    removeButton.addListener (this);
    addAndMakeVisible (removeButton);
    
}

Object1D::~Object1D()
{
}

void Object1D::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::cyan);
    g.strokePath(visualiseState(), PathStrokeType(2.0f));
}

Path Object1D::visualiseState()
{
    auto stringBounds = getHeight() / 2.0;
    Path stringPath;
    stringPath.startNewSubPath(0, stringBounds);
    int stateWidth = GUIDefines::horStateArea * getWidth();
    auto spacing = stateWidth / static_cast<double>(N - 1);
    auto x = spacing;
    
    for (int y = 1; y < N-1; y++)
    {
        int visualScaling = 1;
        float newY = uNext[y] * visualScaling + stringBounds;
        if (isnan(newY))
            newY = 0;
        stringPath.lineTo(x, newY);
        x += spacing;
    }
    stringPath.lineTo(stateWidth, stringBounds);
    return stringPath;
}

void Object1D::resized()
{
    Rectangle<int> buttonArea = getLocalBounds();
    buttonArea.removeFromLeft (GUIDefines::horStateArea * getWidth());
    buttonArea.removeFromLeft (GUIDefines::margin);
    buttonArea.removeFromRight (GUIDefines::margin);
    
    editButton.setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
    removeButton.setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
}

void Object1D::calculateFDS()
{
    for (int l = 2; l < N - 1; ++l)
    {
        uNext[l] = 0;
        for (int j = 0; j < stencil[0].size(); ++j)
        {
            uNext[l] = uNext[l] - stencil[1][j] * u[l - stencilIdxStart + j] - stencil[2][j] * uPrev[l - stencilIdxStart + j];
        }
    }
//    std::cout << std::endl;
}

void Object1D::updateStates()
{
    uPrev = u;
    u = uNext;
    
    uNextPtrIdx = (uNextPtrIdx + 2) % 3;
    uNext = &uVecs[uNextPtrIdx][0];
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
            u[startIdx + i] = u[startIdx + i] + val;
            uPrev[startIdx + i] =  uPrev[startIdx + i] + val;
        }
    }
    
}
void Object1D::mouseDown (const MouseEvent& e)
{
    excited = true;
}

void Object1D::refreshCoefficients()
{
    for (int i = 0; i < coefficientTermIndex.size(); ++i)
    {
        for (int j = 0; j < coefficientTermIndex[i].size(); ++j)
        {
            std::cout << "Term " << i << " is multiplied by " << coefficientTermIndex[i][j].toString() << std::endl;
        }
    }
}

void Object1D::buttonClicked (Button* button)
{
    if (button == &editButton)
    {
        action = editObject;
    }
    if (button == &removeButton)
    {
        action = removeObject;
    }
    
    sendChangeMessage();
}
