#ifdef __cplusplus
extern "C" {
#endif

typedef struct NC_Engine NC_Engine;

NC_Engine* nc_engine_create();
void nc_engine_destroy(NC_Engine* engine);

#ifdef __cplusplus
}
#endif
