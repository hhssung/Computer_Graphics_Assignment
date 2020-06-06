#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	//float	theta=0.0f;			// rotation angle
	float	x_speed = 1.0f;
	float	y_speed = 1.0f;		// speed of x, y
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(std::vector<circle_t>& circles);
	void	circle_t::wall_collide();	// Ball-wall collide function
	void	circle_t::collide(std::vector<circle_t>& circles);	// Ball-Ball collide function

};


// Ball-Ball collide function
void circle_t::collide(std::vector<circle_t>& circles)
{
	bool flag = true;	// if the flag is true, no collision

	for (auto& c : circles)
	{
		float dist = (center - c.center).length();	// ball-ball distance

		if (dist < radius + c.radius && dist!=0)	// if collides & except me
		{
			float mass = radius * radius;
			float c_mass = c.radius * c.radius;
			// Perfect elastic collision
			float new_xspeed = (mass - c_mass) / (mass + c_mass) * x_speed + 2.00000f * c_mass / (mass + c_mass) * c.x_speed;
			float new_yspeed = (mass - c_mass) / (mass + c_mass) * y_speed + 2.00000f * c_mass / (mass + c_mass) * c.y_speed;
			float c_new_xspeed = 2.00000f * mass / (mass + c_mass) * x_speed + (-mass + c_mass) / (mass + c_mass) * c.x_speed;
			float c_new_yspeed = 2.00000f * mass / (mass + c_mass) * y_speed + (-mass + c_mass) / (mass + c_mass) * c.y_speed;
			// update Speeds
			x_speed = new_xspeed;
			y_speed = new_yspeed;
			c.x_speed = c_new_xspeed;
			c.y_speed = c_new_yspeed;
			//update flag
			flag = false;

			//update positions
			c.center.x = c.center.x + c.x_speed;
			c.center.y = c.center.y + c.y_speed;
			//to avoid BRRRRR
			while ((center - c.center).length() < radius + c.radius)
			{
				center.x = center.x + 0.50000f*x_speed;
				center.y = center.y + 0.50000f*y_speed;
			}
			center.x = center.x + x_speed;
			center.y = center.y + y_speed;


			
		}
	}
	if (flag == 1)	// no collide
	{
		center.x = center.x + x_speed;
		center.y = center.y + y_speed;
	}
}


// Ball - wall collide function
void circle_t::wall_collide()
{
	// if the ball crashes wall
	if (center.x + radius > 1.5f || center.x - radius < -1.5f)
	{
		//change the direction of the speed
		x_speed = -x_speed;
		// to avoid BRRRRRR
		while (center.x + radius > 1.5f || center.x - radius < -1.5f)
		{
			if (center.x + radius > 1.5f)
			{
				center.x = center.x - 0.00100f;
			}
			else{
				center.x = center.x + 0.00100f;
			}
		}
	}
	// if the ball crashes wall
	if (center.y + radius > 1.0f|| center.y - radius < -1.0f)
	{
		//change the direction of the speed
		y_speed = -y_speed;
		// to avoid BRRRRRR
		while (center.y + radius > 1.0f || center.y - radius < -1.0f)
		{
			if (center.y + radius > 1.0f)
			{
				center.y = center.y - 0.00100f;
			}
			else {
				center.y = center.y + 0.00100f;
			}
		}
	}
}


// Create Circles
inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;
	srand((unsigned int)time(NULL));	//to create random numbers;
	float ran_x, ran_y;
	float color_ran[4];
	float ran_size;
	float arr[3][25];	// Array containing x, y, and radius information for each circle
	float ran_xspeed, ran_yspeed;
	bool flag;	// Check for overlapping circles
	float comp_x, comp_y, comp_size;

	// Create 25 circles. 
	for (int i = 0; i < 25; i++)
	{
		do
		{
			flag = 0;
			// Random distance
			// x is -1.5f~1.5f, y is -1f~1f
			ran_x = (float)((rand() % 3000) - 1500) / 1000;
			ran_y = (float)((rand() % 2000) - 1000) / 1000;
			// Random radius 0.05f~0.15f 
			ran_size = (float)(rand() % 100) / 1000 + 0.05f;
			// Random colors
			for (int j = 0; j < 4; j++)
			{
				color_ran[j] = (float)(rand() % 1000) / 1000;
			}
			// Random x,y speed. -0.02f~0.02f
			ran_xspeed = (float)((rand() % 4000) - 2000) / 100000;
			ran_yspeed = (float)((rand() % 4000) - 2000) / 100000;

			// Check for overlapping circles
			for (int j = 0; j < i; j++)
			{
				comp_x = ran_x - arr[0][j];
				comp_y = ran_y - arr[1][j];
				comp_size = ran_size + arr[2][j];
				// If the circle overlap
				if (comp_x * comp_x + comp_y * comp_y <= comp_size * comp_size)
				{
					flag = 1;
					break;
				}
			}
			// check the collions with walls
			if (abs(ran_x + 1.5f) < ran_size || abs(ran_x - 1.5f) < ran_size || abs(ran_y + 1.0f) < ran_size || abs(ran_y - 1.0f) < ran_size)
			{
				flag = 1;
			}

			// No overlapping circles, Add the circle's info into array
			if (flag == 0)
			{
				arr[0][i] = ran_x;
				arr[1][i] = ran_y;
				arr[2][i] = ran_size;
			}

		} while (flag == 1);	// Continue looping until the circles do not overlap

		c = { vec2(ran_x, ran_y), ran_size, ran_xspeed, ran_yspeed, vec4(color_ran[0],color_ran[1],color_ran[2],color_ran[3]), };
		circles.emplace_back(c);

	}
	return circles;
}

inline void circle_t::update(  std::vector<circle_t>& circles  )
{
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	//collision handling
	wall_collide();
	collide(circles);

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix*scale_matrix;
}

#endif
