#ifndef __PARTICLE__H
#define __PARTICLE__H

#include "glm\glm.hpp"
#include <GL\glew.h>
#include "Texture.h"
#include <vector>

const int MAX_PARTICLES_PER_EMITTER = 300;

struct Particle
{
	float x, y, z;				//position
	float dx, dy, dz;			//velocity
	GLfloat life;				//time until death
	GLfloat size, sizeDelta;	//size and size velocity
	GLfloat angle, angleDelta;	//angle and angle velocity
};

class ParticleEmitter
{
public:
	ParticleEmitter(const std::string& filename, const glm::vec3& f);
	~ParticleEmitter();

	int GetCurNumParticles() const;

	bool EmitWithVelocity(	const glm::vec3& pos,
							const float& radius,
							const int& num,
							const glm::vec3& direction,
							const glm::vec2& speedExtents,
							const glm::vec2& sizeExtents, const float& sizeDelta,
							const float& angle, const float& angleDelta,
							const glm::vec2& lifeExtents);

	bool EmitOmni(	const glm::vec3& pos,
					const float& radius,
					const int& num,
					const glm::vec2& speedExtents,
					const glm::vec2& sizeExtents, const float& sizeDelta,
					const float& angle,	const float& angleDelta,
					const glm::vec2& lifeExtents);

	void Update(const float& deltaTime);

	static float LimitedRandomF(const float& start, const float& end);

public:
	Texture* texture;
	std::vector<Particle> particles;
	glm::vec3 force;
};

#endif