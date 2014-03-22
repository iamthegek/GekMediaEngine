#ifndef __SCRQUADMESH__H
#define __SCRQUADMESH__H

#include <vector>
#include <GL\glew.h>

struct ScreenVertex 
{	
	GLfloat x, y, z; 
};

struct ScreenQuadMesh
{
	GLuint vbo, ind;
	std::vector<ScreenVertex> data;
	std::vector<GLuint> indices;

	void SetUp()
	{
		//create screen quad
		data.resize(4); 
		data[0].x =-1.0f; data[0].y = 1.0f; data[0].z = 0.0f; 
		data[1].x =-1.0f; data[1].y =-1.0f; data[1].z = 0.0f; 
		data[2].x = 1.0f; data[2].y =-1.0f; data[2].z = 0.0f; 
		data[3].x = 1.0f; data[3].y = 1.0f; data[3].z = 0.0f;
		indices.push_back(0);indices.push_back(1);indices.push_back(2);
		indices.push_back(0);indices.push_back(2);indices.push_back(3);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(ScreenVertex), &data[0], GL_STATIC_DRAW);
		glGenBuffers(1, &ind);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void Render()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind);
		glEnableVertexAttribArray(0); 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), 0);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
	}
	void Destroy()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ind);
	}
};

#endif