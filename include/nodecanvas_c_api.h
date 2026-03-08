#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NC_Engine NC_Engine;

NC_Engine* nc_engine_create();
void nc_engine_destroy(NC_Engine* engine);

uint32_t nc_create_node(
    NC_Engine* engine,
    const char* nodeType,
    float x,
    float y
);

bool nc_connect(
    NC_Engine* engine,
    uint32_t fromNode,
    uint32_t fromConnector,
    uint32_t toNode,
    uint32_t toConnector
);

#ifdef __cplusplus
}
#endif
