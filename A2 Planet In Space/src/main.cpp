#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility

//*************************************
// global constants
static const char*	window_name = "cgbase - transform - Stanford dragon colored by normals";
static const char*	vert_shader_path = "../bin/shaders/transform.vert";
static const char*	frag_shader_path = "../bin/shaders/transform.frag";

//*************************************
// common structures
struct camera
{
	mat4 view_projection_matrix = { 0,1,0,0,0,0,1,0,-1,0,0,1,0,0,0,1 };
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1280, 720); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint	vertex_array = 0;	// ID holder for vertex array object

//*************************************
// global variables
int		frame = 0;		// index of rendering frames
bool	b_index_buffer = true;			// use index buffering?
bool	b_wireframe = false;
bool	rotate = true;
//defaule radius is 1
float	rotate_angle = 0.0f;
int		color = 0;

//*************************************
// scene objects
camera	cam;

//*************************************
std::vector<vertex>	spherevertex;	// host-side vertices
std::vector<uint> sphereindices;	//host-side indices

//*************************************
void update()
{
	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "color" );			if(uloc>-1) glUniform1i( uloc, color );		
	uloc = glGetUniformLocation( program, "view_projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_projection_matrix );	// update the projection matrix (covered later in viewing lecture)
	uloc = glGetUniformLocation( program, "ratio"); glUniform1f (uloc, window_size.x / float(window_size.y));	//give ratio of window
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// notify GL that we use our own program
	glUseProgram(program);
	
	// bind vertex array object
	glBindVertexArray(vertex_array);

	//printf("%f", prevtime);
	if (rotate == false)
	{
		// do nothing
	}
	else {
		rotate_angle += 0.02f;
	}

	// build the model matrix
	mat4 model_matrix = 
		mat4::rotate(vec3(0, 0, 1), rotate_angle);
	
	GLint uloc;
	uloc = glGetUniformLocation(program, "model_matrix");		 
	glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
	glDrawElements(GL_TRIANGLES, sphereindices.size(), GL_UNSIGNED_INT, nullptr);

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf("[help]\n");
	printf("- press ESC or 'q' to terminate the program\n");
	printf("- Press F1 or 'h' to see help\n");
	printf("- press 'd' to toggle color (tc.xy,0) (tc.xxx) (tc.yyy)\n");
	printf("- press 'r' to toggle rotation\n");
	printf("- press 'w' to toggle wireframe\n");
	printf("\n");
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		// 색깔 바꾸기
		else if (key == GLFW_KEY_D)
		{
			color++;
			if (color >= 3) { color = color % 3; }
			printf("> using %s\n", color == 0 ? "(tc.xy, 0)" : color == 1 ? "(tc.xxx)" : "(tc.yyy)");
		}
		//wireframe / solid 모드 바꾸기
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
		//회전 모드 바꾸기
		else if (key == GLFW_KEY_R) {
			rotate = !rotate;
			printf("> %s using rotation\n", rotate ? "Start" : "Stop");
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_PRESS )
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		printf( "> Left mouse button pressed at (%d, %d)\n", int(pos.x), int(pos.y) );
	}
}

void motion( GLFWwindow* window, double x, double y )
{
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);	// turn on depth tests

	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

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
			spherevertex.push_back({ vec3(1.0f * x,1.0f * y,1.0f * z), vec3(x,y,z), vec2(pi / (2.0f * PI), 1 - theta / PI) });	
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
			sphereindices.push_back((theta - 1) * 73 + pi + 1);
			sphereindices.push_back((theta - 1) * 73 + pi);
			sphereindices.push_back(theta * 73 + pi + 1);
			// up, up, down point
			sphereindices.push_back(theta * 73 + pi);
			sphereindices.push_back(theta * 73 + pi + 1);
			sphereindices.push_back((theta - 1) * 73 + pi);
		}
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * spherevertex.size(), &spherevertex[0], GL_STATIC_DRAW);	

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * sphereindices.size(), &sphereindices[0], GL_STATIC_DRAW);
	
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); }

	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
