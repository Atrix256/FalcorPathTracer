## Getting Started

I pulled down this falcor, built it and ran one of the samples to make sure it worked ok: "SimpleDeferred"
https://github.com/NVIDIAGameWorks/Falcor/commit/afdd9125283c90374694ed35875144fde81e9312
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


## Explicit Light Sampling Notes

* show how sampling point lights are really the answer to the integral, because there is only one place that has non zero value!
 * you need distance attenuation though... which complicates the explanation
 * i think point lights have different unit of measurement. radiance vs irrandiance.
* then spherical lights being sampled from solid angle
* then quad lights / generalize to other shapes and non solid angle.

## DOF blog post notes

* talk about falcor
* note that the code is a WIP for another path tracer, so there are todos and such that will get resolved as time goes on.
* make a tag (?) in github for the code that goes with this blog post. understand how those work, but that's what you can put on your blog post i guess!
* make sure it explains how to compile etc. Maybe have someone try it before you publish



## Notes

* make this blog post about using falcor
* also about sub pixel jitter for AA and cosine weighted hemisphere sampling
* ... and about TAA / blue noise?
* possibly also about whitted raytracing?
* also about explicit light sampling?

* quadrupling sample count halves error
 * https://en.wikipedia.org/wiki/Monte_Carlo_method#Integration

## Links

* blue noise textures: http://momentsingraphics.de/?p=127
* Aras' path tracing: http://aras-p.info/blog/2018/03/28/Daily-Pathtracer-Part-0-Intro/
  * (READ HIS STUFF TOO)
* mine: https://blog.demofox.org/2016/09/21/path-tracing-getting-started-with-diffuse-and-emissive/
* http://simonstechblog.blogspot.com/2018/06/simple-gpu-path-tracer.html?m=1
* smallpt: https://drive.google.com/file/d/0B8g97JkuSSBwUENiWTJXeGtTOHFmSm51UC01YWtCZw/view

* DOF: http://cg.skeelogy.com/depth-of-field-using-raytracing/

* mitsuba xml file format: https://mynameismjp.wordpress.com/2015/04/04/mitsuba-quick-start-guide/

## TODOs new 9/12/18 - for PBR path tracer

* clean out old stuff.
* start making pbr path tracing stuff!

## TODOs


* after post, maybe get rid of the multiply by lens area, and comment in the code that there's an implicit auto-exposure so that adjusting aperture size doesn't affect brightness.
 * also make auto fov on always. it's easier to work with and is simpler code.

* rework the tracing loop, it's confusing to follow and I'm pretty sure if it hits max loop count it doesn't shade the final point, which is wasteful!

* look at this for using blue noise for path tracing
 * http://www.iliyan.com/publications/DitheredSampling/DitheredSampling_Sig2016.pdf

? maybe try updating falcor? might get some fixes or something.

* group UI into logical settings. DOF, camera, etc.

* may need an option to sample a fixed number of lights maximum per frame.
 * should have a max defined i guess... 

* may want to try even a simple / lame tone mapping to see if it makes the lights not show up as white.

* NaN's are being generated! need to make it so content can't cause NaNs to be generated!
 * hit this when randomly generating spheres

* make a better scene at some point, or have a drop down of different scenes.
 * for bokeh blog post

* maybe make this program able to generate a mitsuba path tracer xml scene file?
 * probably need it to make an obj, and then can import into mitsuba.
 * can obj handle spheres? or does mitsuba xml file support spheres?
 ! actually it looks like it can do spheres. mjp's shows this in the adding emitters section
 * aras' path tracing thing also has the mitsuba scene available.
  * https://github.com/aras-p/ToyPathTracer/blob/04-fixes/Mitsuba/scene.xml
 ? can it do triangles / quads? if so, can avoid obj files and make mitsuba files directly!

* is the point light calculation correct? I don't think so... check out the non cosine weighted hemisphere equation to make sure

* get mitsuba working after point lights. maybe a small blog post about what's involved?

* support explicitly sampling punctual lights so you understand them

* quad light support

* mitsuba verificiation

* read this and see what you want to take. it's already in the links section
 * http://simonstechblog.blogspot.com/2018/06/simple-gpu-path-tracer.html?m=1
 * it talks about cosine weighted explicit rectangular light sampling, and needing to use the jacobian. Should understand that better!

* info here about pathtracing from the smallpt path tracer, including how basic DOF works. already in links section
 * https://drive.google.com/file/d/0B8g97JkuSSBwUENiWTJXeGtTOHFmSm51UC01YWtCZw/view

* are you multiplying by cosine theta correctly when cosine weighted hemisphere sampling is off?
 * reason through it and maybe make it more explicit that it's correct

* direct light sampling
* russian roulette

* refraction at some point

* try profiling with renderdoc or vtune or who knows what else to see if you can find any obvious bottlenecks to fix

? can we importance sample the lense? like... shoot a ray at a random point on the lights from the camera each frame, like we do for surfaces
 * put this on the todo list, can be a follow up thing

* try low discrepancy sequence on the lense? may make the noise in the bokeh be less white
 * if the importance sampling doesn't work.
 * Do you need to multiply the result by a scaling factor or anything?

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

* motion blur -> longer exposure time means more motion blur!

? tone mapping?

* should you have a denoise option?

* ray marched fog

* chromatic abberation
* motion blur

* textures. IQ has an aarticle on ray differentials for this

Next: specular!  Make a short blog post about this and a tag / release to link to from the blog post?
* show this source to patrick so he can see how falcor works

Notes:
* color is assumed to be divided by pi already, so diffuse 1/pi multiplier isn't there. Does the value not divided by pi have real world units of measurement then?

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
