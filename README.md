# UniversityRealTimeGraphicsSponza
The first semester in my final year tasked me with expanding the renderer I created in [second year](https://github.com/storm20200/UniversitySecondYearSponza). The assignment specified four features which needed implementing. They were deferred rendering, physically-based shading, shadow mapping for shadow-casting spotlights and antialiasing. This had to run at a minimum of 30FPS on relatively old and weak Quadro 4000 hardware (similar to AMD 5750 and Nvidia GTS 450). The provided framework featured a scene inside Sponza Palace, utilising [Crytek's Sponza model](http://www.crytek.com/cryengine/cryengine3/downloads).

All of the rendering code was written by me (everything in the MyView and Renderer class), everything else was part of the given framework. I ended up receiving 90% for my work which I'm quite proud of and I found the assignment very interesting to research and work on. Some of the core features include:

- Multi-draw indirect rendering for all objects,
- Persistently mapped buffer streaming for all dynamic data,
- Uniform buffer blocks for all supported uniform data,
- Forward/deferred rendering toggle,
- Single/multi-threaded toggle,
- Blinn-Phong shading,
- Physically-based shading, diffuse with Disney Diffuse and specular with Schlick's approximation, uncorrelated Smith and GGX,
- Spotlight shadow mapping,
- Integration with [Subpixel Morphological Antialiasing 1x](http://www.iryoku.com/smaa),
- Albedo and physics properties can be stored as material-wide components or texture maps.
- Built for OpenGL 4.5 with Direct State Access used wherever possible.
