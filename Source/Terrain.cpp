#include "Collision.h"
#include <fstream>
#include <iostream>

namespace Collision
{
	Terrain::Terrain(const char* path)
	{
		std::ifstream file(path);
		if(file.is_open())
		{
			char buf[256];
			std::vector<glm::vec3> tmpverts;
			std::vector<int> tmptriangles;
			while(!file.eof())
			{
				file.getline(buf, 256);
				if(buf[0] == 'v' && buf[1] == ' ')
				{
					float x, y, z;
					sscanf_s(buf, "v %f %f %f", &x, &y, &z);
					tmpverts.push_back(glm::vec3(x,y,z));
				}
				else if(buf[0] == 'f' && buf[1] == ' ')
				{
					int v1,v2,v3,n1,n2,n3,t1,t2,t3;
					sscanf_s(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
					tmptriangles.push_back(v1-1);
					tmptriangles.push_back(v2-1);
					tmptriangles.push_back(v3-1);
				}
			}
			file.close();

			for(unsigned int i = 0; i < tmptriangles.size(); i+=3)
			{
				glm::vec3 v0 = tmpverts[tmptriangles[i+0]];
				glm::vec3 v1 = tmpverts[tmptriangles[i+1]];
				glm::vec3 v2 = tmpverts[tmptriangles[i+2]];

				Triangle newTri(v0, v1, v2);
				tris.push_back(newTri);
			}
			tmptriangles.clear();
			tmpverts.clear();
		}
		else
		{
			std::cout << "Error, couldn't open " << path << '\n';
		}
	}

	void Terrain::QuereyAABB(const AABB& box, std::vector<Triangle>& out)
	{
		for(unsigned int i = 0; i < tris.size(); i++)
		{
			Triangle* t = &tris[i];
			float xmin, xmax, ymin, ymax, zmin, zmax;
			xmin = ymin = zmin = std::numeric_limits<float>::max();
			xmax = ymax = zmax =-std::numeric_limits<float>::max();
			for(int j = 0; j < 3; j++)
			{
				if(t->verts[j].x < xmin) xmin = t->verts[j].x;
				if(t->verts[j].x > xmax) xmax = t->verts[j].x;
				if(t->verts[j].y < ymin) ymin = t->verts[j].y;
				if(t->verts[j].y > ymax) ymax = t->verts[j].y;
				if(t->verts[j].z < zmin) zmin = t->verts[j].z;
				if(t->verts[j].z > zmax) zmax = t->verts[j].z;
			}

			AABB triBox(xmin, xmax, ymin, ymax, zmin, zmax);
			if(box.x[0] > triBox.x[1]) continue;
			if(box.x[1] < triBox.x[0]) continue;
			if(box.y[0] > triBox.y[1]) continue;
			if(box.y[1] < triBox.y[0]) continue;
			if(box.z[0] > triBox.z[1]) continue;
			if(box.z[1] < triBox.z[0]) continue;

			out.push_back(tris[i]);
		}
	}
};