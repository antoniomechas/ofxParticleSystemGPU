# ofxParticleSystemGPU
Particle system in GPU with multitexture

This addon allows to create a partice system managed entirely in the GPU
It is forked from the work done by Neil Mendoza at https://github.com/neilmendoza/ofxGpuParticles

Example video:

https://vimeo.com/117903303

Features:

- Different parameters for each particle individually: size, color, texture, etc.
- Velocity can affect the size / width / color of particles by parameter
- It is possible to use multitexture for the particles, so, each particle can have its own texture
- Possibility to restrict how long a particle can go from its starting point
- The particles can be drawn as shapes coming from the starting point
- The color of the particles can be set up by an static / dynamic image
- Optical Flow (GPU). By using ofxMioFlowGLSL (https://github.com/princemio/ofxMIOFlowGLSL).

Coming soon:

- Vector Field
- Emitters

Dependencies:

- ofxGui
- ofxMioFlowGLSL (https://github.com/princemio/ofxMIOFlowGLSL). Coming soon for the optical flow in the GPU.

Developed with OF 0.8.4 and windows 7 64 bits.
Exaple includes project for Visual Studio 2012

by http://antoniomechas.net
