NodeCanvasGL — Engine Architecture
Project overview

NodeCanvasGL is a C++ node graph engine designed for visual graph editing and analysis.

The project started as an OpenGL prototype but the long-term goal is to transform it into a reusable engine that can be embedded into other applications.

The engine will later be used for:

algorithm visualization

UML graph editing

data-flow graphs

cryptographic operation visualization

logic circuit exploration

The first real use case will be visualizing SHA-256 computation cycles, including drill-down to bit-level operations.

Target architecture

The project must evolve into a layered architecture.

C++ Core Engine
       │
       ▼
C API (stable interface)
       │
       ▼
C# Wrapper (PInvoke)
       │
       ▼
WinForms / WPF Control

Later an additional viewer will exist:

Graph Engine
     │
     ▼
Graph JSON
     │
     ▼
Blazor Web Viewer

The C++ engine is the authoritative graph model.

Design principles
Engine must be host independent

The engine must not depend on window systems.

Forbidden dependencies inside the engine:

GLFW

Win32

UI frameworks

input systems

Those belong to hosts.

Rendering must be separated from graph logic

The graph model must not depend on OpenGL.

Rendering should only read graph state.

Graph Model
     │
     ▼
Renderer

Never the other way around.

Input handling must be external

The engine must not handle:

keyboard shortcuts

mouse input

window events

Hosts convert input events to engine operations.

Engine must support multiple hosts

Example hosts:

GLFW demo
WinForms control
WPF control
Web viewer

Each host provides:

input

rendering surface

windowing

The engine provides:

graph model

commands

validation

persistence

Graph model

The core data model is a node graph.

GraphDocument
    │
    ├ Nodes
    ├ Edges
    └ Groups
Node

A node represents an operation or entity.

Examples:

algorithm step

logic operator

UML element

computation stage

Nodes contain connectors.

Connector

A connector represents a connection point.

Types:

Input
Output
Bidirectional

Connectors define rules:

max connections
direction
data type
connection policy
Edge

An edge connects two connectors.

Output → Input

Edges must respect connector rules.

Invalid connections must be rejected by the engine.

Group nodes

Groups allow hierarchical graphs.

A group node can contain:

child nodes
internal edges
internal layout

Groups support:

collapse
expand
connector projection
Node definitions

Nodes are created from NodeDefinitions.

This allows the engine to support custom node types.

Example definition:

NodeDefinition
    title
    icon
    connectors
    style

This enables external systems (such as C#) to define new node types.

Graph views

A graph may have multiple views.

GraphDocument
    │
    ├ View A
    ├ View B
    └ View C

Views define:

viewport
zoom
selection
layout state

This enables:

multiple editor windows

preview navigation

minimaps

detail views

Commands and undo

All graph mutations must be implemented as commands.

CreateNode
DeleteNode
MoveNode
ConnectEdge
DisconnectEdge
GroupNodes

Commands are executed through a command stack.

CommandStack
    undo
    redo

This guarantees deterministic editing behavior.

Rendering layer

Rendering is implemented separately from the graph model.

Renderer responsibilities:

draw nodes
draw connectors
draw edges
draw grid
draw selection

Renderer reads graph state but must not modify it.

Engine API

The engine must expose a stable C API.

Example:

nc_create_engine
nc_destroy_engine

nc_create_node
nc_delete_node

nc_connect
nc_disconnect

nc_undo
nc_redo

nc_render

Opaque handles should be used for safety.

Current project state

The current repository still contains a demo-style architecture.

The App class currently manages:

window
input
graph
commands
node creation
rendering

This mixes host responsibilities with engine responsibilities.

Refactor strategy

Refactoring must happen in small incremental steps.

Each step must:

modify only a few files

keep existing behavior

avoid large redesigns

Large refactors should be split into small tasks.

Immediate refactor goal

Separate the engine from the demo host.

Introduce:

CanvasEngine

CanvasEngine should own:

GraphDocument
CommandStack

The GLFW application becomes only a host layer.

Long term features

Planned engine capabilities include:

grid snapping

node grouping

hierarchical graphs

multiple graph views

custom node definitions

visual node styles

graph serialization

C API exposure

C# wrapper

web viewer support

First real use case

The first real use case for the engine will be a visual SHA-256 cycle explorer.

The system must support:

operation nodes
logic nodes
bit level drilldown
group collapsing
signal tracing

Example nodes:

Maj
Ch
Σ0
Σ1
Add
Rotate
XOR

Operators may expand to show bit-level subgraphs.

Summary

NodeCanvasGL is evolving from an OpenGL prototype into a reusable node graph engine with the following goals:

reusable architecture
host independence
clean graph model
extensible node definitions
C API for external integration

The GLFW application must become only a demo host, while the engine becomes a standalone reusable library.