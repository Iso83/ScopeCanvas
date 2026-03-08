NodeCanvasGL — Refactor Roadmap
Purpose

This roadmap describes the incremental refactoring plan that transforms the current NodeCanvasGL prototype into a reusable node graph engine.

The current codebase mixes:

demo application logic

rendering

graph model

input handling

The goal is to progressively separate these responsibilities while keeping the system functional.

All changes must be performed in small incremental tasks.

Target architecture

Final architecture:

C++ Core Engine
       │
       ▼
C API
       │
       ▼
C# Wrapper
       │
       ▼
WinForms / WPF Control

Later:

Graph Engine
      │
      ▼
Graph JSON
      │
      ▼
Blazor Web Viewer
Current architecture (simplified)
App
 ├ Window (GLFW)
 ├ Input handling
 ├ Graph
 ├ Commands
 ├ Rendering
 └ Demo node creation

This means the application is not reusable as a library.

Refactor strategy

Rules for all phases:

do not redesign large parts at once

keep runtime behavior identical

modify only a few files per step

avoid renaming large sets of files in one commit

verify build after every step

Phase A — Engine extraction

Goal: separate engine state from the GLFW demo host.

Introduce a central engine object.

CanvasEngine
    ├ GraphDocument
    └ CommandStack

The application should only manage:

window
input
render loop
Tasks

A1
Create class CanvasEngine.

Members:

GraphDocument
CommandStack

Expose accessors:

graph()
commands()

A2
Remove GraphDocument ownership from App.

Replace with:

CanvasEngine engine;

A3
Move CommandStack ownership into CanvasEngine.

A4
Update code that references graph:

graph → engine.graph()

A5
Update command calls:

commands → engine.commands()

A6
Ensure renderer only receives:

GraphDocument
Viewport

Renderer must not depend on App.

A7
Verify runtime behavior:

node creation works

edge connections work

undo/redo works

rendering unchanged

Phase B — Node definition system

Goal: allow custom node types.

Introduce:

NodeDefinition
ConnectorDefinition
NodeStyle

Node instances must reference a definition.

Tasks

B1
Create NodeDefinition.

Properties:

title
icon
connectors
visual style

B2
Create ConnectorDefinition.

Properties:

name
direction
maxConnections
dataType

B3
Replace NodeTypeRegistry with NodeDefinitionRegistry.

B4
Update node creation so nodes are created from definitions.

B5
Ensure connectors are instantiated from connector definitions.

Phase C — Connector validation rules

Goal: prevent invalid graph states.

Connector rules must enforce:

direction
capacity
same-node connections
self connections
Tasks

C1
Add validation logic before edge creation.

C2
Prevent self connections.

C3
Prevent invalid direction connections.

C4
Implement connector capacity rules.

C5
Fix multi-edge behavior when reconnecting edges.

Phase D — Group nodes

Goal: support hierarchical graphs.

Introduce:

GroupNode

Groups contain child nodes.

Tasks

D1
Create GroupNode structure.

D2
Allow nodes to belong to groups.

D3
Support collapsing and expanding groups.

D4
Project connectors to group borders when collapsed.

Phase E — Grid and layout

Goal: improve spatial editing.

Features:

grid rendering
grid snapping
node alignment
Tasks

E1
Add grid renderer.

E2
Add grid snapping during node drag.

E3
Add optional alignment helpers.

Phase F — Graph views

Goal: support multiple views of the same graph.

Structure:

GraphDocument
    ├ GraphView A
    ├ GraphView B

Each view has:

viewport
zoom
selection
layout state
Tasks

F1
Create GraphView structure.

F2
Move viewport state to GraphView.

F3
Allow multiple views referencing same graph.

Phase G — C API

Goal: expose engine functionality externally.

The C API must be stable and minimal.

Example functions:

nc_create_engine
nc_destroy_engine

nc_create_node
nc_delete_node

nc_connect
nc_disconnect

nc_undo
nc_redo

Handles must be opaque pointers.

Tasks

G1
Create C API wrapper layer.

G2
Expose engine lifecycle functions.

G3
Expose graph editing functions.

Phase H — C# wrapper

Goal: allow use inside .NET UI.

Architecture:

C# Control
    │
PInvoke wrapper
    │
C API
    │
C++ Engine
Tasks

H1
Create PInvoke bindings.

H2
Create WinForms control host.

H3
Forward input events to engine.

Phase I — Advanced node visuals

Goal: richer node appearance.

Features:

node header
node body
icons
connector labels
status indicators
Phase J — Algorithm visualization

Goal: support algorithm exploration.

Initial target:

SHA256 cycle visualization

Nodes:

Maj
Ch
Σ0
Σ1
Add
Rotate
XOR

Operators must support drill-down to bit level.

Summary

This roadmap transforms NodeCanvasGL from a prototype into a reusable engine.

Key goals:

clean architecture
host independence
extensible node system
C API integration
C# control support

All changes must be implemented gradually through the phases described above.