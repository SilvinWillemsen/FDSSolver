/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    calculator = new Calculator();
    calculator->addChangeListener (this);
    addAndMakeVisible (calculator);
    
    addCoeffWindow = new AddCoefficient();
    
//    coefficientList = new CoefficientList();
    addAndMakeVisible (coefficientList);

    newButton = new TextButton();
    newButton->setButtonText("New Model");
    newButton->addListener (this);
    addAndMakeVisible (newButton);
    
    muteButton = new TextButton();
    muteButton->setButtonText("Mute");
    muteButton->addListener (this);
    addAndMakeVisible (muteButton);
    
    addKeyListener (this);
    
    changeAppState (normalAppState);

    cpuUsage.setColour(Label::textColourId, Colours::white);
    addAndMakeVisible (cpuUsage);
    
    graphicsLabel.setText ("Graphics (Hz): ", dontSendNotification);
    graphicsLabel.setColour (Label::textColourId, Colours::white);
    addAndMakeVisible (graphicsLabel);

    graphicsSlider.setRange (0.1, 60, 0.1);
    graphicsSlider.setValue (15);
    graphicsSlider.addListener (this);
    addAndMakeVisible (graphicsSlider);
    
    modelButtons.add (new TextButton());
    modelButtons[0]->setButtonText("Create String");
    addAndMakeVisible (modelButtons[0]);
    modelButtons[0]->addListener (this);
    
    modelButtons.add (new TextButton());
    modelButtons[1]->setButtonText("Create Mass-Spring");
    addAndMakeVisible (modelButtons[1]);
    modelButtons[1]->addListener (this);
    
    // specify the number of input and output channels that we want to open
    setAudioChannels (2, 2);
    setSize (800, 600);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    stopTimer();
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    fs = sampleRate;
    bufferSize = samplesPerBlockExpected;
    fdsSolver = new FDSsolver (&coefficientList, GUIDefines::debug ? 1.0 : 1.0 / fs);// / fs);
//    equation =
    refresh();
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    for (auto object : objects)
    {
        if (object->needsToBeZero())
            object->setZero();
        if (object->needsCoefficientsRefreshed())
            object->refreshCoefficients();
        if (object->hasBoundaryChanged())
            object->changeBoundaryCondition();
//        if (object->needsToBeRepainted())
        
    }
    
    if (appState != normalAppState)
        return;
    
    float *const channelData1 = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float *const channelData2 = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    
    //    float output{0.0, 0.0};
    for (int i = 0; i < bufferSize; ++i)
    {
        float output = 0.0;
        // for all objects, run their schemes
        for (auto object : objects)
        {
            object->excite();
            object->calculateFDS();
            output = output + object->getOutput (0.5);
            object->updateStates();
        }
        if (!mute)
        {
            channelData1[i] = clip(output);
            channelData2[i] = clip(output);
        }
    }
    
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
}

void MainComponent::resized()
{
    Rectangle<int> totArea = getLocalBounds();

    totArea.reduce (GUIDefines::margin, GUIDefines::margin);
    
    calculator->setBounds(totArea.removeFromTop (GUIDefines::calculatorHeight));
    
    // Draw objects (physical models)
    Rectangle<int> objectArea = getLocalBounds();
    objectArea.removeFromLeft (GUIDefines::margin);
    objectArea.removeFromTop (GUIDefines::buttonHeight + 2 * GUIDefines::margin);
    objectArea.removeFromRight (GUIDefines::buttonAreaWidth + GUIDefines::margin);
    for (int i = 0; i < objects.size(); ++i)
    {
        objects[i]->setBounds(objectArea.removeFromTop(100));
    }
    Rectangle<int> coefficientArea = totArea.removeFromRight (GUIDefines::buttonAreaWidth - GUIDefines::margin);
    coefficientArea.removeFromTop (GUIDefines::margin);
    
    // Draw Coefficients
    coefficientList.setBounds (coefficientArea);
    
    Rectangle<int> lowerButtonArea = getLocalBounds().removeFromBottom(GUIDefines::margin * 2 + GUIDefines::buttonHeight);
    lowerButtonArea.reduce (GUIDefines::margin, GUIDefines::margin);
    newButton->setBounds (lowerButtonArea.removeFromLeft(100));
    lowerButtonArea.removeFromLeft (GUIDefines::margin);
    
    muteButton->setBounds (lowerButtonArea.removeFromRight(GUIDefines::buttonWidth));
    lowerButtonArea.removeFromRight (GUIDefines::margin);
    cpuUsage.setBounds (lowerButtonArea.removeFromLeft(75));
    
    
    graphicsSlider.setBounds (lowerButtonArea.removeFromRight (150));
    lowerButtonArea.removeFromRight (GUIDefines::margin);
    
    graphicsLabel.setBounds(lowerButtonArea.removeFromRight (75));
    int widthPerButton = (lowerButtonArea.getWidth() - GUIDefines::margin) / modelButtons.size();
    lowerButtonArea.removeFromLeft (GUIDefines::margin);
    for (auto modelButton : modelButtons)
    {
        modelButton->setBounds (lowerButtonArea.removeFromLeft(widthPerButton));
        lowerButtonArea.removeFromLeft (GUIDefines::margin);
    }
    
}

void MainComponent::addCoefficient (bool automatic, String nme, double val, bool dyn)
{
    DialogWindow::LaunchOptions dlg;
    int dlgModal = -1;
    if (!automatic)
    {
        addAndMakeVisible (addCoeffWindow);
        dlg.dialogTitle = TRANS (addCoeffWindow->getCoeffPopupState() == normalCoeffState ? "Add Coefficient" : "Edit Coefficient");
        dlg.content.set (addCoeffWindow, false);
        addCoeffWindow->setKeyboardFocus();
        dlgModal = dlg.runModal();
    }
    if (dlgModal == 1 || automatic)
    {
        String coeffName = automatic ? nme : addCoeffWindow->getCoeffName();
        double value = automatic ? val : addCoeffWindow->getValue();
        bool isDynamic = automatic ? dyn : addCoeffWindow->isDynamic();
        
        // if the coefficient already exists
        int coeffIndex = coefficientList.containsCoefficient (coeffName);
        if (coeffIndex >= 0)
        {
            coefficientList.getCoefficients()[coeffIndex].get()->update (isDynamic, value, true);
        } else {
            std::shared_ptr<CoefficientComponent> newCoeff = coefficientList.addCoefficient (coeffName, value, isDynamic);
            newCoeff.get()->addChangeListener (this);
            coefficientList.repaintAndUpdate();
        }

        coefficients.set (coeffName, value);
        
        if (addCoeffWindow->getCoeffPopupState() == normalCoeffState && calculator->getButton ("900")->isEnabled())
        {
            int lim = 3 - coeffName.length();
            for (int i = 0; i < lim; ++i)
            {
                coeffName += "-";
            }
            calculator->addToEquation (coeffName + "_");
        } else {
            addCoeffWindow->setCoeffPopupState (normalCoeffState);
        }
        
        resized();
    }
    
    // always set the dynamic tickbox of the addCoeffWindow to false
    addCoeffWindow->setDynamic (false);
}

void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == calculator)
    {
        switch (calculator->getChangeMessage()) {
            case addCoeffMessage:
                addCoefficient();
                break;
            case createPMMessage:
                appState == normalAppState ? createPhysicalModel() : editPhysicalModel();
                resized();
                break;
            default:
                break;
        }
        
        return;
    }
    
    // if (source == instrument)
    for (auto object : objects)
    {
        if (source == object)
        {
            switch (object->getAction()) {
                case muteObject:
                    object->setZeroFlag();
                    break;
                case editObject:
                    calculator->setEquationString (object->getEquationString());
                    coefficientList.loadCoefficientsFromObject (object->getCoefficientComponents());
                    editingObject = object;
                    changeAppState (editObjectState);
                    break;
                    
                case removeObject:
                    objects.removeObject (object);
                    calculator->clearEquation();
                    coefficientList.emptyCoefficientList();
                    if (appState != normalAppState)
                        changeAppState (normalAppState);
                    break;
                    
                case objectClicked:
                    currentlySelectedObject = object;
                    calculator->setEquationString (object->getEquationString());
                    repaint();
                    coefficientList.loadCoefficientsFromObject (object->getCoefficientComponents());
                    break;
                default:
                    break;
            }
        }
    }
    
    // if (source == coefficientList)
    for (auto coefficientComponent : coefficientList.getCoefficients())
    {
        if (source == coefficientComponent.get())
        {
            String name = coefficientComponent->getName();
            switch (coefficientComponent->getAction()) {
                case insertCoeff:
                    if (calculator->getButton("coeff")->isEnabled())
                        calculator->addToEquation (calculator->encoder (name) + "_");
                    break;
                    
                case editCoeff:
                    addCoeffWindow->setCoeffPopupState (editingCoeff);
                    addCoeffWindow->setCoeffName (coefficientComponent->getName());
                    addCoeffWindow->setCoeffValue (coefficientComponent->getValue());
                    addCoefficient();
                    break;
                    
                case removeCoeff:
                    coefficientList.removeCoefficient (coefficientComponent);
                    
                    // remove coefficient values from object
                    coefficients.remove (coefficientComponent->getName());
                    coefficientList.repaintAndUpdate();
                    break;
                    
                case sliderMoved:
                    if (currentlySelectedObject != nullptr)
                        currentlySelectedObject->setCoefficient (coefficientComponent->getName(), coefficientComponent->getSliderValue());
                    break;
                    
                case caughtReturnKey:
                    calculator->buttonClicked (calculator->getButton("createPM"));
                    break;
                default:
                    break;
            }
        }
    }
    refresh();
}

bool MainComponent::createPhysicalModel()
{
    String equationString = calculator->getEquationString();
    int amountOfTimeSteps = fdsSolver->getStencilWidth (equationString, false);
    int stencilWidth = fdsSolver->getStencilWidth (equationString, true);
    Equation eq (amountOfTimeSteps, stencilWidth);
    
    Array<var> coefficientTermIndex;
    std::vector<Equation> terms;
    if (fdsSolver->solve (equationString, eq, &coefficients, coefficientTermIndex, terms))
    {
        Object* newObject;
        int editedObjectIdx = objects.indexOf (editingObject);
        
        // create 0D object
        if (stencilWidth == 1)
        {
            if (appState != editObjectState)
                newObject = new Object0D (equationString, eq, terms);
            else
                newObject = objects[editedObjectIdx];
        }
        else
        {
            for (int i = 0; i < (eq.getNumPoints() + 3) / 4; ++i)
                testVec[i] = _mm256_setr_pd (0.0, 0.0, 0.0, 0.0);
            testVec.erase (testVec.begin() + (eq.getNumPoints() + 3) / 4, testVec.end());
            
            // edit object
    #ifdef AVX_SUPPORTED
                newObject = new Object1DAVX (equationString, eq, terms, testVec);
    #else
                newObject = new Object1D (equationString, eq, terms);
    #endif
        }
        
        // only add the coefficients that are actually used by the newly created object
        StringArray usedCoeffs = fdsSolver->getUsedCoeffs (equationString);
        NamedValueSet currentCoefficients = coefficientList.getNamedValueSet (usedCoeffs);
        newObject->setCoefficients (currentCoefficients);
        
        for (auto coeffComp : coefficientList.getCoefficients())
            if (currentCoefficients.contains(coeffComp.get()->getName()))
                newObject->setCoefficientComponent (coeffComp);

        newObject->setCoefficientTermIndex (coefficientTermIndex);
        newObject->refreshCoefficients();
        newObject->createUpdateEq();
        newObject->addChangeListener (this);
        if (appState == editObjectState)
            objects.set(editedObjectIdx, newObject);
        else
            objects.add(newObject);
        
        addAndMakeVisible (newObject);
        changeAppState (normalAppState);
    } else {
        return false;
    }
    
    return true;
}

bool MainComponent::editPhysicalModel()
{
    if (!createPhysicalModel())
        return false;
    editingObject = nullptr;
    return true;
}
    
void MainComponent::buttonClicked (Button* button)
{
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        button->setState (Button::ButtonState::buttonNormal);
        calculator->buttonClicked (calculator->getButton("createPM"));
        return;
    }
    
    
    if (button == newButton)
    {
        if (appState == normalAppState)
        {
            changeAppState (newObjectState);
        }
        else if (appState == newObjectState) // new model has been cancelled
        {
            coefficientList.emptyCoefficientList();
            changeAppState (normalAppState);
        }
    }
    
    if (button == muteButton)
    {
        mute = !mute;
    }
    
    if (button == modelButtons[0])
    {
        createString();
    }
    
    if (button == modelButtons[1])
    {
        createMassSpring();
    }
    
}

void MainComponent::sliderValueChanged (Slider* slider)
{
    if (slider == &graphicsSlider)
        startTimerHz (slider->getValue());
}

void MainComponent::refresh()
{
    resized();
}

void MainComponent::timerCallback()
{
    for (int i = 0; i < objects.size(); ++i)
    {
        objects[i]->repaint();
    }
    double cpu = static_cast<int>(deviceManager.getCpuUsage() * 1000.0) / 10.0;
    cpuUsage.setText ("CPU: " + String (cpu) + " %", dontSendNotification);
    
}

double MainComponent::clip (double output, double min, double max)
{
    if (output > max)
    {
        return output = max;
    }
    else if (output < min)
    {
        return output = min;
    }
    return output;
}

bool MainComponent::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    String buttonToClick;
    if (appState == normalAppState)
        return false;
    
    // if not a capital letter
    if (!(ModifierKeys::getCurrentModifiers() == ModifierKeys::shiftModifier && CharacterFunctions::isLetter (key.getTextCharacter())))
    {
        switch (key.getTextCharacter())
        {
            case 12:
                buttonToClick = "createPM";
                break;
            case '=':
                buttonToClick = StringCode::getEncoded()[0];
                break;
            case '+':
                buttonToClick = StringCode::getEncoded()[1];
                break;
            case '-':
                //assuming that either the "-" or "(-)" is enabled
                if (calculator->getButton("102")->isEnabled())
                    buttonToClick = StringCode::getEncoded()[2];
                else
                    buttonToClick = StringCode::getEncoded()[12];
                break;
            case 'g':
                buttonToClick = StringCode::getEncoded()[3];
                break;
            case 'h':
                buttonToClick = StringCode::getEncoded()[4];
                break;
            case 'j':
                buttonToClick = StringCode::getEncoded()[5];
                break;
            case 'k':
                buttonToClick = StringCode::getEncoded()[6];
                break;
            case 'v':
                buttonToClick = StringCode::getEncoded()[7];
                break;
            case 'b':
                buttonToClick = StringCode::getEncoded()[8];
                break;
            case 'n':
                buttonToClick = StringCode::getEncoded()[9];
                break;
            case 'm':
                buttonToClick = StringCode::getEncoded()[10];
                break;
            case 'u':
                buttonToClick = StringCode::getEncoded()[11];
                break;
            case 127:
                buttonToClick = "backspace";
                break;
            default:
                buttonToClick = "NOBUTTON";
                break;
        }
        
        if (buttonToClick != "NOBUTTON")
            if (calculator->getButton (buttonToClick)->isEnabled())
                calculator->buttonClicked (calculator->getButton (buttonToClick));
    }
    else
    {
        String keyString;
        keyString += (juce_wchar) key.getKeyCode();

        if (coefficientList.containsCoefficient (keyString) >= 0)
        {
            if (calculator->getButton ("900")->isEnabled())
            {
                for (auto coefficientComponent : coefficientList.getCoefficients())
                    if (coefficientComponent->getName() == keyString)
                    {
                        coefficientComponent->clickCoeffButton();
                        return true;
                    }
            } else {
                std::cout << "Coeffbutton is disabled" << std::endl;
                return false;
            }
                
        }
        addCoeffWindow->setCoeffName (keyString);
        addCoefficient();
    }
    return true;
}

// Linking the return key to the createPM button
bool MainComponent::keyStateChanged (bool isKeyDown, Component* originatingComponent)
{
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        calculator->returnKeyIsDown = true;
        if (!calculator->createPMalreadyClicked)
            calculator->buttonClicked (calculator->getButton("createPM"));
    }
    else if (calculator->returnKeyIsDown) // this means that the return key is released
    {
        calculator->createPMalreadyClicked = false;
    }
    return true;
}

void MainComponent::changeAppState (ApplicationState applicationState)
{
    currentlySelectedObject = nullptr;
    
    switch (applicationState) {
        case newObjectState:
            newButton->setEnabled (true);
            newButton->setButtonText ("CANCEL");
            break;
        case normalAppState:
            newButton->setEnabled (true);
            newButton->setButtonText ("New Model");
            break;
        case editObjectState:
            newButton->setEnabled (false);
            break;
        default:
            break;
    }
    coefficientList.setApplicationState (applicationState);
    calculator->setApplicationState (applicationState);
    for (auto object : objects)
        object->setApplicationState (applicationState);
        
    appState = applicationState;
}

void MainComponent::createString()
{
    addCoefficient (true, "T", 200000, false);
    calculator->setEquationString("203_300_100_T--_207_300_");
    createPhysicalModel();
    resized();
}

void MainComponent::createMassSpring()
{
    addCoefficient (true, "W", 2000000, false);
    calculator->setEquationString("203_300_100_901_W--_300_");
    createPhysicalModel();
    resized();
}
