// vertex attributes
in vec3 position;
in vec3 normal;
in vec2 texcoord;

// matrices
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform float ratio;

out vec3 norm;
out vec2 tc;
out int color;

void main()
{
	vec4 wpos = model_matrix * vec4(position,1);
    vec4 epos = view_matrix * wpos;
    gl_Position = projection_matrix * epos;
	
	tc = texcoord;
	// pass eye-coordinate normal to fragment shader
}
