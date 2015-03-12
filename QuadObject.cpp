
#include "QuadObject.h"

QuadObject::QuadObject()
{
	radius = 0;
	speed = 0;
	active = true;
	Identity(&world);
}

QuadObject::~QuadObject()
{
	quad = NULL;
}

void QuadObject::draw()
{
	if (!active)
		return;
    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        quad->draw();
    }
		/*box->draw();*/
}

void QuadObject::init(Quad *b, float r, Vector3 pos, Vector3 vel, float sp, float s)
{
	quad = b;
	radius = r;
	radius *= 1.00; //fudge factor
	position = pos;
	velocity = vel;
	speed = sp;
	scale = s;
	radiusSquared = radius * radius;
}

void QuadObject::update(float dt)
{
	quad->update(dt);
	position += velocity*dt;
	Identity(&world);
	Translate(&world, position.x, position.y, position.z);

}

void QuadObject::setAngleX(float val) {
	quad->setRotXAngle(val);
}

bool QuadObject::collided(QuadObject *QuadObject)
{
	Vector3 diff = position - QuadObject->getPosition();
	float length = D3DXVec3LengthSq(&diff);
	float radii = radiusSquared + QuadObject->getRadiusSquare();
	if (length <= radii)
		return true;
	return false;
}