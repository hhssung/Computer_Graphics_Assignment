#pragma once
#ifndef __PLANET_H__
#define __PLANET_H__
#include "cgmath.h"
#include "cgut.h"

struct planet {
	float radius;	
	float rev_radius;
	float rev_angle;	//revolution
	float rot_angle;	//rotation

	//public functions
	planet(float putrev_radius, float putrev_angle, float putrot_angle, float putradius)
	{
		rev_radius = putrev_radius;
		rev_angle = putrev_angle;
		rot_angle = putrot_angle;
		radius = putradius;
	};
	static void planet::create_vertex_indices(std::vector<vertex>& VertexList, std::vector<uint>& IndexList);
	mat4 putmatrix(float time);
};

void planet::create_vertex_indices(std::vector<vertex>& VertexList, std::vector<uint>& IndexList)
{
	//printf("5");
	//36 edges latitude
	for (uint temp_theta = 0; temp_theta <= 36; temp_theta++)
	{
		//72 edges longitude
		for (uint temp_pi = 0; temp_pi <= 72; temp_pi++)
		{
			//calc each theta and pi
			float theta = PI * float(temp_theta) / float(36);
			float pi = PI * float(temp_pi) / float(36);	// 72 divided by 2

			//calc x,y,z like pdf
			float x = sin(theta) * cos(pi);
			float y = sin(theta) * sin(pi);
			float z = cos(theta);

			// push each vertices
			VertexList.push_back({ vec3(x, y, z), vec3(x, y, z), vec2(pi / (2.0f * PI), 1 - theta / PI) });
		}
	}

	//create buffers
	//36 edges latitude
	for (uint theta = 1; theta <= 36; theta++)
	{
		//72 edges longitude
		for (uint pi = 0; pi < 72; pi++)
		{
			// down, down, up point
			//printf("%f", theta * 73 + pi + 1);
			IndexList.push_back((theta - 1) * 73 + pi + 1);
			IndexList.push_back((theta - 1) * 73 + pi);
			IndexList.push_back(theta * 73 + pi + 1);
			// up, up, down point
			IndexList.push_back(theta * 73 + pi);
			IndexList.push_back(theta * 73 + pi + 1);
			IndexList.push_back((theta - 1) * 73 + pi);
		}
	}
}

//update matrices of planet
mat4 planet::putmatrix(float time) {
	//printf("4");
	float getradius = radius;
	float getrev_radius = rev_radius;
	float rotation_angle = time * rot_angle;
	float revolution_angle = time * rev_angle;

	mat4 model_matrix = mat4::rotate(vec3(0, 0, 1), revolution_angle)* mat4::translate(getrev_radius, 0, 0)* mat4::rotate(vec3(0, 0, 1), rotation_angle)* mat4::scale(getradius, getradius, getradius);
	return model_matrix;
}

#endif