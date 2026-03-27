# ScopeCanvas PlantUML set (per module)

Volgorde (zoals module-opbouw in CMake):

1. `01-ScopeCanvasCore-detail.puml`
2. `01-ScopeCanvasCore-deps.puml`
3. `02-ScopeCanvasRouting-detail.puml`
4. `02-ScopeCanvasRouting-deps.puml`
5. `03-ScopeCanvasRender-detail.puml`
6. `03-ScopeCanvasRender-deps.puml`
7. `04-ScopeCanvasWidget-detail.puml`
8. `04-ScopeCanvasWidget-deps.puml`
9. `05-ScopeCanvasDemoGL-detail.puml`
10. `05-ScopeCanvasDemoGL-deps.puml`
11. `06-ScopeCanvasDemoImGui-detail.puml`
12. `06-ScopeCanvasDemoImGui-deps.puml`

Elke `*-detail.puml` bevat:
- classes/structs met fields + methods
- relaties tussen classes
- notities over status (af vs. skeleton/WIP)

Elke `*-deps.puml` toont:
- afhankelijkheid op class-niveau naar vorige modules.
