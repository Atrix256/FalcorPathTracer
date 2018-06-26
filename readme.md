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

* videos done at 400x300?

* talk about falcor

* run your DOF implementation by someone to make sure it seems ok?

* note that this is a diffuse path tracer only so no shiny specular highlights or reflections, but those would do bokeh too if they were there.

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

* In path tracing DOF you "scatter" which means you need "random point in 2d shape" for bokeh
 * In rasterization, a common thing is to "scatter while gathering", in which case you need "2d point in shape" test for bokeh.

* better bokeh results recipe:
 * https://twitter.com/romainguy/status/1009528535436939266

* 3 minute dof / bokeh explanation video on youtube
 * https://www.youtube.com/watch?v=bXpTDtU8wgU

* pinhole camera on wikipedia: https://en.wikipedia.org/wiki/Pinhole_camera_model#The_geometry_and_mathematics_of_the_pinhole_camera
* lens based camera: https://en.wikipedia.org/wiki/Camera_lens#Theory_of_operation
* more on pinhole: https://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera/virtual-pinhole-camera-model
* circle of confusion: https://en.m.wikipedia.org/wiki/Circle_of_confusion

Rendering with perspective projection matrix is a pinhole camera.

Smaller hole = sharper image, but also dimmer.  There's also a limit to how small it can get (diffraction limit. Could link!)

To get more light (faster exposure) and also sharper images, can replace the pin hole with a lens.

Unfortunately now only one point (plane?) is in focus though, unlike pin hole cameras which have focus not based on distance, but on pin hole (aperture) size! (Could do a pinhole camera with varying hole size!)

The longer the focal length (the less curved the lens) the longer it takes (over distance) to make the image less focused. This means, the acceptable range of focus is bigger. A deeper depth of field.

The more curved (the shorter the focal length) the shallower/shorter the depth of field is.

Todo: how does aperture size fit in here?

Good bokeh shots: (from Romain guy)
* Wider aperture
* Longer depth of field
* Big distances between foreground and background

Show diagrams and show where screen is (near and far plane) in each model.

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

## TODOs for DOF / Bokeh Blog post

* maybe put a couple more balls in the scene in the background, so there is something to see out of focus etc

* try simulating a pinhole camera with varying aperture sizes

* do pixels all have the same focus point? if so, you are doing it wrong. Figure lens cameras out.
 * you are close but not quite right. need to do the offset at the camera position, but yes, aim at where the pinhole camera would, at the focal distance

* rework the tracing loop, it's confusing to follow and I'm pretty sure if it hits max loop count it doesn't shade the final point, which is wasteful!

? Do I need to multiply by area of aperture or anything for bokeh? Even when circular...

* look for TODOs

* images to show
 * scene without jitter or dof / bokeh
 * add jitter
 * add dof
 * add shaped bokeh

* images for when explaining how to get good bokeh shots
 * link to romain guy's tweet?
 * show when it's in foreground.
 * show when everythings in background
 * show a nice final result

* animations to show:
 * circle around the scene? toggle bokeh on and off periodically
 * adjust focal length
 * adjust aperature size

* make a tag (?) in github for the code that goes with this blog post. understand how those work, but that's what you can put on your blog post i guess!
* make sure it explains how to compile etc. Maybe have someone try it before you publish

## TODOs



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
