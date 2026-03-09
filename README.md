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
src/            engine implementation
include/        public headers
assets/         shaders and runtime resources
third_party/    external libraries
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
