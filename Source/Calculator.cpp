/*
  ==============================================================================

    Calculator.cpp
    Created: 19 Jul 2019 12:48:03pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Calculator.h"

//==============================================================================
Calculator::Calculator (FDSsolver* fdsSolver) : fdsSolver (fdsSolver)
{
    textBox = std::make_unique<Label>();
    textBox->setColour(Label::textColourId, Colours::black);
    textBox->setColour(Label::backgroundColourId, Colours::white);
    String fontName ("Latin Modern Math");
    String style ("Regular");
    textBox->setFont (Font (fontName, style, 16.0));
    
    addAndMakeVisible (*textBox);
    
    // Create all buttons
    buttons.push_back(std::make_shared<TextButton> ("createPM"));
    createPM = buttons[0];
    createPM->setButtonText ("Create");
    createPM->addShortcut (KeyPress (KeyPress::returnKey));
    
    buttons.push_back(std::make_shared<TextButton> ("clearEq"));
    clearEq = buttons[1];
    clearEq->setButtonText ("Clear");
    
    // indicate where operators start
    startOfOperators = buttons.size();
    
    deltaString = String(CharPointer_UTF8 ("\xce\xb4"));
    
    buttons.push_back(std::make_shared<TextButton> ("100"));
    equals = buttons[buttons.size() - 1];
    equals->setButtonText (" = ");
    
    buttons.push_back(std::make_shared<TextButton> ("101"));
    plus = buttons[buttons.size() - 1];
    plus->setButtonText (" + ");
    
    buttons.push_back(std::make_shared<TextButton> ("102"));
    minus = buttons[buttons.size() - 1];
    minus->setButtonText  (" " + String(CharPointer_UTF8 ("\xe2\x80\x93")) + " ");
    
    buttons.push_back(std::make_shared<TextButton> ("901"));
    minusSign = buttons[buttons.size() - 1];
    minusSign->setButtonText ("(-)");
    
    buttons.push_back(std::make_shared<TextButton> ("200"));
    deltaForwT = buttons[buttons.size() - 1];
    deltaForwT->setButtonText (deltaString + "t+");
    
    buttons.push_back(std::make_shared<TextButton> ("201"));
    deltaBackT = buttons[buttons.size() - 1];
    deltaBackT->setButtonText (deltaString + "t-");
    
    buttons.push_back(std::make_shared<TextButton> ("202"));
    deltaCentT = buttons[buttons.size() - 1];
    deltaCentT->setButtonText (deltaString + "t" + String (CharPointer_UTF8 ("\xc2\xb7")));
    
    buttons.push_back(std::make_shared<TextButton> ("203"));
    deltaTT = buttons[buttons.size() - 1];
    deltaTT->setButtonText (deltaString + "tt");
    
    buttons.push_back(std::make_shared<TextButton> ("204"));
    deltaForwX = buttons[buttons.size() - 1];
    deltaForwX->setButtonText (deltaString + "x+");
    
    buttons.push_back(std::make_shared<TextButton> ("205"));
    deltaBackX = buttons[buttons.size() - 1];
    deltaBackX->setButtonText (deltaString + "x-");
    
    buttons.push_back(std::make_shared<TextButton> ("206"));
    deltaCentX = buttons[buttons.size() - 1];
    deltaCentX->setButtonText (deltaString + "x" + String (CharPointer_UTF8 ("\xc2\xb7")));
    
    buttons.push_back(std::make_shared<TextButton> ("207"));
    deltaXX = buttons[buttons.size() - 1];
    deltaXX->setButtonText (deltaString + "xx");
    
    buttons.push_back(std::make_shared<TextButton> ("300"));
    uLN = buttons[buttons.size() - 1];
    uLN->setButtonText ("u");
    
    buttons.push_back(std::make_shared<TextButton> ("900"));
    coeff = buttons[buttons.size() - 1];
    coeff->setButtonText ("coeff");
    
    buttons.push_back(std::make_shared<TextButton> ("backspace"));
    backSpace = buttons[buttons.size() - 1];
    backSpace->setButtonText (String(CharPointer_UTF8 ("\xe2\x8c\xab")));
    
    buttons.push_back(std::make_shared<TextButton> ("changedim"));
    changeDim = buttons[buttons.size() - 1];
    changeDim->setButtonText ("1D");
    
    for (auto button : buttons)
    {
        button->addListener(this);
        addAndMakeVisible (button.get());
    }

    addKeyListener (this);
    
}

Calculator::~Calculator()
{
}

void Calculator::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
}

void Calculator::resized()
{
    Rectangle<int> totArea = getLocalBounds();
    Rectangle<int> buttonArea = totArea.removeFromRight (GUIDefines::buttonAreaWidth);
    buttonArea.removeFromLeft (GUIDefines::margin);
    textBox->setBounds (totArea.removeFromTop(GUIDefines::buttonHeight));
    
    int topButtonWidth = (buttonArea.getWidth() - GUIDefines::margin) / 2.0;
    
    Rectangle<int> buttonRow;
    buttonRow = buttonArea.removeFromTop (GUIDefines::buttonHeight);
    
    createPM->setBounds (buttonRow.removeFromLeft (topButtonWidth));
    buttonRow.removeFromLeft (GUIDefines::margin);
    clearEq->setBounds (buttonRow.removeFromLeft (topButtonWidth));
    
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
    
}

void Calculator::buttonClicked (Button* button)
{
    // Linking the return key to the createPM button
    KeyPress key = KeyPress (KeyPress::returnKey);
    if (key.KeyPress::isCurrentlyDown())
    {
        if (button == createPM.get() && !createPMalreadyClicked)
        {
        }
        else if (button != createPM.get())
        {
            button->setState (Button::ButtonState::buttonNormal);
            return;
        }
    } else {
        createPMalreadyClicked = false;
    }
    
    if (button == clearEq.get())
    {
        clearEquation();
    }
    
    else if (button == createPM.get())
    {
        if (createPMalreadyClicked)
            return;
        std::cout << "createPMclicked" << std::endl;
        createPMalreadyClicked = true;
        action = createPMAction;
        sendChangeMessage();
    }
    
    else if (button == coeff.get())
    {
        action = addCoeffAction;
        sendChangeMessage();
    }
    else if (button == backSpace.get())
    {
        equationString = equationString.dropLastCharacters (4);
    }
    else if (button == changeDim.get())
    {
        changeDimension();
        
    } else {
        equationString += encoder (button->getButtonText()) + "_";
    }
    refresh();
}

bool Calculator::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    return false;
}

bool Calculator::refresh()
{
    bool disableCoeffs;

    // obtain the type of the last character
    int firstInt;
    if (equationString.length() == 0)
    {
        firstInt = 0;
    } else {
        String firstChar = equationString.substring (equationString.length() - 4, equationString.length() - 3);
        const char* test = static_cast<const char*> (firstChar.toUTF8());
        firstInt = std::isdigit(*test) ? firstChar.getIntValue() : 9;
    }
    
    // Fill the vector with not allowed characters and enable / disable buttons based on this
    std::vector<int> notAllowedCharacters;
    fdsSolver->checkNotAllowedCharacters (firstInt, notAllowedCharacters);
    
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
    
    // If the equation contains spatial derivatives, disable the ability to change the dimension
    if (fdsSolver->checkDimension (equationString) != 0)
        changeDim->setEnabled (false);
    else
        changeDim->setEnabled (true);
    
    // If the dimension is 0D, disable the spatial derivative buttons
    if (curDim == 0)
    {
        for (int i = 10; i < 14; ++i)
                buttons[i]->setEnabled (false);
    }
    else if (curDim == 2)
    {
        buttons[10]->setEnabled (false);
    }
    
    // in the case of a (-)
    if (firstInt > 1)
    {
        minusSign->setEnabled (false);
    }
    
    // if the equation already has an equals sign, disable the button
    if (equationString.contains("100"))
    {
        equals->setEnabled (false);
    }
    
    // if the coefficient button is disabled, disable all coeffbuttons
    if (std::find(notAllowedCharacters.begin(), notAllowedCharacters.end(), 9) != notAllowedCharacters.end())
        disableCoeffs = true;
    else
        disableCoeffs = false;
    
    textBox->setText (decoder (equationString), dontSendNotification);
    
    return disableCoeffs;
}

std::shared_ptr<TextButton> Calculator::getButton (String buttonName)
{
    if (buttonName == "coeff")
        return coeff;
    
    for (auto button : buttons)
        if (button->getName() == buttonName)
            return button;
    std::cout << buttonName << " is non-existent." << std::endl;
    return nullptr;
}

String Calculator::encoder (String string)
{
    if (string == "(-)")
    {
        string = "-";
    }
    for (int i = 0; i < StringCode::getStringArraySize(); ++i)
    {
        if (StringCode::getDecoded()[i] == string)
        {
            return StringCode::getEncoded()[i];
        }
    }
    
    int lim = 3 - string.length();
    for (int i = 0; i < lim; ++i)
    {
        string += "-";
    }
    return string;
}

String Calculator::decoder (String string)
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
            for (int j = 0; j < StringCode::getStringArraySize(); ++j)
            {
                if (StringCode::getEncoded()[j] == tokens[i])
                {
                    returnString += StringCode::getDecoded()[j];
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

void Calculator::setApplicationState (ApplicationState state)
{
    switch (state) {
        case normalAppState:
            setEnabled (false);
            createPM->setButtonText ("Create");
            clearEquation();
            break;
        case newObjectState:
            clearEquation();
            setEnabled (true);
            break;
        case editObjectState:
            createPM->setButtonText ("Edit");
            setEnabled (true);
            break;
        default:
            break;
    }
    appState = state;
}

void Calculator::changeDimension (int dim)
{
    if (dim != -1)
        curDim = dim;
    else // increase dimension
        curDim = (curDim + 1) % (GUIDefines::maxDim + 1);
    
    switch (curDim)
    {
        case 0:
            changeDim->setButtonText ("0D");
            buttons[11]->setButtonText (deltaString + "x+");
            buttons[12]->setName ("205");
            buttons[12]->setButtonText (deltaString + "x-");
            buttons[12]->setName ("206");
            buttons[13]->setButtonText (deltaString + "xx");
            buttons[13]->setName ("207");
            break;
        case 1:
            changeDim->setButtonText ("1D");
            break;
        case 2:
            changeDim->setButtonText ("2D");
            buttons[11]->setButtonText (deltaString + "xx");
            buttons[11]->setName ("208");
            buttons[12]->setButtonText (deltaString + "yy");
            buttons[12]->setName ("209");
            buttons[13]->setButtonText (CharPointer_UTF8 ("\xce\x94"));
            buttons[13]->setName ("210");
            break;
    }
}
