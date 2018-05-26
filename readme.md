## Getting Started

I pulled down this falcor, built it and ran one of the samples to make sure it worked ok: "SimpleDeferred"
https://github.com/NVIDIAGameWorks/Falcor/commit/0b561caae19e8325853166cc4c93d4763570774a

I followed the instructions for "creating a new project" in the readme.md file
* If you haven't done so already, create a Visual Studio solution and project for your code. Falcor only supports 64-bit builds, so make sure you have a 64-bit build configuration
* Add Falcor.props to your project (Property Manager -> Right click your project -> Add existing property sheet)
* Add Falcor.vcxproj to your solution
* Add a reference to Falcor in your project (Solution Explorer -> Your Project -> Right Click References -> Click Add Reference... -> Choose Falcor)