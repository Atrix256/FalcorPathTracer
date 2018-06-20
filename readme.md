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


## Explicit Light Sampling Notes

* show how sampling point lights are really the answer to the integral, because there is only one place that has non zero value!
 * you need distance attenuation though... which complicates the explanation
 * i think point lights have different unit of measurement. radiance vs irrandiance.
* then spherical lights being sampled from solid angle
* then quad lights / generalize to other shapes and non solid angle.

## DOF blog post notes

* basic camera ray generation using inverse view transform matrix?
* jitter: show with on vs off
* DOF
 * both length and radius are in world units
 * talk about shaped bokeh! (random point in shape!)
 ? should we show cosine weighted hemisphere too? not sure how to show it off so maybe not. maybe too off topic

* general uniform point in 2d shape: http://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=03E49E24217598863AE5A66FDEB68832?doi=10.1.1.6.6683&rep=rep1&type=pdf
* point in triangle: https://math.stackexchange.com/q/18686/138443
* also this: http://blog.wolfram.com/2011/07/28/how-i-made-wine-glasses-from-sunflowers/

* DOF: http://cg.skeelogy.com/depth-of-field-using-raytracing/

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

## TODOs

* make a different scene that is larger and has small bright light sources in the background

? why does my bokeh have rounded corners??
 * the lights are large and round

* make a better scene at some point, or have a drop down of different scenes.
 * for bokeh blog post

* more interesting shaped bokeh?
 * triangle?
 * make a star from triangles?
 
* shaped aperature support? doesn't mean much until we have specular highlights, but small bright lights will do it too i guess...

* could write a super quick raytracing depth of field blog post.
 * including shaped bokeh?
 ? what unit of measurement is aperture size in?
  * well, if you do it post transformation, it is in world units aka meters!!

* get rid of unuseful stuff - blue noise, jitter, integrate checkbox etc.

* maybe make this program able to generate a mitsuba path tracer xml scene file?
 * probably need it to make an obj, and then can import into mitsuba.
 * can obj handle spheres? or does mitsuba xml file support spheres?
 ! actually it looks like it can do spheres. mjp's shows this in the adding emitters section
 * aras' path tracing thing also has the mitsuba scene available.
  * https://github.com/aras-p/ToyPathTracer/blob/04-fixes/Mitsuba/scene.xml
 ? can it do triangles / quads? if so, can avoid obj files and make mitsuba files directly!

* is the point light calculation correct? I don't think so... check out the non cosine weighted hemisphere equation to make sure

* distance attentuation of point lights too. square falloff

* get mitsuba working after point lights. maybe a small blog post about what's involved?

* i see anti aliasing even when jittering is off... what is up with that? where is it coming from?
 * yeah, the jitter is for TAA. maybe remove it for now?

* DOF -
 * params: aperature size (controls blur), focal length (controls where the focused objects are)
 * could also do shaped bokeh!

* if doing DOF, don't need jitter I don't think.
 * maybe want to be able to turn DOF though
 * and also, jitter still seems to possibly be applicable - like to in focus objects - since you still want to integrate over the pixel

? why isn't jittered sampling on by default? is it not working or??


* support explicitly sampling punctual lights so you understand them

* quad light support

* mitsuba verificiation

* rework the tracing loop, it's confusing to follow and I'm pretty sure if it hits max loop count it doesn't shade the final point, which is wasteful!

* read this and see what you want to take. it's already in the links section
 * http://simonstechblog.blogspot.com/2018/06/simple-gpu-path-tracer.html?m=1
 * it talks about cosine weighted explicit rectangular light sampling, and needing to use the jacobian. Should understand that better!

* info here about pathtracing from the smallpt path tracer, including how basic DOF works. already in links section
 * https://drive.google.com/file/d/0B8g97JkuSSBwUENiWTJXeGtTOHFmSm51UC01YWtCZw/view

* are you multiplying by cosine theta correctly when cosine weighted hemisphere sampling is off?
 * reason through it and maybe make it more explicit that it's correct

* depth of field, before specular?
 * also direct light sampling and russian roulette

* refraction at some point

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

* should you have a denoise option?

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
