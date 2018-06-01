## Getting Started

I pulled down this falcor, built it and ran one of the samples to make sure it worked ok: "SimpleDeferred"
https://github.com/NVIDIAGameWorks/Falcor/commit/0b561caae19e8325853166cc4c93d4763570774a
* git clone https://github.com/Atrix256/FalcorPathTracer.git
* git checkout 0b561caae19e8325853166cc4c93d4763570774a
* somehow it was able to find the location of falcor... weird.

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

* quadrupling sample count halves error
 * https://en.wikipedia.org/wiki/Monte_Carlo_method#Integration

## Links

* blue noise textures: http://momentsingraphics.de/?p=127
* Aras' path tracing: http://aras-p.info/blog/2018/03/28/Daily-Pathtracer-Part-0-Intro/
  * (READ HIS STUFF TOO)
* mine: https://blog.demofox.org/2016/09/21/path-tracing-getting-started-with-diffuse-and-emissive/

## TODOs

* are you multiplying by cosine theta correctly when cosine weighted hemisphere sampling is off?
 * reason through it and maybe make it more explicit that it's correct

* direct light sampling
 * have an option to turn it on and off to compare the difference

* depth of field, before specular?
 * also direct light sampling and russian roulette

* Read Aras' stuf before too long
 * compare with mitsuba too, but aras has a post on that

* try profiling with renderdoc or vtune or who knows what else to see if you can find any obvious bottlenecks to fix

* TAA version
 * jitter camera
 * use IGN 5.5 noise
 * need world position per pixel to project to previous frame - either by actually storing the position, or getting it via depth.
 * probably not compatible with DOF.

* clean up code. eg put rng stuff into a header?
 * or maybe it'd be better & simpler to have everything in a single file. if so, bring geo.h in!

* comment code better, especially main?

* blue noise isn't that compelling as you've implemented it. keep it? ditch it? modify it?
 * ditching it is probably fine, but i like the example of how to use a texture, so... ??
 * may want it for TAA

* presets if it makes sense...
 * path tracer slow
 * path tracer fast
 * TAA path tracer

* should there be a better scene? or maybe a couple scenes you can choose from the drop down?

* rename compute.hlsl to pathtrace.hlsl or something

* if ray generation details get complex (russian roullette), have it somehow atomic count raycount like aras does? or leave it primary rays?

* organize UI into logical groups

* look for TODOs

? tone mapping?

Next: specular!  Make a short blog post about this and a tag / release to link to from the blog post?
* show this source to patrick so he can see how falcor works


Links:
* a pretty good read on lots of things pathtracing: https://computergraphics.stackexchange.com/questions/5152/progressive-path-tracing-with-explicit-light-sampling
* deriving lambertian BRDF 1/pi: http://www.rorydriscoll.com/2009/01/25/energy-conservation-in-games/
* a good explanation of what importance sampling is all about: https://computergraphics.stackexchange.com/questions/4979/what-is-importance-sampling

Questions:
* how does direct light sampling work?
* how do you importance sample arbitrary BRDFs / BSDFs?
* how does multiple importance sampling work?
* how and why does russian roulete work?
* lambertian BRDF is 1/pi as explained in that link, but why is there the 2 in 2 * cosTheta * LOut * albedo? You mention it in your pathtracing post, maybe figure out why.
* a lot of path tracers have a pdf value they multiply by. whats that about
 * It's the probability of having chosen that ray direction. If uniform sampling it's 1 / total area.
 ? this is the 1/pi in non cosine weighted hemispherical samples, right?

Answered questions:
