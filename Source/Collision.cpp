#include "Collision.h"

using namespace glm;

Collision::Plane::Plane(const vec3& origin, const vec3& normal) 
{
	this->normal = normal;
	this->origin = origin;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
}

Collision::Plane::Plane(const vec3& p1, const vec3& p2, const vec3& p3)
{
	normal = normalize(cross((p2-p1),(p3-p1)));
	origin = p1;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
}

bool Collision::Plane::IsFrontFacingTo(const vec3& direction) const 
{
	double d = dot(normal,direction);
	return (d <= 0);
}

double Collision::Plane::SignedDistanceTo(const vec3& point) const 
{
	return dot(point,normal) + equation[3];
}

float Collision::SquaredLength(const vec3& v)
{
	return dot(v,v);
}

typedef unsigned int uint32;
#define checkin(a) ((uint32&) a)
bool Collision::CheckPointInTriangle(const vec3& point, const vec3& pa,const vec3& pb, const vec3& pc)
{
	vec3 e10=pb-pa;
	vec3 e20=pc-pa;
	float a = dot(e10,e10);
	float b = dot(e10,e20);
	float c = dot(e20,e20);
	float ac_bb=(a*c)-(b*b);
	vec3 vp(point.x-pa.x, point.y-pa.y, point.z-pa.z);
	float d = dot(vp,e10);
	float e = dot(vp,e20);
	float x = (d*c)-(e*b);
	float y = (e*a)-(d*b);
	float z = x+y-ac_bb;
	return ((checkin(z)& ~(checkin(x)|checkin(y)) ) & 0x80000000);
}

bool Collision::GetLowestRoot(float a, float b, float c, float maxR, float* root) 
{
	float determinant = b*b - 4.0f*a*c;
	if (determinant < 0.0f) return false;
	float sqrtD = sqrtf(determinant);
	float r1 = (-b - sqrtD) / (2*a);
	float r2 = (-b + sqrtD) / (2*a);
	if (r1 > r2) 
	{
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}
	if (r1 > 0 && r1 < maxR) 
	{
		*root = r1;
		return true;
	}
	if (r2 > 0 && r2 < maxR) 
	{
		*root = r2;
		return true;
	}
	return false;
}

void Collision::CheckTriangle(CollisionPacket* colPackage, const vec3& p1,const vec3& p2,const vec3& p3)
{
	Plane trianglePlane(p1,p2,p3);
	if (trianglePlane.IsFrontFacingTo(colPackage->eVelNormal)) 
	{
		double t0, t1;
		bool embeddedInPlane = false;
		double signedDistToTrianglePlane = trianglePlane.SignedDistanceTo(colPackage->eBasePoint);
		float normalDotVelocity = dot(trianglePlane.normal,colPackage->eVel);
		if (normalDotVelocity == 0.0f) 
		{
			if (fabs(signedDistToTrianglePlane) >= 1.0f) return;
			embeddedInPlane = true;
			t0 = 0.0;
			t1 = 1.0;
		}
		else 
		{
			t0=(-1.0-signedDistToTrianglePlane)/normalDotVelocity;
			t1=( 1.0-signedDistToTrianglePlane)/normalDotVelocity;
			if (t0 > t1) 
			{
				double temp = t1;
				t1 = t0;
				t0 = temp;
			}
			
			if (t0 > 1.0f || t1 < 0.0f) return;
			if (t0 < 0.0) t0 = 0.0;
			if (t1 < 0.0) t1 = 0.0;
			if (t0 > 1.0) t0 = 1.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		vec3 collisionPoint;
		bool foundCollison = false;
		float t = 1.0;

		if (!embeddedInPlane) 
		{
			vec3 planeIntersectionPoint = (colPackage->eBasePoint-trianglePlane.normal)+ float(t0) * colPackage->eVel;
			if(CheckPointInTriangle(planeIntersectionPoint,p1,p2,p3))
			{
				foundCollison = true;
				t = t0;
				collisionPoint = planeIntersectionPoint;
			}
		}

		if (foundCollison == false) 
		{
			vec3 velocity = colPackage->eVel;
			vec3 base = colPackage->eBasePoint;
			float velocitySquaredLength = SquaredLength(velocity);
			float a,b,c, newT;
			a = velocitySquaredLength;
			glm::vec3 triPoints[3] = {p1, p2, p3};

			for(int i = 0; i < 3; i++)
			{
				b = 2.0f*dot(velocity,base-triPoints[i]);
				c = SquaredLength(triPoints[i]-base) - 1.0f;
				if(GetLowestRoot(a,b,c, t, &newT)) 
				{
					t = newT;
					foundCollison = true;
					collisionPoint = triPoints[i];
				}
			}

			glm::vec3 triEdges[3] = {p2-p1, p3-p2, p1-p3};
			glm::vec3 baseToVertex;
			float edgeSquaredLength, edgeDotVelocity, edgeDotBaseToVertex;
			for(int i = 0; i < 3; i++)
			{
				baseToVertex = triPoints[i] - base;
				edgeSquaredLength = SquaredLength(triEdges[i]);
				edgeDotVelocity = dot(triEdges[i],velocity);
				edgeDotBaseToVertex = dot(triEdges[i],baseToVertex);
				a = edgeSquaredLength*-velocitySquaredLength + edgeDotVelocity * edgeDotVelocity;
				b = edgeSquaredLength*(2.0f*dot(velocity,baseToVertex))- 2.0f*edgeDotVelocity*edgeDotBaseToVertex;
				c = edgeSquaredLength*(1.0f-SquaredLength(baseToVertex))+edgeDotBaseToVertex*edgeDotBaseToVertex;
				if (GetLowestRoot(a,b,c, t, &newT)) 
				{
					float f=(edgeDotVelocity*newT-edgeDotBaseToVertex) / edgeSquaredLength;
					if (f >= 0.0 && f <= 1.0) 
					{
						t = newT;
						foundCollison = true;
						collisionPoint = triPoints[i] + f*triEdges[i];
					}
				}
			}
		}
		if (foundCollison == true) 
		{
			float distToCollision = t*length(colPackage->eVel);
			if (colPackage->foundCollision == false ||	distToCollision < colPackage->nearestDistance) 
			{
				colPackage->nearestDistance = distToCollision;
				colPackage->contactPoint=collisionPoint;
				colPackage->foundCollision = true;
			}
		}
	}
}