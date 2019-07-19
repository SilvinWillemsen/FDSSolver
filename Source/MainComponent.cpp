/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Create all buttons
    buttons.add (new TextButton("createPM"));
    createPM = buttons[0];
    createPM->setButtonText("Create");
    createPM->addShortcut (KeyPress (KeyPress::returnKey));
    
    buttons.add (new TextButton("clearEq"));
    clearEq = buttons[1];
    clearEq->setButtonText("Clear");

    // indicate where operators start
    startOfOperators = buttons.size();
    
    deltaString = String(CharPointer_UTF8 ("\xce\xb4"));
    
    buttons.add (new TextButton("100"));
    equals = buttons[buttons.size() - 1];
    equals->setButtonText(" = ");
    
    buttons.add (new TextButton("101"));
    plus = buttons[buttons.size() - 1];
    plus->setButtonText(" + ");
    
    buttons.add (new TextButton("102"));
    minus = buttons[buttons.size() - 1];
    minus->setButtonText(" " + String(CharPointer_UTF8 ("\xe2\x80\x93")) + " ");
    
    buttons.add (new TextButton("901"));
    minusSign = buttons[buttons.size() - 1];
    minusSign->setButtonText("(-)");
    
    buttons.add (new TextButton("200"));
    deltaForwT = buttons[buttons.size() - 1];
    deltaForwT->setButtonText(deltaString + "t+");
    
    buttons.add (new TextButton("201"));
    deltaBackT = buttons[buttons.size() - 1];
    deltaBackT->setButtonText(deltaString + "t-");
    
    buttons.add (new TextButton("202"));
    deltaCentT = buttons[buttons.size() - 1];
    deltaCentT->setButtonText(deltaString + "t" + String (CharPointer_UTF8 ("\xc2\xb7")));
    
    buttons.add (new TextButton("203"));
    deltaTT = buttons[buttons.size() - 1];
    deltaTT->setButtonText(deltaString + "tt");
    
    buttons.add (new TextButton("204"));
    deltaForwX = buttons[buttons.size() - 1];
    deltaForwX->setButtonText(deltaString + "x+");
    
    buttons.add (new TextButton("205"));
    deltaBackX = buttons[buttons.size() - 1];
    deltaBackX->setButtonText(deltaString + "x-");
    
    buttons.add (new TextButton("206"));
    deltaCentX = buttons[buttons.size() - 1];
    deltaCentX->setButtonText(deltaString + "x" + String (CharPointer_UTF8 ("\xc2\xb7")));
    
    buttons.add (new TextButton("207"));
    deltaXX = buttons[buttons.size() - 1];
    deltaXX->setButtonText(deltaString + "xx");
   
    buttons.add (new TextButton("300"));
    uLN = buttons[buttons.size() - 1];
    uLN->setButtonText("u");
    
    buttons.add (new TextButton("900"));
    coeff = buttons[buttons.size() - 1];
    coeff->setButtonText("coeff");
    
    buttons.add (new TextButton("backspace"));
    backSpace = buttons[buttons.size() - 1];
    backSpace->setButtonText(String(CharPointer_UTF8 ("\xe2\x8c\xab")));
    
    textBox = new Label();
    textBox->setColour(Label::textColourId, Colours::black);
    textBox->setColour(Label::backgroundColourId, Colours::white);
    String fontName ("Latin Modern Math");
    String style ("Italic");
    textBox->setFont (Font (fontName, style, 16.0));
    
    addAndMakeVisible(textBox);
    
    addCoeffWindow = new AddCoefficient();
    
    for (auto button : buttons)
    {
        button->addListener(this);
        addAndMakeVisible (button);
    }
    
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
    fdsSolver = new FDSsolver (&stringCode, GUIDefines::debug ? 1.0 : 1.0 / fs);// / fs);
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
    Rectangle<int> buttonArea = totArea.removeFromRight(GUIDefines::areaWidth);

    totArea.removeFromTop(GUIDefines::margin);
    totArea.removeFromLeft(GUIDefines::margin);
    textBox->setBounds(totArea.removeFromTop(GUIDefines::buttonHeight));
    totArea.removeFromTop (GUIDefines::margin);
    
    // draw objects
    for (int i = 0; i < objects.size(); ++i)
    {
        objects[i]->setBounds(totArea.removeFromTop(100));
    }
    
    buttonArea.removeFromRight(GUIDefines::margin);
    buttonArea.removeFromLeft(GUIDefines::margin);
    buttonArea.removeFromTop(GUIDefines::margin);
    
    int topButtonWidth = (buttonArea.getWidth() - GUIDefines::margin) / 2.0;
    
    Rectangle<int> buttonRow;
    buttonRow = buttonArea.removeFromTop(GUIDefines::buttonHeight);
    
    createPM->setBounds (buttonRow.removeFromLeft(topButtonWidth));
    buttonRow.removeFromLeft(GUIDefines::margin);
    clearEq->setBounds(buttonRow.removeFromLeft(topButtonWidth));
    
    for (int i = startOfOperators; i < buttons.size(); ++i)
    {
        if ((i - startOfOperators) % 4 == 0)
        {
            buttonArea.removeFromTop(GUIDefines::margin);
            buttonRow = buttonArea.removeFromTop(GUIDefines::buttonHeight);
        }
        buttons[i]->setBounds(buttonRow.removeFromLeft(GUIDefines::buttonWidth));
        buttonRow.removeFromLeft(GUIDefines::margin);
    }
    
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
//    int labelCount = 0;
//    int sliderCount = 0;
    for (int i = 0; i < coefficientComponents.size(); ++i)
    {
        coefficientComponents[i]->setBounds(buttonArea.removeFromTop(GUIDefines::buttonHeight));
        buttonArea.removeFromTop(GUIDefines::margin);
    }

}

void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    for (auto object : objects)
    {
        if (source == object)
        {
            switch (object->getAction()) {
                case editObject:
                    equation = object->getEquationString();
                    createPM->setButtonText ("Edit");
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
                    equation = equation + encoder (name) + "_";
                    break;
                    
                case editCoeff:
                    addCoeffWindow->setCoeffState (editingCoeff);
                    addCoeffWindow->setCoeffName (coefficientComponent->getName());
                    buttonClicked (coeff);
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

void MainComponent::createPhysicalModel ()
{
    int amountOfTimeSteps = fdsSolver->getStencilWidth (equation, false);
    int stencilWidth = fdsSolver->getStencilWidth (equation, true);
    Equation eq (amountOfTimeSteps, stencilWidth);
    
    Array<var> coefficientTermIndex;
    if (fdsSolver->solve (equation, eq, &coefficients, coefficientTermIndex))
    {
        if (appState == normalAppState)
            objects.add (new Object1D (equation, eq, fdsSolver->calculateGridSpacing (eq, static_cast<double>(coefficients.getValueAt(0)))));
        else if (appState == editObjectState)
            objects.set (objects.indexOf (editingObject), new Object1D (equation, eq, fdsSolver->calculateGridSpacing (eq, static_cast<double>(coefficients.getValueAt(0)))));
        
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
//    if (equation != editingObject->getEquationString())
//    {
    createPhysicalModel();
//    } else {
//        editingObject->setCoefficients (&coefficients);
//        editingObject->refreshCoefficients();
//    }
//    objects.set (objects.indexOf (editingObject), new Object1D (equation, fdsSolver->getStencil (eq), eq.getNumPoints()));
    editingObject = nullptr;
    createPM->setButtonText ("Create");
    appState = normalAppState;
}
    
void MainComponent::buttonClicked (Button* button)
{
    
    // Linking the return key to the createPM button
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        if (button == createPM && !createPMalreadyClicked)
            createPMalreadyClicked = true;
        else if (button != createPM)
        {
            button->setState (Button::ButtonState::buttonNormal);
            return;
        }
    }

    if (button == clearEq)
    {
        equation = "";
    }
    else if (button == createPM)
    {
        if (createPMalreadyClicked)
            return;
        std::cout << "createPMclicked" << std::endl;
        appState == normalAppState ? createPhysicalModel() : editPhysicalModel();
        resized();
    }
    
    else if (button == coeff)
    {
        addAndMakeVisible (addCoeffWindow);
        DialogWindow::LaunchOptions dlg;
        dlg.dialogTitle = TRANS(coeffPopupState == normalCoeffState ? "Add Coefficient" : "Edit Coefficient");
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
            
            if (coeffPopupState == normalCoeffState)
            {
                int lim = 3 - coeffName.length();
                for (int i = 0; i < lim; ++i)
                {
                    coeffName += "-";
                }
                equation = equation + coeffName + "_";
            } else {
                addCoeffWindow->setCoeffState(normalCoeffState);
            }
            
            resized();
        }
        
        // always set the dynamic tickbox of the addCoeffWindow to false
        addCoeffWindow->setDynamic (false);
    }
    else if (button == backSpace)
    {
        equation = equation.dropLastCharacters (4);
    }
    else
    {
        equation = equation + encoder (button->getButtonText()) + "_";
    }
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

String MainComponent::encoder (String string)
{
    if (string == "(-)")
    {
        string = "-";
    }
    for (int i = 0; i < stringCode.getStringArraySize(); ++i)
    {
        if (stringCode.decoded[i] == string)
        {
            return stringCode.encoded[i];
        }
    }
    
    int lim = 3 - string.length();
    for (int i = 0; i < lim; ++i)
    {
        string += "-";
    }
    return string;
//    return CharPointer_UTF8 ("\xef\xbf\xbd");
}

String MainComponent::decoder (String string)
{
    StringArray tokens;
    tokens.addTokens (string, "_", "\"");
    String returnString;
    
    // Remove last token as that will be a "_"
    tokens.remove(tokens.size() - 1);
    
    for (int i = 0; i < tokens.size(); i++)
    {
        String firstChar = tokens[i].substring(0, 1);
        const char* test = static_cast<const char*> (firstChar.toUTF8());
        
        if (!std::isdigit(*test))
        {
            returnString += tokens[i].upToFirstOccurrenceOf ("-", false, true);
            
        } else {
            bool flag = true;
            for (int j = 0; j < stringCode.getStringArraySize(); ++j)
            {
                if (stringCode.encoded[j] == tokens[i])
                {
                    returnString += stringCode.decoded[j];
                    flag = false;
                    break;
                }
            }
            if (flag)
                returnString += "??";
        }
    }
    
    return returnString;
}

void MainComponent::refresh()
{

    // obtain the type of the last character
    int firstInt;
    if (equation.length() == 0)
    {
        firstInt = 0;
    } else {
        String firstChar = equation.substring(equation.length() - 4, equation.length() - 3);
        const char* test = static_cast<const char*> (firstChar.toUTF8());
        firstInt = std::isdigit(*test) ? firstChar.getIntValue() : 9;
    }
    
    std::vector<int> notAllowedCharacters;
    switch (firstInt)
    {
        case 0:
            notAllowedCharacters.push_back(1);
            break;
        case 1:
            notAllowedCharacters.push_back(1);
            break;
        case 2:
            notAllowedCharacters.push_back(1);
            notAllowedCharacters.push_back(9);
            break;
        case 3:
            notAllowedCharacters.push_back(2);
            notAllowedCharacters.push_back(3);
            notAllowedCharacters.push_back(9);
            break;
        case 9:
            break;
    }
    
    // refresh which buttons are active
    for (int i = 0; i < buttons.size(); ++i)
    {
        if (std::find(notAllowedCharacters.begin(), notAllowedCharacters.end(),
                      buttons[i]->getName().substring(0, 1).getIntValue()) != notAllowedCharacters.end())
        {
            buttons[i]->setEnabled (false);
        } else {
            buttons[i]->setEnabled (true);
        }
    }
    
    // in the case of a (-)
    if (firstInt > 1)
    {
        minusSign->setEnabled (false);
    }
    
    // if the equation already has an equals sign, disable the button
    if (equation.contains("100"))
    {
        equals->setEnabled (false);
    }
    
    // if the coefficient button is disabled, disable all coeffbuttons
    if (std::find(notAllowedCharacters.begin(), notAllowedCharacters.end(), 9) != notAllowedCharacters.end())
        for (auto coeffButton : coeffButtons)
            coeffButton->setEnabled (false);
    else
        for (auto coeffButton : coeffButtons)
            coeffButton->setEnabled (true);
    
    textBox->setText(decoder(equation), dontSendNotification);
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
    TextButton* buttonToClick = nullptr;
    
    if (!(ModifierKeys::getCurrentModifiers() == ModifierKeys::shiftModifier))
    {
        switch (key.getTextCharacter())
        {
            case 12:
                buttonToClick = createPM;
                break;
            case '=':
                buttonToClick = equals;
                break;
            case '+':
                buttonToClick = plus;
                break;
            case '-':
                buttonToClick = minus;
                break;
            case 'g':
                buttonToClick = deltaForwT;
                break;
            case 'h':
                buttonToClick = deltaBackT;
                break;
            case 'j':
                buttonToClick = deltaCentT;
                break;
            case 'k':
                buttonToClick = deltaTT;
                break;
            case 'v':
                buttonToClick = deltaForwX;
                break;
            case 'b':
                buttonToClick = deltaBackX;
                break;
            case 'n':
                buttonToClick = deltaCentX;
                break;
            case 'm':
                buttonToClick = deltaXX;
                break;
            case 'u':
                buttonToClick = uLN;
                break;
            case 'q':
                std::cout << coefficients.getValueAt(0).toString() << std::endl;
                break;
            case 127:
                buttonToClick = backSpace;
                break;
            default:
                buttonToClick = nullptr;
                break;
        }
        
        if (buttonToClick != nullptr && buttonToClick->isEnabled())
            buttonClicked (buttonToClick);
    }
    else if (CharacterFunctions::isLetter (key.getTextCharacter()) && coeff->isEnabled())
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
        buttonClicked (coeff);
    }
    return true;
}

// Linking the return key to the createPM button
bool MainComponent::keyStateChanged (bool isKeyDown, Component* originatingComponent)
{
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        returnKeyIsDown = true;
    }
    else if (returnKeyIsDown) // this means that the return key is released
    {
        createPMalreadyClicked = false;
    }
}
