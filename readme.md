## Getting Started

I pulled down this falcor, built it and ran one of the samples to make sure it worked ok: "SimpleDeferred"
https://github.com/NVIDIAGameWorks/Falcor/commit/0b561caae19e8325853166cc4c93d4763570774a

I followed the instructions for "creating a new project" in the readme.md file
* If you haven't done so already, create a Visual Studio solution and project for your code. Falcor only supports 64-bit builds, so make sure you have a 64-bit build configuration
* Add Falcor.props to your project (Property Manager -> Right click your project -> Add existing property sheet)
* Add Falcor.vcxproj to your solution
* Add a reference to Falcor in your project (Solution Explorer -> Your Project -> Right Click References -> Click Add Reference... -> Choose Falcor)

I coppied the "ComputeShader" project's files into my own project...
* ComputeShader.cpp / .h
* Data/compute.hlsl

Compiled, ran and it worked woo.  Now time to start writing code.

## Pathtracing

Starting simple with just diffuse and emissive, I used the info from my previous post:
https://blog.demofox.org/2016/09/21/path-tracing-getting-started-with-diffuse-and-emissive/

But also:
* importance sampling via cosine weighted hemisphere sampling (explain)
* jittering the camera




## Notes

* make this blog post about using falcor
* also about sub pixel jitter for AA and cosine weighted hemisphere sampling
* ... and about TAA / blue noise?
* possibly also about whitted raytracing?

## Links

* blue noise textures: http://momentsingraphics.de/?p=127
* Aras' path tracing: http://aras-p.info/blog/2018/03/28/Daily-Pathtracer-Part-0-Intro/
  * (READ HIS STUFF TOO)
* mine: https://blog.demofox.org/2016/09/21/path-tracing-getting-started-with-diffuse-and-emissive/

## TODOs

* an option for non cosine weighted hemispherical samples (pure white noise)
 * maybe also blue noise hemispherical samples?

? have a "miss color" that is a shader constant
 * could expose it in the ui
 * if the ray misses at first, use this color
 * when a ray misses eventually or iteration count is over, use this color

* clean up code. eg put hash21 into a header?

* make jitter options: none, white noise, vdc

* try using blue noise in cosine weighted hemispheres? or bn as an RNG seed somehow.
 * could also have other noise types
 * blue noise only good for low sample counts
* add temporal AA?
* have imgui options for these.

* presets if it makes sense...
 * path tracer
 * TAA path tracer

* implement russian roulette? or wait for specular maybe

Next: specular!  Make a short blog post about this and a tag / release to link to from the blog post?

* vsync toggle? how do you change it? it's in the device desc. maybe let it be off.

* fix the debug spam.
 * does it happen to the compute shader example? yes it does!

* more geo types and a specific scene

* rename compute.hlsl to pathtrace.hlsl or something

* look for TODOs

* show this source to patrick so he can see how falcor works

* is X,Y,Z the orientation you expect?

* is your white noise good enough?
 * I feel like it might not be. It looks pretty noisy
 * seems to have some subtle patterns in the output and takes too long to converge