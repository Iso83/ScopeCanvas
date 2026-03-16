# ScopeCanvas

ScopeCanvas is a GPU-accelerated canvas engine for building node-based editors, visual tools, and widget-driven diagram interfaces.

The project focuses on providing a high-performance rendering and interaction layer for large graphs and visual pipelines while remaining independent from application logic.

Typical use cases include:

* node-based editors
* visual programming tools
* data-flow pipelines
* UI/widget canvases
* UML or diagram editors

ScopeCanvas provides the **canvas runtime**, while applications define their own node semantics and widget content.

---

## Features (current / planned)

Current:

* GPU accelerated canvas rendering
* node graph rendering
* grid / camera / viewport system
* node selection and interaction
* edge connections
* layout engine
* command system (undo / redo)

Planned:

* widget nodes
* custom node rendering hooks
* grouping and hierarchical nodes
* virtualized rendering for large graphs
* embedding in external UI frameworks
* C API for language bindings

---

## Tech Stack

* **C++20**
* **OpenGL**
* **GLFW**
* **GLM**
* **CMake**

---

## Project Structure

```
ScopeCanvas
├─ src/engine/core   -> target: ScopeCanvasCore   (geen externe libs voor core tests)
├─ src/engine/render -> target: ScopeCanvasRender (GL / ImGui)
├─ src/engine/tests  -> CTest voor core
└─ src/studio        -> target: ScopeCanvasStudio (demo host)
```

Scope split:

* **ScopeCanvasEngine** = generic model, rendering, interaction-safe graph operations.
* **ScopeCanvasStudio** = demo/sample host (menus, sample node styles, demo layouts).
* Future apps can bind engine node ids to external domain ids without adding domain logic into the engine.

---

## Testing (CTest)

Core model checks are available through CTest:

```
cmake -S . -B build -DSCOPECANVAS_BUILD_STUDIO=OFF
cmake --build build --target ScopeCanvasEngineTests
ctest --test-dir build --output-on-failure
```

---

## Build

```
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Status

The project started as an experimental prototype and is now evolving into a reusable canvas engine.
