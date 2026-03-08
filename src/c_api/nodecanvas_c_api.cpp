#include "../../include/nodecanvas_c_api.h"

#include "Engine/CanvasEngine.h"

#include <new>

struct NC_Engine {
    CanvasEngine* engine = nullptr;
};

NC_Engine* nc_engine_create() {
    NC_Engine* handle = new (std::nothrow) NC_Engine;
    if (handle == nullptr) {
        return nullptr;
    }

    handle->engine = new (std::nothrow) CanvasEngine;
    if (handle->engine == nullptr) {
        delete handle;
        return nullptr;
    }

    return handle;
}

void nc_engine_destroy(NC_Engine* engine) {
    if (engine == nullptr) {
        return;
    }

    delete engine->engine;
    engine->engine = nullptr;
    delete engine;
}

uint32_t nc_create_node(
    NC_Engine* engine,
    const char* nodeType,
    float x,
    float y
) {
    if (engine == nullptr || engine->engine == nullptr || nodeType == nullptr) {
        return 0;
    }

    Node* node = engine->engine->graph().createNodeOfType(nodeType, {x, y});
    if (node == nullptr) {
        return 0;
    }

    return node->id;
}

bool nc_connect(
    NC_Engine* engine,
    uint32_t fromNode,
    uint32_t fromConnector,
    uint32_t toNode,
    uint32_t toConnector
) {
    if (engine == nullptr || engine->engine == nullptr) {
        return false;
    }

    return engine->engine->graph().createEdge(
        fromNode,
        fromConnector,
        toNode,
        toConnector
    );
}

void nc_undo(NC_Engine* engine) {
    if (engine == nullptr || engine->engine == nullptr) {
        return;
    }

    engine->engine->commands().undo();
}

void nc_redo(NC_Engine* engine) {
    if (engine == nullptr || engine->engine == nullptr) {
        return;
    }

    engine->engine->commands().redo();
}
