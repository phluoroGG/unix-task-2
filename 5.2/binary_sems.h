#ifndef BINARY_SEMS_H
#define BINARY_SEMS_H

typedef enum { FALSE, TRUE } Boolean;

extern Boolean bsUseSemUndo;
extern Boolean bsRetryOnEintr;

int initSemAvailable(int semId, int semNum);

int initSemInUse(int semId, int semNum);

int reserveSem(int semId, int semNum);

int releaseSem(int semId, int semNum);

#endif
