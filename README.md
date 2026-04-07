# Forward Rendering Pipeline

A software rasterizer implementing the complete forward rendering pipeline, written in C++ from scratch. Built as part of CENG 477 — Introduction to Computer Graphics at METU.

## Overview

Given a 3D scene described in XML (meshes, instances, cameras, transformations), the program produces a 2D raster image in PPM format. No GPU or graphics API is used — every stage of the pipeline is implemented in software.

The project was built on a course-provided scaffold (data structures, XML parsing, math utilities). The rendering logic — transformations, culling, clipping, rasterization, and depth buffering — is implemented in `Scene.cpp`.

## Pipeline Stages

- **Modeling transformations** — Translation, rotation, and scaling applied to mesh instances in user-defined order
- **Viewing transformation** — Camera space transformation using gaze, up, and position vectors
- **Projection** — Both perspective and orthographic projection
- **Backface culling** — Normal-based culling in world space (enable/disable per scene)
- **Clipping** — Liang-Barsky line clipping for wireframe mode
- **Rasterization** — Midpoint algorithm for edges (wireframe), barycentric coordinate interpolation for filled triangles (solid)
- **Depth buffering** — Per-pixel depth test for correct occlusion

## Rendering Modes

- **Wireframe** — Draws triangle edges only, with clipping
- **Solid** — Fills triangles with interpolated vertex colors

## Build & Run

```bash
make rasterizer
./rasterizer <input_file.xml>
```

Output is written as a `.ppm` file. To convert to PNG:

```bash
convert output.ppm output.png  # requires ImageMagick
```

## Input Format

Scene files are in XML and define background color, culling setting, cameras, vertices, transformations (T/R/S), meshes, and instances. Each instance references a mesh and applies an ordered sequence of transformations.

## Tech Stack

- **Language:** C++
- **Build:** Make
- **XML Parsing:** tinyxml2
- **Output Format:** PPM

## Author

**Doğu Erbaş**  
doguerbass@gmail.com  
METU Computer Engineering — CENG 477
