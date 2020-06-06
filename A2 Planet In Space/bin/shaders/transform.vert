// vertex attributes
in vec3 position;
in vec3 normal;
in vec2 texcoord;

// matrices
uniform mat4 model_matrix;
uniform mat4 view_projection_matrix;
uniform float ratio;

out vec3 norm;
out vec2 tc;
out int color;

void main()
{
	gl_Position = view_projection_matrix * model_matrix * vec4(position, 1);
	if(ratio > 1)
	{
		gl_Position.xy *= vec2(1/ratio,1);
	}else{
		gl_Position.xy *= vec2(1,ratio);
	}
	tc = texcoord;
	// pass eye-coordinate normal to fragment shader
	norm = normalize(mat3(view_projection_matrix)*normal);
}
