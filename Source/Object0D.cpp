/*
  ==============================================================================

    Object0D.cpp
    Created: 16 Sep 2019 1:18:30pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Object0D.h"

//==============================================================================
Object0D::Object0D (String equationString, Equation stencil, std::vector<Equation> terms, int numObject) : Object (equationString, stencil, terms, 0, numObject)
{
    uVecs.reserve (stencil.getTimeSteps()); //resize according to amount of vectors in stencil
    
    for (int i = 0; i < stencil.getTimeSteps(); ++i)
        uVecs.push_back (0);
    
    u.resize (stencil.getTimeSteps());
    
    for (int i = 0; i < u.size(); ++i)
        u[i] = &uVecs[i];
    
}

Object0D::~Object0D()
{
}

void Object0D::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::cyan);
    g.drawEllipse(getWidth() / 2.0, u[0][0] + getHeight() / 2.0, 10, 10, 5);
}

void Object0D::resized()
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
    }
}

void Object0D::calculateFDS()
{
    u[0][0] = -stencil[1][0] * u[1][0] - stencil[2][0] * u[2][0];
}

void Object0D::updateStates()
{
    int lengthUVec = static_cast<int>(u.size());
    for (int i = lengthUVec - 1; i > 0; --i)
        u[i] = u[i - 1];
    uNextPtrIdx = (uNextPtrIdx + (lengthUVec - 1)) % lengthUVec;
    u[0] = &uVecs[uNextPtrIdx];
}


void Object0D::setZero()
{
    for (int i = 0; i < uVecs.size(); ++i)
        uVecs[i] = 0; setZFlag = false;
    
}

void Object0D::excite()
{
    if (excited)
    {
        excited = false;
        u[1][0] += 1.5;
        u[2][0] += 1.5;
    }
}
