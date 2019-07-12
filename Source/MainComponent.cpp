/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    
    buttons.add (new TextButton("checkEq"));
    checkEq = buttons[0];
    checkEq->setButtonText("Check");
    
    buttons.add (new TextButton("createPM"));
    createPM = buttons[1];
    createPM->setButtonText("Create");

    startOfOperators = buttons.size();
    
    deltaString = String(CharPointer_UTF8 ("\xce\xb4"));
    
    buttons.add (new TextButton("equals"));
    equals = buttons[buttons.size() - 1];
    equals->setButtonText(" = ");
    
    buttons.add (new TextButton("plus"));
    plus = buttons[buttons.size() - 1];
    plus->setButtonText(" + ");
    
    buttons.add (new TextButton("minus"));
    minus = buttons[buttons.size() - 1];
    minus->setButtonText(" - ");
    
    buttons.add (new TextButton("deltaForwT"));
    deltaForwT = buttons[buttons.size() - 1];
    deltaForwT->setButtonText(deltaString + "t+");
    
    buttons.add (new TextButton("deltaBackT"));
    deltaBackT = buttons[buttons.size() - 1];
    deltaBackT->setButtonText(deltaString + "t-");
    
    buttons.add (new TextButton("deltaTT"));
    deltaTT = buttons[buttons.size() - 1];
    deltaTT->setButtonText(deltaString + "tt");
    
    buttons.add (new TextButton("deltaForwX"));
    deltaForwX = buttons[buttons.size() - 1];
    deltaForwX->setButtonText(deltaString + "x+");
    
    buttons.add (new TextButton("deltaBackX"));
    deltaBackX = buttons[buttons.size() - 1];
    deltaBackX->setButtonText(deltaString + "x-");
    
    buttons.add (new TextButton("deltaXX"));
    deltaXX = buttons[buttons.size() - 1];
    deltaXX->setButtonText(deltaString + "xx");
   
    buttons.add (new TextButton("uLN"));
    uLN = buttons[buttons.size() - 1];
    uLN->setButtonText("u");
    
    buttons.add (new TextButton("coeff"));
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
    textBox->setFont(Font(fontName, style, 16.0));
    
    addAndMakeVisible(textBox);
    
    addCoeffWindow = new AddCoefficient();
    
    for (auto button : buttons)
    {
        button->addListener(this);
        addAndMakeVisible (button);
    }
    
    coeffLabels.reserve(1);
    
    setSize (800, 600);
    
    // specify the number of input and output channels that we want to open
    setAudioChannels (2, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    delete textBox;
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
    fdsSolver = new FDSsolver (&stringCode, debug ? 1.0 : 1.0 / fs);// / fs);
    fdsSolver->setGridSpacing (debug ? 1.0 : (1.0 / 80.0));
    int stencilwidth = 7;
    eq = new Equation (stencilwidth);
    
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();

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
    int areawidth = 200;
    
    Rectangle<int> totArea = getLocalBounds();
    Rectangle<int> buttonArea = totArea.removeFromRight(areawidth);

    totArea.removeFromTop(GUIDefines::margin);
    totArea.removeFromLeft(GUIDefines::margin);
    textBox->setBounds(totArea.removeFromTop(GUIDefines::buttonHeight));
    
    buttonArea.removeFromRight(GUIDefines::margin);
    buttonArea.removeFromLeft(GUIDefines::margin);
    buttonArea.removeFromTop(GUIDefines::margin);
    
    int topButtonWidth = (buttonArea.getWidth() - GUIDefines::margin) / 2.0;
    int buttonWidth = (buttonArea.getWidth() - GUIDefines::margin * 2) / 3.0;
    
    Rectangle<int> buttonRow;
    buttonRow = buttonArea.removeFromTop(GUIDefines::buttonHeight);
    
    checkEq->setBounds(buttonRow.removeFromLeft(topButtonWidth));
    buttonRow.removeFromLeft(GUIDefines::margin);
    createPM->setBounds(buttonRow.removeFromLeft(topButtonWidth));
    
    
    for (int i = startOfOperators; i < buttons.size(); ++i)
    {
        if ((i - startOfOperators) % 3 == 0)
        {
            buttonArea.removeFromTop(GUIDefines::margin);
            buttonRow = buttonArea.removeFromTop(GUIDefines::buttonHeight);
        }
        buttons[i]->setBounds(buttonRow.removeFromLeft(buttonWidth));
        buttonRow.removeFromLeft(GUIDefines::margin);
    }
    
    buttonArea.removeFromTop(GUIDefines::margin);
    
    Rectangle<int> coeffListArea = buttonArea;
    for (int i = 0; i < coefficients.size(); ++i)
    {
        coeffLabels[i]->setBounds (coeffListArea.removeFromTop(GUIDefines::buttonHeight));
    }
    
}

void MainComponent::buttonClicked(Button* button)
{
    if (button == checkEq)
    {
        fdsSolver->checkEquation (equation);
    }
    else if (button == createPM)
    {
        fdsSolver->solve (equation, eq);
        equation = "";
    }
    else if (button == coeff)
    {
        addAndMakeVisible(addCoeffWindow);
        DialogWindow::LaunchOptions dlg;
        dlg.dialogTitle = TRANS("Add Coefficient");
        dlg.content.set(addCoeffWindow, false);
        if (dlg.runModal() == 0)
        {
            String coeffName = addCoeffWindow->getCoeffName();
            if (coefficients.contains(coeffName))
            {
                std::cout << "Coefficient name already exists" << std::endl;
                return;
            }
            double value = addCoeffWindow->getValue();
            
            coefficients.set (coeffName, value);
            String labelString = coeffName + " = " + String(value);
            Label* label = new Label ();
            coeffLabels.push_back (label);
            label->setText (labelString, dontSendNotification);
            label->setFont (Font("Latin Modern Math", "Italic", 16.0));
            label->setColour (Label::textColourId, Colours::white);
            addAndMakeVisible (label);
            
            int lim = 3 - coeffName.length();
            for (int i = 0; i < lim; ++i)
            {
                coeffName += "-";
            }
            std::cout << coeffName << std::endl;
            equation = equation + coeffName + "_";
            resized();
        }
        
    }
    else if (button == backSpace)
    {
        equation = equation.dropLastCharacters (4);
    }
    else
    {
        equation = equation + encoder(button->getButtonText()) + "_";
    }
    refreshLabel();
}

String MainComponent::encoder (String string)
{
    for (int i = 0; i < stringCode.getStringArraySize(); ++i)
    {
        if (stringCode.decoded[i] == string)
        {
            return stringCode.encoded[i];
        }
    }
    return CharPointer_UTF8 ("\xef\xbf\xbd");
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

void MainComponent::refreshLabel()
{
    textBox->setText(decoder(equation), dontSendNotification);
}
