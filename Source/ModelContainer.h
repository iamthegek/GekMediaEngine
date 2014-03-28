#ifndef __MODELMANAGE__H
#define __MODELMANAGE__H

#include "BaseModel.h"
#include "StaticModel.h"
#include "AnimatedModel.h"
#include "DeferredBuffers.h"

class ModelContainer
{
public:
	ModelContainer();
	~ModelContainer();

	void AddAnimatedModel(AnimatedModel* model);
	void AddStaticModel(StaticModel* model);

	void Update(const float& deltaTime);
	void Render(DeferredBuffers* bufs, const float& deltaTime);

protected:
	std::vector<AnimatedModel*> animatedModels;
	std::vector<StaticModel*> staticModels;
	int lodCalcTimer;

	Shader sdShader;  //static, diffuse
	Shader sdnShader; //static, diffuse + normal
	Shader sShadowShader;
	Shader adShader;  //animated, diffuse
	Shader adnShader; //animated, diffuse + normal
	Shader aShadowShader;
};

#endif