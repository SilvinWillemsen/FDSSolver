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
Calculator::Calculator()
{
    textBox = new Label();
    textBox->setColour(Label::textColourId, Colours::black);
    textBox->setColour(Label::backgroundColourId, Colours::white);
    String fontName ("Latin Modern Math");
    String style ("Italic");
    textBox->setFont (Font (fontName, style, 16.0));
    
    addAndMakeVisible(textBox);
    
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
    
    
    
    for (auto button : buttons)
    {
        button->addListener(this);
        addAndMakeVisible (button);
    }

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
    textBox->setBounds(totArea.removeFromTop(GUIDefines::buttonHeight));
    
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
    
}

void Calculator::buttonClicked (Button* button)
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
        equationString = "";
    }
    
    else if (button == createPM)
    {
        if (createPMalreadyClicked)
            return;
        std::cout << "createPMclicked" << std::endl;
        changeMessage = createPMMessage;
        sendChangeMessage();
    }
    
    else if (button == coeff)
    {
        changeMessage = addCoeffMessage;
        sendChangeMessage();
    }
    else if (button == backSpace)
    {
        equationString = equationString.dropLastCharacters (4);
    } else {
        equationString += encoder (button->getButtonText()) + "_";
    }
    refresh();
}

bool Calculator::keyPressed(const KeyPress& key, Component* originatingComponent)
{
    
}

// Linking the return key to the createPM button
bool Calculator::keyStateChanged (bool isKeyDown, Component* originatingComponent)
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

TextButton* Calculator::getButton (String buttonName)
{
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
