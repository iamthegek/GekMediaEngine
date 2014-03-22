#include "Particle.h"
#include <random>

ParticleEmitter::ParticleEmitter(const std::string& filename, const glm::vec3& f)
{
	texture = new Texture(filename);
	force = f;
}
ParticleEmitter::~ParticleEmitter()
{
	delete texture;
	particles.clear();
}

int ParticleEmitter::GetCurNumParticles() const
{
	return (int)particles.size();
}

bool ParticleEmitter::EmitWithVelocity(	const glm::vec3& pos,
										const float& radius,
										const int& num,
										const glm::vec3& direction,
										const glm::vec2& speedExtents,
										const glm::vec2& sizeExtents, const float& sizeDelta,
										const float& angle, const float& angleDelta,
										const glm::vec2& lifeExtents)
{
	for(int i = 0; i < num; i++)
	{
		if(GetCurNumParticles() < MAX_PARTICLES_PER_EMITTER)
			return false;

		Particle p;
		p.x = LimitedRandomF(-radius, radius) + pos.x;
		p.y = LimitedRandomF(-radius, radius) + pos.y;
		p.z = LimitedRandomF(-radius, radius) + pos.z;

		glm::vec3 vel = LimitedRandomF(speedExtents.x, speedExtents.y) * direction;
		p.dx = vel.x;
		p.dy = vel.y;
		p.dz = vel.z;

		p.size = LimitedRandomF(sizeExtents.x, sizeExtents.y);
		p.sizeDelta;

		p.angle = angle;
		p.angleDelta = angleDelta;

		p.life = LimitedRandomF(lifeExtents.x, lifeExtents.y);
		particles.push_back(p);
	}
	return true;
}

bool ParticleEmitter::EmitOmni(	const glm::vec3& pos,
								const float& radius,
								const int& num,
								const glm::vec2& speedExtents,
								const glm::vec2& sizeExtents, const float& sizeDelta,
								const float& angle,	const float& angleDelta,
								const glm::vec2& lifeExtents)
{
	for(int i = 0; i < num; i++)
	{
		if(GetCurNumParticles() < MAX_PARTICLES_PER_EMITTER)
			return false;

		Particle p;
		p.x = LimitedRandomF(-radius, radius) + pos.x;
		p.y = LimitedRandomF(-radius, radius) + pos.y;
		p.z = LimitedRandomF(-radius, radius) + pos.z;

		glm::vec3 dir = glm::normalize(glm::vec3(p.x, p.y, p.z) - pos);
		glm::vec3 vel = LimitedRandomF(speedExtents.x, speedExtents.y) * dir;
		p.dx = vel.x;
		p.dy = vel.y;
		p.dz = vel.z;

		p.size = LimitedRandomF(sizeExtents.x, sizeExtents.y);
		p.sizeDelta;

		p.angle = angle;
		p.angleDelta = angleDelta;

		p.life = LimitedRandomF(lifeExtents.x, lifeExtents.y);
		particles.push_back(p);
	}
	return true;
}

void ParticleEmitter::Update(const float& deltaTime)
{
	for(unsigned int i = 0; i < particles.size();)
	{
		particles[i].life -= deltaTime;
		if(particles[i].life < 0.0f)
		{
			particles.erase(particles.begin() + i);
			continue;
		}
		else
			i++;

		particles[i].dx += force.x * deltaTime;
		particles[i].dy += force.y * deltaTime;
		particles[i].dz += force.z * deltaTime;

		particles[i].x += particles[i].dx * deltaTime;
		particles[i].y += particles[i].dy * deltaTime;
		particles[i].z += particles[i].dz * deltaTime;

		particles[i].size += particles[i].sizeDelta * deltaTime;
		particles[i].angle += particles[i].angleDelta * deltaTime;
	}
}

float ParticleEmitter::LimitedRandomF(const float& start, const float& end)
{
	return ((float)rand() / RAND_MAX) * (end-start) + start;
}