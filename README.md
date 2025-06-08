# Test Scene Controls

To move the camera, use the **`WASD`** keys.

The `UP`, `DOWN`, `LEFT`, and `RIGHT` keys serve as modifiers, and change what the `WASD` (and arrow) keys control.

## While holding the `UP` key: *Light Position*

`WASD`, `LEFT`, and `RIGHT` now control the position of the light source in the test scene.

## While holding the `DOWN` key: *Colour Adjustment*

`W`: Increase the `red`ness of all models

`S`: Decrease the `red`ness of all models

`D`: Increase the `green`ness of all models

`S`: Decrease the `green`ness of all models

`RIGHT`: Increase the `blue`ness of all models

`LEFT`: Decrease the `blue`ness of all models

## While holding the `LEFT` key: *Brightness Adjustment*

`W`: Increase material colour brightness of all models

`S`: Decrease material colour brightness of all models

`D`: Increase material shininess of all models

`A`: Decrease material shininess of all models

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
