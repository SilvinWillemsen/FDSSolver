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
    calculator->getButton("createPM")->addShortcut (KeyPress (KeyPress::returnKey));
    
    addCoeffWindow = new AddCoefficient();
    
//    coefficientList = new CoefficientList();
    addAndMakeVisible (coefficientList);

    newButton = new TextButton();
    newButton->setButtonText("New Model");
    newButton->addListener (this);
    addAndMakeVisible (newButton);
    
    addKeyListener (this);
    
    changeAppState (normalAppState);
    setSize (800, 600);

    // specify the number of input and output channels that we want to open
    setAudioChannels (2, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    Timer::stopTimer();
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
    Timer::startTimerHz (120);
//    equation =
    refresh();
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
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
        channelData1[i] = clip(output);
        channelData2[i] = clip(output);
    }
    for (auto object : objects)
        if (object->needsToBeZero())
            object->setZero();
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

    totArea.removeFromTop(GUIDefines::margin);
    totArea.removeFromLeft(GUIDefines::margin);
    totArea.removeFromRight(GUIDefines::margin);
    
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
    
    newButton->setBounds (GUIDefines::margin, getHeight() - GUIDefines::margin - GUIDefines::buttonHeight, 100, GUIDefines::buttonHeight);
}

void MainComponent::addCoefficient()
{
    addAndMakeVisible (addCoeffWindow);
    DialogWindow::LaunchOptions dlg;
    dlg.dialogTitle = TRANS (addCoeffWindow->getCoeffPopupState() == normalCoeffState ? "Add Coefficient" : "Edit Coefficient");
    dlg.content.set (addCoeffWindow, false);
    addCoeffWindow->setKeyboardFocus();
    
    if (dlg.runModal() == 1)
    {
        String coeffName = addCoeffWindow->getCoeffName();
        double value = addCoeffWindow->getValue();
        bool isDynamic = addCoeffWindow->isDynamic();
        
        // if the coefficient already exists
        int coeffIndex = coefficientList.containsCoefficient (coeffName);
        if (coeffIndex >= 0)
        {
            coefficientList.getCoefficients()[coeffIndex]->update (isDynamic, value);
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
                case editObject:
                    calculator->setEquationString (object->getEquationString());
                    calculator->getButton ("createPM")->setButtonText ("Edit");
                    editingObject = object;
                    editingObject->setZeroFlag();
                    changeAppState (editObjectState);
                    break;
                    
                case removeObject:
                    objects.removeObject (object);
                    coefficientList.emptyCoefficientList();
                    break;
                    
                case objectClicked:
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
                    calculator->addToEquation (calculator->encoder (name) + "_");
                    break;
                    
                case editCoeff:
                    addCoeffWindow->setCoeffPopupState (editingCoeff);
                    addCoeffWindow->setCoeffName (coefficientComponent->getName());
                    addCoefficient();
                    break;
                    
                case removeCoeff:
                    coefficientList.removeCoefficient (coefficientComponent);
                    
                    // remove coefficient values from object
                    coefficients.remove (coefficientComponent->getName());
                    coefficientList.repaintAndUpdate();
                    break;
                    
                case sliderMoved:
                    coefficients.set (name, coefficientComponent->getSliderValue());
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
    if (fdsSolver->solve (equationString, eq, &coefficients, coefficientTermIndex))
    {
        if (appState != editObjectState)
            objects.add (new Object1D (equationString, eq, fdsSolver->calculateGridSpacing (eq, static_cast<double>(coefficients.getValueAt(0)))));
        else
            objects.set (objects.indexOf (editingObject), new Object1D (equationString, eq, fdsSolver->calculateGridSpacing (eq, static_cast<double> (coefficients.getValueAt(0)))));
        
        Object1D* newObject = objects[objects.size() - 1];
        
        // only add the coefficients that are actually used by the newly created object
        StringArray usedCoeffs = fdsSolver->getUsedCoeffs (equationString);
        NamedValueSet currentCoefficients = coefficientList.getNamedValueSet (usedCoeffs);
        newObject->setCoefficients (currentCoefficients);
        
        for (auto coeffComp : coefficientList.getCoefficients())
            if (currentCoefficients.contains(coeffComp.get()->getName()))
                newObject->setCoefficientComponent (coeffComp);
        
        newObject->addChangeListener (this);
        newObject->setCoefficientTermIndex (coefficientTermIndex);
        newObject->refreshCoefficients();
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
    calculator->getButton("createPM")->setButtonText ("Create");
    appState = normalAppState;
    
    return true;
}
    
void MainComponent::buttonClicked (Button* button)
{
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        button->setState (Button::ButtonState::buttonNormal);
        return;
    }
    
    if (button == newButton)
    {
        if (appState == normalAppState)
        {
            changeAppState (newObjectState);
            newButton->setButtonText ("CANCEL");
        }
        else if (appState == newObjectState) // new model has been cancelled
        {
            coefficientList.emptyCoefficientList();
            changeAppState (normalAppState);
            newButton->setButtonText ("New Model");
        }
    }
}

void MainComponent::sliderValueChanged (Slider* slider)
{
    coefficients.set (slider->getName(), slider->getValue());
    for (auto object : objects)
    {
        if (object->getCoefficientPtr()->contains (slider->getName()))
        {
            object->getCoefficientPtr()->set (slider->getName(), slider->getValue());
            object->refreshCoefficients();
        }
    }
}

void MainComponent::refresh()
{
    if (calculator->refresh())
        for (auto coeffButton : coeffButtons)
            coeffButton->setEnabled (false);
    else
        for (auto coeffButton : coeffButtons)
            coeffButton->setEnabled (true);
    
    resized();
}

void MainComponent::timerCallback()
{
    for (int i = 0; i < objects.size(); ++i)
        objects[i]->repaint();
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

void MainComponent::changeAppState (ApplicationState applicationState)
{
    coefficientList.setApplicationState (applicationState);
    calculator->setApplicationState (applicationState);
    appState = applicationState;
}

