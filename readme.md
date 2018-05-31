## Getting Started

I pulled down this falcor, built it and ran one of the samples to make sure it worked ok: "SimpleDeferred"
https://github.com/NVIDIAGameWorks/Falcor/commit/0b561caae19e8325853166cc4c93d4763570774a
* git clone https://github.com/Atrix256/FalcorPathTracer.git
* git checkout 0b561caae19e8325853166cc4c93d4763570774a

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

* check out aras' sphere test code

* convert from linear to sRGB.
 * a copy shader that does this (and tone mapping?), not a copy resource operation.

* the axes of orientation are off somehow. maybe y is just flipped, i'm not really sure.
 * do orientation tests and find out.

* get the cornel box scene from the other path tracer

* are you multiplying by cosine theta correctly when cosine weighted hemisphere sampling is off?
 * reason through it and maybe make it more explicit that it's correct

* depth of field, before specular?

* is falcor next to this project? when i have it that way, it seems to have found it... ?!?!?!
 * yep. Maybe falcor registers somehow like via an environment variable?

* Read Aras' stuf before too long

* try profiling with renderdoc or vtune or who knows what else to see if you can find any obvious bottlenecks to fix

? is there a way to hide UI in falcor?

* directly sample lights?

* don't need to jitter camera because the shader jitters the rays inside
 * do this even w/ TAA? Well no... because we need to find the pixel from last frame.

* TAA may want IGN as a noise source.

* clean up code. eg put rng stuff into a header?
 * or maybe it'd be better & simpler to have everything in a single file. if so, bring geo.h in!

* comment code better, especially main?

* try using blue noise in cosine weighted hemispheres? or bn as an RNG seed somehow.
 * could also have other noise types
 * blue noise only good for low sample counts
* make jitter options: none, white noise, vdc?
* add temporal AA?
* have imgui options for these.
* blue noise isn't that compelling as you've implemented it. keep it? ditch it? modify it?

* presets if it makes sense...
 * path tracer
 * TAA path tracer

* implement russian roulette? or wait for specular maybe

* fix the debug spam.
 * does it happen to the compute shader example? yes it does!

* more geo types and a specific scene

* rename compute.hlsl to pathtrace.hlsl or something

* if ray generation details get complex (russian roullette), have it somehow atomic count raycount like aras does.

* organize UI into logical groups

* look for TODOs

? tone mapping?

Next: specular!  Make a short blog post about this and a tag / release to link to from the blog post?
* show this source to patrick so he can see how falcor works
