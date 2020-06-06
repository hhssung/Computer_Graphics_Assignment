#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec3 norm;
in vec2 tc;

// uniform variable
uniform int color;

// the only output variable
out vec4 fragColor;

void main()
{
	if(color == 0){
		fragColor = vec4(tc.xy, 0, 1);
	}else if(color == 1){
		fragColor = vec4(tc.xxx,1);
	}else{
		fragColor = vec4(tc.yyy,1);
	}
}
