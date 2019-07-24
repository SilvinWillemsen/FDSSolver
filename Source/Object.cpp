/*
  ==============================================================================

    Object.cpp
    Created: 15 Jul 2019 9:52:47am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Object.h"

//==============================================================================
Object::Object (String equationString,
                    Equation stencil,
                    std::vector<Equation> termsInput) : equationString (equationString),
                                                        stencil (stencil),
                                                        h (stencil.getGridSpacing()),
                                                        N (1.0 / h)
{
//#ifdef AVX_SUPPORTED
//    std::cout << "AVX" << std::endl;
//#endif
    terms.reserve (termsInput.size());
    for (int i = 0; i < termsInput.size(); ++i)
        terms.push_back (termsInput[i]);
    
    uVecs.reserve (stencil.getTimeSteps()); //resize according to amount of vectors in stencil
    
    for (int i = 0; i < stencil.getTimeSteps(); ++i)
        uVecs.push_back (std::vector<double> (N, 0));
    

    numTimeSteps = stencil.getTimeSteps();
    u.resize (numTimeSteps);
    
    for (int i = 0; i < u.size(); ++i)
        u[i] = &uVecs[i][0];
    
    stencilIdxStart = (stencil.getStencilWidth() - 1) / 2.0; // should be an integer
    
    // GUI STUFF
    buttons.add (new TextButton("Mute"));
    muteButton = buttons[0];
    
    muteButton->setButtonText ("M");
    muteButton->addListener (this);
    addAndMakeVisible (muteButton);
    
    buttons.add (new TextButton("Edit"));
    editButton = buttons[1];
    
    editButton->setButtonText ("E");
    editButton->addListener (this);
    addAndMakeVisible (editButton);
    
    buttons.add (new TextButton("Remove"));
    removeButton = buttons[2];
    
    removeButton->setButtonText (String (CharPointer_UTF8 ("\xc3\x97")));
    removeButton->addListener (this);
    addAndMakeVisible (removeButton);
    
    boundaryButtons.add (new TextButton ("Left"));
    leftBoundButton = boundaryButtons[0];
    
    leftBoundButton->setButtonText ("-");
    leftBoundButton->addListener (this);
    addAndMakeVisible (leftBoundButton);
    
    boundaryButtons.add (new TextButton ("Right"));
    rightBoundButton = boundaryButtons[1];
    
    rightBoundButton->setButtonText ("-");
    rightBoundButton->addListener (this);
    addAndMakeVisible (rightBoundButton);

}

Object::~Object()
{
}

void Object::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::cyan);
    Path stringPath = visualiseState();
    g.strokePath (stringPath, PathStrokeType(2.0f));
}

Path Object::visualiseState()
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

void Object::resized()
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
    Rectangle<int> boundButtonArea = getLocalBounds().removeFromBottom (GUIDefines::buttonHeight + GUIDefines::margin);
    boundButtonArea.removeFromBottom (GUIDefines::margin);
    leftBoundButton->setBounds (boundButtonArea.removeFromLeft (GUIDefines::buttonWidth / 2.0));
    rightBoundButton->setBounds (boundButtonArea.removeFromRight (GUIDefines::buttonWidth / 2.0));
}

void Object::calculateFDS()
{
    int lowerBound = leftBoundary == simplySupported ? 1 : 2;
    int upperBound = rightBoundary == simplySupported ? N : N - 1;
    for (int l = lowerBound; l < upperBound; ++l)
    {
        u[0][l] = 0;
        for (int j = 0; j < stencil.getStencilWidth(); ++j)
        {
            u[0][l] = u[0][l] - stencil[1][j] * u[1][l - stencilIdxStart + j] - stencil[2][j] * u[2][l - stencilIdxStart + j];
        }
    }
}

void Object::updateStates()
{
    int lengthUVec = static_cast<int>(u.size());
    for (int i = lengthUVec - 1; i > 0; --i)
        u[i] = u[i - 1];
    uNextPtrIdx = (uNextPtrIdx + (lengthUVec - 1)) % lengthUVec;
    u[0] = &uVecs[uNextPtrIdx][0];
}

void Object::excite()
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
void Object::mouseDown (const MouseEvent& e)
{
    if (appState != normalAppState)
        return;
    action = objectClicked;
    sendChangeMessage();
    excited = true;
}

void Object::mouseMove (const MouseEvent &e)
{
    if (appState != normalAppState)
        return;
    
    int dist = sqrt((prevMouseX - e.x) * (prevMouseX - e.x) + (prevMouseY - e.y) * (prevMouseY - e.y));
    if (dist > 10)
    {
        prevMouseX = e.x;
        prevMouseY = e.y;
        startTimer (1000);
    } else {
        return;
    }
    
    if (!showButtons)
    {
        for (auto button : buttons)
        {
            button->setVisible (true);
        }
        showButtons = true;
        resized();
    }
}

void Object::timerCallback()
{
    for (auto button : buttons)
    {
        if (button->isOver())
            return;
    }
    stopTimer();
    for (auto button : buttons)
    {
        button->setVisible (false);
    }
    showButtons = false;
    resized();
}

// LOTS OF THIS FUNCTION IS THE SAME AS THE END OF FDSsolver::solve (...)
void Object::refreshCoefficients()
{
    // reset stencil
    stencil = Equation (stencil.getTimeSteps(), stencil.getStencilWidth());
    
    for (int i = 0; i < terms.size(); ++i)
    {
        Equation term (stencil.getTimeSteps(), stencil.getStencilWidth());
        term = term + terms[i];
        for (int j = 0; j < coefficientTermIndex[i].size(); ++j)
        {
            if (coefficientTermIndex[i][j].isString())
            {
                bool coeffExists = false;
                for (auto coeff : coefficientComponents)
                {
                    if (coeff->getName() == coefficientTermIndex[i][j].toString())
                    {
                        coeffExists = true;
                        term = term * (coeff->getValue());
                        std::cout << "Term " << i << " is multiplied by " << coefficientTermIndex[i][j].toString() << " which has a value of " << coeff->getValue() <<  std::endl;
                    }
                }
                if (!coeffExists)
                    std::cout << coefficientTermIndex[i][j].toString() << " doesn't exist!" << std::endl;
            } else {
                term = term * (static_cast<int> (coefficientTermIndex[i][j]));
            }
        }
        stencil = stencil + term;
    }
    
    // recalculate gridspacing also somewhere
    stencil.setNumPointsFromGridSpacing (h);
    int uNextIdx = (stencil.getStencilWidth() - 1) / 2.0;

    if (!GUIDefines::debug)
        stencil = stencil / (stencil.getUCoeffs(0)[uNextIdx]);
    
    refreshCoefficientsFlag = false;
}

void Object::buttonClicked (Button* button)
{
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        button->setState (Button::ButtonState::buttonNormal);
        return;
    }
    if (button == leftBoundButton)
    {
        BCChangeFlag = true;
        left = true;
        button->setButtonText (leftBoundary == clamped ? "/" : "-");
        return;
    }
    
    if (button == rightBoundButton)
    {
        BCChangeFlag = true;
        left = false;
        button->setButtonText (rightBoundary == clamped ? "\\" : "-");
        return;
    }
    
    if (button == muteButton)
    {
        action = muteObject;
    }
    if (button == editButton)
    {
        action = editObject;
    }
    if (button == removeButton)
    {
        action = removeObject;
    }
    sendChangeMessage();
}

void Object::setApplicationState (ApplicationState applicationState)
{
    switch (applicationState)
    {
        case newObjectState:
        case editObjectState:
            setZeroFlag();
            break;
        case normalAppState:
            break;
    };
    appState = applicationState;
}

void Object::setZero()
 {
     for (int i = 0; i < uVecs.size(); ++i)
         for (int j = 0; j < uVecs[i].size(); ++j)
             uVecs[i][j] = 0; setZFlag = false;
     
 }
void Object::changeBoundaryCondition()
{
    BoundaryCondition& boundaryToChange = left ? leftBoundary : rightBoundary;
    
    if (boundaryToChange == simplySupported)
        for (int i = 0; i < uVecs.size(); ++i)
        {
            int idx = left ? 1 : N - 2;
            uVecs[i][idx] = 0;
        }
    
    boundaryToChange = (boundaryToChange == clamped) ? simplySupported : clamped;
    
    if (left)
        if (leftBoundary == simplySupported)
        {
            std::cout << "leftBoundary is changed to simply supported" << std::endl;
        } else {
            std::cout << "leftBoundary is changed to clamped" << std::endl;
        }
    else if (!left)
    {
        if (rightBoundary == simplySupported)
        {
            std::cout << "rightBoundary is changed to simply supported" << std::endl;
        } else {
            std::cout << "rightBoundary is changed to clamped" << std::endl;
        }
    }
    BCChangeFlag = false;
}
