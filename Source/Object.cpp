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
                    std::vector<Equation> termsInput,
                    std::vector<BoundaryCondition> boundaries) : equationString (equationString),
                                                        stencil (stencil),
                                                        h (stencil.getGridSpacing()),
                                                        N (1.0 / h)
{
    // Set boundary conditions
    for (int i = 0; i < boundaries.size(); ++i)
    {
        boundaryConditions.push_back (boundaries[i]);
        boundaryButtons.add (new TextButton ("Bound" + String(i)));
        
        boundaryButtons[i]->setButtonText (boundaries[i] == clamped ? "-" : (i == 0 ? "/" : "\\"));
        boundaryButtons[i]->addListener (this);
        addAndMakeVisible (boundaryButtons[i]);
    }
    
    // Set terms
    terms.reserve (termsInput.size());
    for (int i = 0; i < termsInput.size(); ++i)
        terms.push_back (termsInput[i]);
    
    numTimeSteps = stencil.getTimeSteps();
    
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
}

Object::~Object()
{
}

void Object::buttonClicked (Button* button)
{
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        button->setState (Button::ButtonState::buttonNormal);
        return;
    }
    for (int i = 0; i < boundaryButtons.size(); ++i)
    {
        if (button == boundaryButtons[i])
        {
            BCChangeFlag = true;
            boundaryChangeIdx = i;
            button->setButtonText (boundaryConditions[i] == clamped ? (i == 0 ? "/" : "\\") : "-");
            return;
        }
    }
    
    if (button == muteButton)
    {
        action = muteObject;
    }
    else if (button == editButton)
    {
        action = editObject;
    }
    else if (button == removeButton)
    {
        action = removeObject;
    }
    sendChangeMessage();
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
    {
        showButtons = false;
        return;
    }
    
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

void Object::setApplicationState (ApplicationState applicationState)
{
    switch (applicationState)
    {
        case newObjectState:
        case editObjectState:
            showButtons = false;
            for (auto button : buttons)
            {
                button->setVisible (false);
            }
            for (auto boundaryButton : boundaryButtons)
                boundaryButton->setVisible (true);
            setZeroFlag();
            break;
        case normalAppState:
            for (auto boundaryButton : boundaryButtons)
                boundaryButton->setVisible (false);
            break;
    };
    appState = applicationState;
    resized();
}
