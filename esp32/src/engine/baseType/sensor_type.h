#pragma once
typedef enum {
    ENGINE_PAD,
    ENGINE_CYMBAL,
    ENGINE_SNARE,
    ENGINE_HIHAT
} engine_type_t;


typedef struct {
   engine_type_t type;
   void *object;
   void (*update)(void *);
}  engine_t;