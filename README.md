# eduRend Lab Exercises

By **Chris Illy Culling**

2025-04-03

**36%** — **| OOOO———————— |**

## Lab 1: Cube & Camera

- [x] Cube class
- [x] Mouse-controlled camera rotation
- [x] Hierarchical transformations

Non-mandatory:
- [ ] OBJ-export & import

## Lab 2: First-person Camera, Phong Shading

- [x] First-person Camera
    - [x] Movement relative to the camera view direction
    - [x] Limit x-axis rotation
- [ ] Constant Buffers
- [ ] Phong Shading

## Lab 3: Textures and Samplers

- [ ] Import, bind and use a texture
- [ ] Sampler, address modes and filters

## Lab 4: Normal Mapping

- [ ] Implement normal mapping

## Lab 5: Reflective Shader and Skybox

- [ ] Phong with reflected component
- [ ] Skybox

# eduRend
![eduRend](EDU_2d_s.png)

Main framework for graphics assignments in DA307A Computer Graphics and Modelling, Malmö University.

Clone this repository to start working with the course assignments. Cloning can be done here on this page, or using e.g. [GitHub Desktop Client](https://desktop.github.com/).

## Requirements
- Windows 10 or 11.
- Visual Studio 2019 (C++14) or newer
- A GPU that supports DirectX 11.

## Main changes: 2023 version
- Major cleanup (@xzereha, @FilipTrip)

## Main changes: 2022 version
- Scene class hierarchy (@cjgribel)
- [stb_image](https://github.com/nothings/stb) for texture loading (@cjgribel)
- Shader hot-reloading (@xzereha)
- Better DirectX debugging (@xzereha)
- Window class (@xzereha)

Base version (2016-2021) by CJ Gribel (@cjgribel).
