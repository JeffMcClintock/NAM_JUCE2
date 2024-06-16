/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyAmpPluginAudioProcessor::MyAmpPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

MyAmpPluginAudioProcessor::~MyAmpPluginAudioProcessor()
{
}

//==============================================================================
const juce::String MyAmpPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyAmpPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MyAmpPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MyAmpPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MyAmpPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyAmpPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MyAmpPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyAmpPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MyAmpPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyAmpPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MyAmpPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // !! New code !!
    namProcessor.clear();
    loadedModel.clear();
    namProcessor.resize(getTotalNumOutputChannels());
    loadedModel.resize(getTotalNumOutputChannels());
    loadModel(modelPath);
}

void MyAmpPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyAmpPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MyAmpPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	// !! New code !!
    if (filesUpdated.exchange(false))
    {
        for (int ch = 0; ch < namProcessor.size(); ch++)
            namProcessor[ch] = std::move(loadedModel[ch]);
    }

    for (int ch = 0; ch < buffer.getNumChannels(); ch++)
    {
        if (namProcessor[ch])
        {
            const float inputLevelNormalized = 1.0f;
            const float outputLevelNormalized = 1.0f;

            //?        namProcessor->SetNormalize(outputNormalized);
            namProcessor[ch]->process(
                const_cast<float*>(buffer.getReadPointer(ch)), buffer.getWritePointer(ch), buffer.getNumSamples());

            namProcessor[ch]->finalize_(buffer.getNumSamples());
        }
    }
}

//==============================================================================
bool MyAmpPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MyAmpPluginAudioProcessor::createEditor()
{
    return new MyAmpPluginAudioProcessorEditor (*this);
}

//==============================================================================
void MyAmpPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // !! New code !!
    auto xml = std::make_unique<juce::XmlElement>("state");
    xml->setAttribute("ModelPath", modelPath);
    copyXmlToBinary(*xml, destData);
}

void MyAmpPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // !! New code !!
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (!xmlState.get())
        return;

    modelPath = xmlState->getStringAttribute("ModelPath").toStdString();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyAmpPluginAudioProcessor();
}

// !! New code !!
void MyAmpPluginAudioProcessor::loadModel(const std::string& file)
{
    if (file.empty())
        return;

    modelPath = file;
    auto dspPath = std::filesystem::u8path(modelPath);
    auto loaded = nam::get_dsp(dspPath);

    for (auto& p : loadedModel)
    {
        if (loaded)
            p = nam::get_dsp(dspPath);
    }

    filesUpdated.store(true, std::memory_order_release);
}
