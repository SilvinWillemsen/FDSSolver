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
    buttons.add (new TextButton("clearEq"));
    clearEq = buttons[0];
    clearEq->setButtonText("Clear");
    
    buttons.add (new TextButton("createPM"));
    createPM = buttons[1];
    createPM->setButtonText("Create");

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
    deltaBackT = buttons[buttons.size() - 1];
    deltaBackT->setButtonText(deltaString + "t" + String (CharPointer_UTF8 ("\xc2\xb7")));
    
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
    deltaBackX = buttons[buttons.size() - 1];
    deltaBackX->setButtonText(deltaString + "x" + String (CharPointer_UTF8 ("\xc2\xb7")));
    
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
    
    setSize (800, 600);
    
    // specify the number of input and output channels that we want to open
    setAudioChannels (2, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
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
    fdsSolver = new FDSsolver (&stringCode, GUIDefines::debug ? 1.0 : 1.0 / fs);// / fs);
    fdsSolver->setGridSpacing (GUIDefines::debug ? 1.0 : (1.0 / 80.0));
    refresh();
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    // for all objects, run their schemes
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
    int areawidth = 250;
    
    Rectangle<int> totArea = getLocalBounds();
    Rectangle<int> buttonArea = totArea.removeFromRight(areawidth);

    totArea.removeFromTop(GUIDefines::margin);
    totArea.removeFromLeft(GUIDefines::margin);
    textBox->setBounds(totArea.removeFromTop(GUIDefines::buttonHeight));
    
    buttonArea.removeFromRight(GUIDefines::margin);
    buttonArea.removeFromLeft(GUIDefines::margin);
    buttonArea.removeFromTop(GUIDefines::margin);
    
    int topButtonWidth = (buttonArea.getWidth() - GUIDefines::margin) / 2.0;
    int buttonWidth = (buttonArea.getWidth() - GUIDefines::margin * 3) / 4.0;
    
    Rectangle<int> buttonRow;
    buttonRow = buttonArea.removeFromTop(GUIDefines::buttonHeight);
    
    clearEq->setBounds (buttonRow.removeFromLeft(topButtonWidth));
    buttonRow.removeFromLeft(GUIDefines::margin);
    createPM->setBounds(buttonRow.removeFromLeft(topButtonWidth));
    
    
    for (int i = startOfOperators; i < buttons.size(); ++i)
    {
        if ((i - startOfOperators) % 4 == 0)
        {
            buttonArea.removeFromTop(GUIDefines::margin);
            buttonRow = buttonArea.removeFromTop(GUIDefines::buttonHeight);
        }
        buttons[i]->setBounds(buttonRow.removeFromLeft(buttonWidth));
        buttonRow.removeFromLeft(GUIDefines::margin);
    }
    
    buttonArea.removeFromTop(GUIDefines::margin);
    
    // Draw Coefficients
    
    if (coeffButtons.size() != 0)
    {
        buttonArea.removeFromTop (GUIDefines::margin);
        coeffTopLabel->setBounds (buttonArea.removeFromTop (GUIDefines::buttonHeight));
        buttonArea.removeFromTop (GUIDefines::margin);
    }
    int labelCount = 0;
    int sliderCount = 0;
    for (int i = 0; i < coeffButtons.size(); ++i)
    {
        Rectangle<int> coeffRow = buttonArea.removeFromTop(GUIDefines::buttonHeight);
        coeffButtons[i]->setBounds (coeffRow.removeFromLeft(buttonWidth));
        if (coeffDynamic[i])
        {
            coeffRow.removeFromLeft (GUIDefines::margin);
            coeffSliders[sliderCount]->setBounds (coeffRow);
            ++sliderCount;
        } else {
            coeffLabels[labelCount]->setBounds (coeffRow);
            ++labelCount;
        }
        buttonArea.removeFromTop(GUIDefines::margin);
    }
    
}

void MainComponent::buttonClicked (Button* button)
{
    if (button == clearEq)
    {
//        fdsSolver->checkEquation (equation);
        equation = "";
    }
    else if (button == createPM)
    {
        int stencilwidth = fdsSolver->getStencilWidth (equation);
        eq = new Equation (stencilwidth);
        std::vector<double> coeffs (coefficients.size(), 0);
        for (int i = 0; i < coefficients.size(); ++i)
        {
            coeffs[i] = coefficients.getValueAt (i);
        }
        
        fdsSolver->setCoeffValues (&coefficients);
        if (fdsSolver->solve (equation, eq))
        {
//            equation = "";
        }
    }
    else if (button == coeff)
    {
        addAndMakeVisible(addCoeffWindow);
        DialogWindow::LaunchOptions dlg;
        dlg.dialogTitle = TRANS("Add Coefficient");
        dlg.content.set (addCoeffWindow, false);
        if (dlg.runModal() == 1)
        {
            String coeffName = addCoeffWindow->getCoeffName();
            if (coefficients.contains (coeffName))
            {
                std::cout << "Coefficient name already exists" << std::endl;
                return;
            }
            double value = addCoeffWindow->getValue();
            
            coefficients.set (coeffName, value);
            
            coeffButtons.add (new TextButton(coeffName));
            coeffButtons[coeffButtons.size() - 1]->setButtonText (coeffName);
            coeffButtons[coeffButtons.size() - 1]->addListener(this);
            addAndMakeVisible (coeffButtons[coeffButtons.size() - 1]);
        
            // if the coefficient is made dynamic...
            if (addCoeffWindow->isDynamic())
            {
                //create a slider
                coeffSliders.add (new Slider (Slider::LinearHorizontal, Slider::TextBoxLeft));
                Slider* newSlider = coeffSliders[coeffSliders.size() - 1];
                newSlider->setRange(0.0, value);
                newSlider->setValue (value);
                newSlider->setName (coeffName);
                addAndMakeVisible (newSlider);
                newSlider->addListener (this);
                coeffDynamic.push_back (true);
                
            } else {
                
                //create a label
                String labelString = " = " + String(value);
                coeffLabels.add (new Label());
                Label* label = coeffLabels[coeffLabels.size() - 1];
                label->setText (labelString, dontSendNotification);
                label->setFont (Font("Latin Modern Math", "Regular", 16.0));
                label->setColour (Label::textColourId, Colours::white);
                addAndMakeVisible (label);
                
                coeffDynamic.push_back (false);
            }
            
            int lim = 3 - coeffName.length();
            for (int i = 0; i < lim; ++i)
            {
                coeffName += "-";
            }
            equation = equation + coeffName + "_";
            
            resized();
        }
        
        // always set dynamic to false
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
    std::cout << coefficients.getName(0).toString() << slider->getName() <<  std::endl;
    std::cout << static_cast<double> (*coefficients.getVarPointer (slider->getName())) << std::endl;
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
}
