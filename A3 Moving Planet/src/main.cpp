// Assignment A3

#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "planet.h"		// planet module
#include "trackball.h"

//*************************************
// global constants
static const char*	window_name = "cgbase - transform - Stanford dragon colored by normals";
static const char*	vert_shader_path = "../bin/shaders/transform.vert";
static const char*	frag_shader_path = "../bin/shaders/transform.frag";

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3(100, 100, 100);
	vec3	at = vec3(0, 0, 0);
	vec3	up = vec3(0, 1, 0);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
	float	aspect = 0.0f;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
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

bool	Left_shift = false;
bool	Left_Ctrl = false;

bool	zooming = false;
bool	panning = false;

float TimeScaleFactor = 1.0f;

//defaule radius is 1
float	rotate_angle = 0.0f;
int		color = 0;

//*************************************
// scene objects
camera	cam;
trackball  tb;

//*************************************
std::vector<vertex>	spherevertex;	// host-side vertices
std::vector<uint> sphereindices;	//host-side indices

std::vector<planet> planets;


//*************************************
void update()
{
	cam.aspect= 1280 / float(720);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect, cam.dnear, cam.dfar);

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program , "view_matrix");	 glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation( program, "color" );			if(uloc>-1) glUniform1i( uloc, color );		
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );	// update the projection matrix (covered later in viewing lecture)
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
	/*
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
	*/

	static float time = 0.0f;
	static float prevtime = 0.0f;

	if (rotate == false)
	{
		//do nothing
	}
	else {
		time += (float(glfwGetTime()) - prevtime) * TimeScaleFactor;
	}
	prevtime = float(glfwGetTime());
	/*
	uloc = glGetUniformLocation(program, "model_matrix");
	glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
	glDrawElements(GL_TRIANGLES, sphereindices.size(), GL_UNSIGNED_INT, nullptr);
*/
	for (size_t k = 0; k < planets.size(); k++) {
		GLint uloc;
		if ((uloc = glGetUniformLocation(program, "model_matrix")) > -1)
		{
			glUniformMatrix4fv(uloc, 1, GL_TRUE, planets.at(k).putmatrix(time));
		}
		// Update the uniform model matrix and render
		glDrawElements(GL_TRIANGLES, sphereindices.size(), GL_UNSIGNED_INT, nullptr);
	}


	
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
	printf("- press 'left_control + left_mouse' or 'middle_mouse' for panning\n");
	printf("- press 'left_shift + left_mouse' or 'right_mouse' for zooming\n");
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
		else if (key == GLFW_KEY_LEFT_SHIFT)
		{
			Left_shift = true;
		}
		else if (key == GLFW_KEY_LEFT_CONTROL)
		{
			Left_Ctrl = true;
		}

	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_LEFT_SHIFT) {
			Left_shift = false;
		}
		else if (key == GLFW_KEY_LEFT_CONTROL) {
			Left_Ctrl = false;
		}
	}


}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
	vec2 npos = cursor_to_ndc(pos, window_size);
	if(button==GLFW_MOUSE_BUTTON_LEFT)	//shift: zooming, ctrl: panning
	{
		if (action == GLFW_PRESS) { 
			if (Left_shift == true && Left_Ctrl == false)
			{
				zooming = true;
			}
			else if (Left_shift == false && Left_Ctrl == true)
			{
				panning = true;
			}
			tb.begin(cam.view_matrix, npos); 
		}
		else if (action == GLFW_RELEASE) { tb.end(); zooming = false; panning = false; }
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)	//zooming
	{
		if (action == GLFW_PRESS) { tb.begin(cam.view_matrix, npos); zooming = true; }
		else if (action == GLFW_RELEASE) { tb.end(); zooming = false; }
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) //panning
	{
		if (action == GLFW_PRESS) { tb.begin(cam.view_matrix, npos); panning = true; }
		else if (action == GLFW_RELEASE) { tb.end(); panning = false; }
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if (!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
	

	cam.view_matrix = tb.update(npos, zooming, panning);
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

	//Create vertex, index
	planet::create_vertex_indices(spherevertex, sphereindices);

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

	// Sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune
	planets.push_back(planet(0.0f, 0.0f, 1.0f / 2.50f, 4.50f)); 
	planets.push_back(planet(8.0f, 1.0f / 1.0f, 1.0f / 4.00f, 0.50f)); 
	planets.push_back(planet(13.0f, 1.0f / 2.5f, -1.0f / 5.50f, 0.70f)); 
	planets.push_back(planet(20.5f, 1.0f / 3.5f, 1.0f / 1.00f, 0.65f)); 
	planets.push_back(planet(26.5f, 1.0f / 5.5f, 1.0f / 1.00f, 0.35f)); 
	planets.push_back(planet(40.0f , 1.0f / 13.0f, 1.0f / 0.50f, 2.55f)); 
	planets.push_back(planet(50.0f , 1.0f / 18.0f, 1.0f / 0.50f, 2.35f));
	planets.push_back(planet(65.0f, 1.0f / 23.0f, -1.0f / 1.00f, 1.55f)); 
	planets.push_back(planet(90.0f, 1.0f / 26.0f, 1.0f / 1.00f, 1.50f));

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
