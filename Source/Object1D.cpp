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
Object1D::Object1D (String equationString, Equation stencil, double h) : equationString (equationString),
                                                               stencil (stencil),
                                                               h (h),
                                                               N (1.0 / h)
{
    uVecs.resize (3); //resize according to amount of vectors in stencil
    
    for (int i = 0; i < uVecs.size(); ++i)
        uVecs[i] = std::vector<double> (N, 0);
    
    numTimeSteps = stencil.getTimeSteps();
    u.resize (numTimeSteps);
    
    for (int i = 0; i < u.size(); ++i)
        u[i] = &uVecs[i][0];
    
    stencilIdxStart = (stencil.getStencilWidth() - 1) / 2.0; // should be an integer
    
    // GUI STUFF
    
    editButton.setName ("Edit");
    editButton.setButtonText ("E");
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
    Path stringPath = visualiseState();
    g.strokePath (stringPath, PathStrokeType(2.0f));
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

void Object1D::resized()
{
    Rectangle<int> buttonArea = getLocalBounds();
    buttonArea.removeFromLeft (GUIDefines::horStateArea * getWidth());
    buttonArea.removeFromLeft (GUIDefines::margin);
    buttonArea.removeFromRight (GUIDefines::margin);
    
    editButton.setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
    buttonArea.removeFromTop (GUIDefines::margin);
    removeButton.setBounds(buttonArea.removeFromTop (GUIDefines::buttonHeight));
}

void Object1D::calculateFDS()
{
    for (int l = 2; l < N - 1; ++l)
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
void Object1D::mouseDown (const MouseEvent& e)
{
    action = objectClicked;
    sendChangeMessage();
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
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        button->setState (Button::ButtonState::buttonNormal);
        return;
    }
    
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
