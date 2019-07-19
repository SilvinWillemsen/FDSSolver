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
    addAndMakeVisible (calculator);
    addCoeffWindow = new AddCoefficient();
    
    coeffTopLabel = new Label();
    coeffTopLabel->setColour (Label::textColourId, Colours::white);
    coeffTopLabel->setText ("Coefficients", dontSendNotification);
    coeffTopLabel->setFont (Font (16.0f));
    addAndMakeVisible (coeffTopLabel);

    addKeyListener (this);
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
    fdsSolver = new FDSsolver (GUIDefines::debug ? 1.0 : 1.0 / fs);// / fs);
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

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    Rectangle<int> totArea = getLocalBounds();

    totArea.removeFromTop(GUIDefines::margin);
    totArea.removeFromLeft(GUIDefines::margin);
    totArea.removeFromRight(GUIDefines::margin);
    
    calculator->setBounds(totArea.removeFromTop (200));
    
    // draw objects
    for (int i = 0; i < objects.size(); ++i)
    {
        objects[i]->setBounds(totArea.removeFromTop(100));
    }
    
    Rectangle<int> buttonArea = totArea.removeFromRight (GUIDefines::buttonAreaWidth);
    buttonArea.removeFromTop(GUIDefines::margin);
    
    // Draw Coefficients
    
    if (coefficientComponents.size() != 0)
    {
        coeffTopLabel->setVisible (true);
        buttonArea.removeFromTop (GUIDefines::margin);
        coeffTopLabel->setBounds (buttonArea.removeFromTop (GUIDefines::buttonHeight));
        buttonArea.removeFromTop (GUIDefines::margin);
    } else {
        coeffTopLabel->setVisible (false);
    }

    for (int i = 0; i < coefficientComponents.size(); ++i)
    {
        coefficientComponents[i]->setBounds(buttonArea.removeFromTop(GUIDefines::buttonHeight));
        buttonArea.removeFromTop(GUIDefines::margin);
    }

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
        if (coefficients.contains (coeffName))
        {
            int coeffIndex = coefficients.indexOf (coeffName);
            coefficientComponents[coeffIndex]->update(isDynamic, value);
        } else {
            coefficientComponents.add (new CoefficientComponent (coeffName, value, isDynamic));
            coefficientComponents[coefficientComponents.size() - 1]->addChangeListener (this);
            addAndMakeVisible (coefficientComponents[coefficientComponents.size() - 1]);
        }
        
        coefficients.set (coeffName, value);
        
        if (addCoeffWindow->getCoeffPopupState() == normalCoeffState)
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
                    appState = editObjectState;
                    break;
                    
                case removeObject:
                    objects.removeObject (object);
                    break;
                
                default:
                    break;
            }
        }
    }
    
    for (auto coefficientComponent : coefficientComponents)
    {
        if (source == coefficientComponent)
        {
            String name = coefficientComponent->getName();
            switch (coefficientComponent->getAction()) {
                case insertCoeff:
                    calculator->addToEquation (calculator->encoder (name) + "_");
                    break;
                    
                case editCoeff:
                    addCoeffWindow->setCoeffPopupState (editingCoeff);
                    addCoeffWindow->setCoeffName (coefficientComponent->getName());
                    calculator->clickButton ("coeff");
                    break;
                    
                case removeCoeff:
                    coefficientComponents.removeObject(coefficientComponent);
                    coefficients.remove (coefficientComponent->getName());
                    break;
                    
                case sliderMoved:
                    coefficients.set (name, coefficientComponent->getSliderValue());
                    std::cout << coefficients.getVarPointer (name)->toString() << std::endl;
                    break;
                    
                default:
                    break;
            }
        }
    }
    refresh();
}

void MainComponent::createPhysicalModel()
{
    String equationString = calculator->getEquationString();
    int amountOfTimeSteps = fdsSolver->getStencilWidth (equationString, false);
    int stencilWidth = fdsSolver->getStencilWidth (equationString, true);
    Equation eq (amountOfTimeSteps, stencilWidth);
    
    Array<var> coefficientTermIndex;
    if (fdsSolver->solve (equationString, eq, &coefficients, coefficientTermIndex))
    {
        if (appState == normalAppState)
            objects.add (new Object1D (equationString, eq, fdsSolver->calculateGridSpacing (eq, static_cast<double>(coefficients.getValueAt(0)))));
        else if (appState == editObjectState)
            objects.set (objects.indexOf (editingObject), new Object1D (equationString, eq, fdsSolver->calculateGridSpacing (eq, static_cast<double>(coefficients.getValueAt(0)))));
        
        Object1D* newObject = objects[objects.size() - 1];
        newObject->setCoefficients (&coefficients);
        newObject->addChangeListener (this);
        newObject->setCoefficientTermIndex (coefficientTermIndex);
        newObject->refreshCoefficients();
        addAndMakeVisible (newObject);

    }
}

void MainComponent::editPhysicalModel()
{
    createPhysicalModel();
    editingObject = nullptr;
    calculator->getButton("createPM")->setButtonText ("Create");
    appState = normalAppState;
}
    
void MainComponent::buttonClicked (Button* button)
{
    refresh();
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
    
    if (!(ModifierKeys::getCurrentModifiers() == ModifierKeys::shiftModifier))
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
                buttonToClick = StringCode::getEncoded()[2];
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
    else if (CharacterFunctions::isLetter (key.getTextCharacter()) && calculator->getButton ("900")->isEnabled())
    {
        String keyString;
        keyString += (juce_wchar) key.getKeyCode();
        //        bool alreadyCoefficient = false;
        if (coefficients.contains (keyString))
        {
            for (auto coefficientComponent : coefficientComponents)
                if (coefficientComponent->getName() == keyString)
                {
                    coefficientComponent->clickCoeffButton();
                    return true;
                }
        }
        addCoeffWindow->setCoeffName (keyString);
        addCoefficient();
    }
    return true;
}

int MainComponent::testFunc(int test)
{
    return test;
}
