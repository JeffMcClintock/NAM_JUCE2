/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyAmpPluginAudioProcessorEditor::MyAmpPluginAudioProcessorEditor (MyAmpPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // !! New code !!
    addAndMakeVisible(loadModelButton);
    loadModelButton.setButtonText("Load Model");
    loadModelButton.onClick = [this] {
        myChooser = std::make_unique<juce::FileChooser>(
            "Select a file to open...", juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.nam");

        myChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser) {
                audioProcessor.loadModel(chooser.getResult().getFullPathName().toStdString());
            });
        };
}

MyAmpPluginAudioProcessorEditor::~MyAmpPluginAudioProcessorEditor()
{
}

//==============================================================================
void MyAmpPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MyAmpPluginAudioProcessorEditor::resized()
{
    // !! New code !!
    loadModelButton.setBounds(20, 10, 100, 20);
}
