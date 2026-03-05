# NodeCanvasGL – Agent Instructions

This repository contains a C++20 OpenGL based node/diagram canvas engine.

The architecture is strictly separated into layers:

Engine
- Graph model (Node, Edge, Group, Connector)
- Serialization
- Commands (undo/redo)

View
- Camera2D
- Viewport

Render
- Renderer
- GridRenderer
- NodeRenderer
- EdgeRenderer

Interaction
- SelectionController
- DragController
- ConnectController
- GroupController
- ArrangeController

## Rules

1. Do not change folder structure.
2. Do not merge engine logic with rendering code.
3. Rendering must remain OpenGL based.
4. Graph data structures must remain independent of rendering.
5. All new classes must follow the existing folder structure.

## Code style

- C++20
- No global variables
- RAII for OpenGL resources
- Prefer small focused classes

## Build
mkdir build
cd build
cmake ..
cmake --build .

## Goal

Create a performant node canvas system similar to:

- Unreal Blueprint
- Grasshopper
- Node-based editors

Features expected:

- Pan / zoom canvas
- Node selection and drag
- Bezier edge connections
- Grid snapping
- Group nodes
- Save / load JSON
- Undo / redo