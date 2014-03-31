#ifndef __COLLISION__H
#define __COLLISION__H

#include <glm\glm.hpp>
#include <vector>

namespace Collision
{

	struct AABB
	{
		float x[2], y[2], z[2];

		AABB(const float& xmin, const float& xmax, const float& ymin, const float& ymax, const float& zmin, const float& zmax)
		{
			x[0] = xmin; x[1] = xmax;
			y[0] = ymin; y[1] = ymax;
			z[0] = zmin; z[1] = zmax;
		}
	};

	struct CollisionPacket
	{
		glm::vec3 radius;
		glm::vec3 r3Pos, r3Vel;
		glm::vec3 eVel, eVelNormal;
		glm::vec3 eBasePoint;
	
		bool foundCollision;
		glm::vec3 contactPoint;
		double nearestDistance;
	};

	struct Sphere
	{
		float radius;
		glm::vec3 position;

		Sphere(const glm::vec3& p, const float& r)
		{
			radius = r;
			position = p;
		}
	};

	struct Triangle
	{
		glm::vec3 verts[3];
		glm::vec3 normal;

		Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
		{
			verts[0] = v0;
			verts[1] = v1;
			verts[2] = v2;
			normal = glm::normalize(glm::cross(v1-v0,v2-v0));
		}
	};

	struct Terrain
	{
		std::vector<Triangle> tris;

		Terrain(const char* path);
		void QuereyAABB(const AABB& box, std::vector<Triangle>& out);
	};

	struct Plane 
	{
		float equation[4];
		glm::vec3 origin;
		glm::vec3 normal;
		Plane(const glm::vec3& origin, const glm::vec3& normal);
		Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
		bool IsFrontFacingTo(const glm::vec3& direction) const;
		double SignedDistanceTo(const glm::vec3& point) const;
	};
	float SquaredLength(const glm::vec3& v);

	bool GetLowestRoot(float a, float b, float c, float maxR, float* root);

	bool CheckPointInTriangle(const glm::vec3& point, const glm::vec3& pa, const glm::vec3& pb, const glm::vec3& pc);	void CheckTriangle(CollisionPacket* colPackage, const glm::vec3& p1,const glm::vec3& p2,const glm::vec3& p3);}
#endif