//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  RailModels.h
//  Header file for a collection of models for a railway scene
//  ========================================================================

extern GLUquadricObj* q;

void loadTexture();

struct Point;

typedef struct Point Point;

Point initPoint(float x, float y, float z);

void normal(float, float, float,
			float, float, float,
			float, float, float);

Point trackCircuit(float theta);

void floor();

int getPoints();

void tracks(float, float, float*, float*, int); 

void road();

void car();

void engine(float);

void woodWagon(float);

void wagon(float);

void barrierArm(float);

void trafficLight(float);

void tunnel();

void station(float, float);

void smokeParticle(float);
