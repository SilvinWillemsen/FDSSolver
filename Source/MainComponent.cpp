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

    
    for (auto button : buttons)
    {
        button->addListener(this);
        addAndMakeVisible (button);
    }
    
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
    fdsSolver = new FDSsolver (&stringCode, debug ? 1.0 : fs);// / fs);
    fdsSolver->setGridSpacing (debug ? 1.0 : (1.0 / 80.0));
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
    int margin = 10;
    int buttonHeight = 30;
    
    Rectangle<int> totArea = getLocalBounds();
    Rectangle<int> buttonArea = totArea.removeFromRight(areawidth);
    
    totArea.removeFromTop(margin);
    totArea.removeFromLeft(margin);
    textBox->setBounds(totArea.removeFromTop(buttonHeight));
    
    buttonArea.removeFromRight(margin);
    buttonArea.removeFromLeft(margin);
    buttonArea.removeFromTop(margin);
    
    int topButtonWidth = (buttonArea.getWidth() - margin) / 2.0;
    int buttonWidth = (buttonArea.getWidth() - margin * 2) / 3.0;
    
    Rectangle<int> buttonRow;
    buttonRow = buttonArea.removeFromTop(buttonHeight);
    
    checkEq->setBounds(buttonRow.removeFromLeft(topButtonWidth));
    buttonRow.removeFromLeft(margin);
    createPM->setBounds(buttonRow.removeFromLeft(topButtonWidth));
    
    
    for (int i = startOfOperators; i < buttons.size(); ++i)
    {
        if ((i - startOfOperators) % 3 == 0)
        {
            buttonArea.removeFromTop(margin);
            buttonRow = buttonArea.removeFromTop(buttonHeight);
        }
        buttons[i]->setBounds(buttonRow.removeFromLeft(buttonWidth));
        buttonRow.removeFromLeft(margin);
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
        fdsSolver->solve (equation);
        equation = "";
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
        tokens[i]; // holds next token
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
    
    return returnString;
}

void MainComponent::refreshLabel()
{
    textBox->setText(decoder(equation), dontSendNotification);
}
