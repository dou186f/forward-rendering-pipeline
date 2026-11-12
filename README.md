# Forward Rendering Pipeline

A software rasterizer implementing core stages of the forward rendering pipeline in C++. Built as part of CENG 477 - Introduction to Computer Graphics at METU.

## Overview

Given a 3D scene described in XML (meshes, instances, cameras, and transformations), the program produces raster images in PPM format without a GPU or graphics API.

The project was built on a course-provided scaffold for data structures, XML parsing, and math utilities. The rendering pipeline is implemented in `Scene.cpp`, with transformation and clipping helpers in `Helpers.cpp`.

## Pipeline Stages

- **Modeling transformations** - Translation, rotation, and scaling applied to mesh instances in the order listed in the XML
- **Viewing transformation** - Camera space transformation using gaze, up, and position vectors
- **Projection** - Perspective and orthographic projection
- **Backface culling** - Normal-based culling in world space, enabled or disabled per scene
- **Clipping** - Outcode-based, Cohen-Sutherland-style line clipping in homogeneous coordinates for wireframe mode
- **Rasterization** - Bresenham-style incremental line drawing for wireframe edges and barycentric interpolation for filled triangles
- **Depth buffering** - Per-pixel depth testing for solid triangles

## Rendering Modes

- **Wireframe** - Draws triangle edges with clipping and interpolated vertex colors
- **Solid** - Fills triangles with interpolated vertex colors and depth testing

## Build & Run

Requires `g++` and GNU Make.

```bash
git clone https://github.com/dou186f/forward-rendering-pipeline.git
cd forward-rendering-pipeline
make rasterizer
./rasterizer inputs_outputs/inputs_outputs/culling_disabled_inputs/sample.xml
```

The example produces `sample.ppm`. Output filenames are defined in each scene's `OutputName` fields.

To convert the output to PNG with ImageMagick (optional):

```bash
magick sample.ppm sample.png
```

## Example Scenes and Outputs

Example scenes and outputs are available under `inputs_outputs/inputs_outputs/`:

| Folder | Contents |
|---|---|
| `culling_disabled_inputs/` | XML scenes with backface culling disabled |
| `culling_enabled_inputs/` | XML scenes with backface culling enabled |
| `culling_disabled_outputs/` | Supplied PNG outputs for the culling-disabled examples |
| `culling_enabled_outputs/` | Supplied PNG outputs for the culling-enabled examples |
| `different_projection_type/` | Perspective and orthographic variants of the flag and horse-and-mug scenes |
| `clipping_example/` | A wireframe clipping scene with two camera outputs |

Supplied PNG examples:

| Sample scene | Horse and mug |
|---|---|
| ![Sample scene with culling disabled](inputs_outputs/inputs_outputs/culling_disabled_outputs/sample.png) | ![Horse and mug with culling disabled](inputs_outputs/inputs_outputs/culling_disabled_outputs/horse_and_mug_1.png) |

## Tech Stack

- **Language:** C++11
- **Build:** GNU Make and g++
- **XML Parsing:** tinyxml2, included in the repository
- **Output Format:** PPM (ASCII P3)

## Author

**Doğu Erbaş**  
doguerbass@gmail.com  
METU Computer Engineering - CENG 477
