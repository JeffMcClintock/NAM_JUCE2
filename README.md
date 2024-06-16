# NAM_JUCE2
JUCE Plugin using NAM as a submodule

1. Create a new repository
2. add an 'ext' folder
3. Add NAM Core in the ext folder: git submodule add https://github.com/sdatkinson/NeuralAmpModelerCore
4. Create a JUCE Plugin project 
5. In Projucer File Explorer add the source code files from folder: ext/NeuralAmpModelerCore/NAM
6. In Projucer settings add the following include directories:
    * ../../../ext/NeuralAmpModelerCore/NAM
    * ../../../ext/NeuralAmpModelerCore/Dependencies/eigen
    * ../../../ext/NeuralAmpModelerCore/Dependencies/nlohmann
7. In Projucer settings, add preprocessor definition: NAM_SAMPLE_FLOAT
8. look for code annotated with '!! New code !!' to see the bare minimum code to get NAM working

