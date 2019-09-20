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
                int numBoundaries, int numObject) : stencil (stencil),
                                                    equationString (equationString),
                                                    h (stencil.getGridSpacing()),
                                                    N (1.0 / h),
                                                    numObject (numObject)
{
    // Set boundary conditions
    std::vector<BoundaryCondition> boundaries (numBoundaries, clamped);
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
    
    stencilIdxStart = (stencil.getStencilWidth() - 1) / 2.0; // should be an integer
    
    // GUI STUFF
    buttons.add (new TextButton("Mute"));
    buttons[0]->setButtonText ("M");
    buttons[0]->addListener (this);
    addAndMakeVisible (buttons[0]);
    
    buttons.add (new TextButton("Edit"));
    buttons[1]->setButtonText ("E");
    buttons[1]->addListener (this);
    addAndMakeVisible (buttons[1]);
    
    buttons.add (new TextButton("Remove"));
    buttons[2]->setButtonText (String (CharPointer_UTF8 ("\xc3\x97")));
    buttons[2]->addListener (this);
    addAndMakeVisible (buttons[2]);
}

Object::~Object()
{
}

void Object::buttonClicked (Button* button)
{
    // Caputure return key
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        button->setState (Button::ButtonState::buttonNormal);
        return;
    }
    
    // Boundary buttons
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
    
    // Mute, edit and remove buttons
    if (button == buttons[0])
    {
        action = muteObject;
    }
    else if (button == buttons[1])
    {
        action = editObject;
    }
    else if (button == buttons[2])
    {
        action = removeObject;
    }
    sendChangeMessage();
}

void Object::mouseDown (const MouseEvent& e)
{
    if (appState != normalAppState)
        return;
    xLoc = e.x;
    yLoc = e.y;
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
    stencilVectorForm.clear();
    stencilVectorForm.shrink_to_fit();
    
    for (int i = 0; i < terms.size(); ++i)
    {
        Equation term = terms[i];
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
    
    curTimeStep.resize (stencil.getStencilWidth(), 0.0);
    prevTimeStep.resize (stencil.getStencilWidth(), 0.0);
    for (int j = 0; j < stencil.getStencilWidth(); ++j)
    {
        curTimeStep[j] = stencil.getUCoeffAt(1, j);
        prevTimeStep[j] = stencil.getUCoeffAt(2, j);
    }
    isSymmetric = stencil.isSymmetric();
    
    stencilVectorForm.reserve (stencil.getTimeSteps() * stencil.getStencilWidth());
    for (int j = 1; j < stencil.getTimeSteps(); ++j)
        for (int i = 0; i < stencil.getStencilWidth(); ++i)
            stencilVectorForm.push_back(stencil.getUCoeffAt(j, i));
            
    stencilVectorForm.shrink_to_fit();
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

void Object::updateEqGenerator (String& updateEqString)
{
    void *handle;
    char *error;
    std::hash<int64> hasher;
    auto newName = hasher (juce::Time::getCurrentTime().toMilliseconds());
    
    // convert updateEqString to char
    const char* eq = static_cast<const char*>(updateEqString.toUTF8());

    FILE *fd= fopen("code.c", "w");
    
    fprintf(fd, "#include <stdio.h>\n"
                "void updateEq(double* uNext, double* u, double* uPrev, double* coeffs)\n"
                "{\n"
                "%s\n"
                "}", eq);
    fclose(fd);
    
    String systemInstr;
    
    // if this object is new (has not been edited yet) delete the previous file
    if (curName != 0)
    {
        removeFiles (curName);
    }
    curName = newName;
    systemInstr = String ("clang -shared -undefined dynamic_lookup -O3 -o " + String (curName) + ".so code.c -g");
    system (toConstChar (systemInstr));
    handle = dlopen (toConstChar (String (String (curName) + ".so")), RTLD_LAZY);
    
//    String originalName = String("generated" + String(numObject) + ".so");
//    String fileToGenerate;
//    String systemInstr;
//
//    // if the original name (fx. generated1.so) already exists, remove this file and add the edited name (fx. generated1E.so)
//    if (access( toConstChar(originalName), F_OK ) != -1)
//    {
//        fileToGenerate = String("generated" + String(numObject) + "E.so");
//        systemInstr = static_cast<const char*> (String ("rm " + String (originalName) + "\n rm -R " + String (originalName) + ".dSYM").toUTF8());
//        system(toConstChar (systemInstr));
//
//    } else {
//        // otherwise, use the original name to create a file (fx. generated1.so)
//        fileToGenerate = originalName;
//
//        // if an edited file already exists (fx. if the model has been edited twice), remove the edited file
//        String editFileName = String("generated" + String(numObject) + "E.so");
//        if (access( toConstChar (editFileName), F_OK ) != -1)
//        {
//            String editedName = static_cast<const char*>(String("generated" + String (numObject) + "E.so").toUTF8());
//            systemInstr = String("rm " + editedName + "\n rm -R " + editedName + ".dSYM").toUTF8();
//            system(toConstChar (systemInstr));
//        }
//    }
//
//    systemInstr = String("clang -shared -undefined dynamic_lookup -O3 -o " + String (fileToGenerate) + " code.c -g");
//    system(toConstChar (systemInstr));
//    handle = dlopen (toConstChar (fileToGenerate), RTLD_LAZY);
//
    if (!handle)
    {
        fprintf (stderr, "%s\n", dlerror());
        exit(1);
    }
    
    dlerror();    /* Clear any existing error */
    
    *(void **)(&updateEq) = dlsym (handle, "updateEq"); // second argument finds function name
    
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s\n", error);
        exit(1);
    }
}
