
#ifndef QuadObject_H
#define QuadObject_H

#include "d3dUtil.h"


#include "Quad.h"
#include "GameObject.h"
#include "constants.h"

class QuadObject: public GameObject
{
public:

	QuadObject();
	~QuadObject();

	void init(Quad *b, float r, Vector3 pos, Vector3 vel, float sp, float s);
	void draw();
	void update(float dt);

	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	void setRadius(float r) {radius = r; radiusSquared = (scale*r)*(scale*r);}
	void setAngleX(float val);
	float getRadiusSquare() {return radiusSquared;}
	float getRadius() {return radius;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {scale = s; radiusSquared = (s*radius)*(s*radius);}
	float getScale() {return scale;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	bool collided(QuadObject *QuadObject);


private:
	Quad *quad; 
	Vector3 position;
	Vector3 velocity;
	float speed;
	float radius;
	float radiusSquared;
	bool active;
	Matrix world;
	float scale;
	ID3D10EffectTechnique* mTech;
};


#endif
