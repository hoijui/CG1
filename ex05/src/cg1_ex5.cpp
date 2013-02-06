#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

#include <glm/glm.hpp>

using namespace glm;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Mesh.h"
#include "Ray.h"
#include "Scene.h"

#define print(var) std::cout << var << std::endl;

static void toRGB(float& x);
static void toRGB(vec3& color);

// global variables //////////////////////////////////////////
int _id_window, _id_screen, _id_world;
int _win_w = 256;
int _win_h = 256;
int _win_gap = 8;

vec3 eye(0, 5, 20);
vec3 up(0, 1, 0);
vec3 center(0, 0, 0);
float fovy = 60;
float zNear = 0.1;
float zFar = 100.0;

vec4 _clear_col(0.5,  0.5, 0.5, 1.0);

mat4 projection, modelview, modelview2, modelview2_inv;
int viewport[4];

float _view_oldx = 0;
float _view_oldy = 0;
float _view_rotx = 0;
float _view_roty = 0;
bool _view_dofovy = false;
bool _view_doshift = false;
bool _view_motion = false;

float _world_oldx = 0;
float _world_roty = -35;


std::vector<Ray> rays;
std::vector<vec3> rayTracedImage;
GLuint rayTracedImageId = 0;

float _sample_factor = 0.9f;
int _sample_width = 0;
int _sample_height = 0;

int _rays_disp_step = 10;
bool _auto_vis = true;

int _recursions = 0;

enum vis_mode { vis_default, vis_opengl, vis_isecs, vis_lods, vis_N };
int _vis_mode = vis_default;

std::string _vis_names[vis_N] = { "raytraced image", "opengl shaded" };

static Scene scene;

const int nbrLightSources = 1;
GLfloat light_positions[][3] =
{
{ -10.0, -25.0, -1.0},
{ 0.0, 0.0, 100.0},
{ 10.0, 0.0, 0.0},
{ 0.0, 10.0, 0.0},
};

GLfloat light_ambients[][4] =
{
{ 1.0, 1.0, 1.0}, // default
{ 1.0, 0.0, 0.0},
{ 1.0, 1.0, 1.0}, // default
{ 1.0, 1.0, 1.0}, // default
};

GLfloat light_diffuses[][4] =
{
{ 1.0, 1.0, 1.0}, // default
{ 1.0, 1.0, 1.0},
{ 1.0, 1.0, 1.0}, // default
{ 1.0, 1.0, 1.0}, // default
};

//const int nbrSpheres = 2;
//GLfloat sphere_positions[nbrSpheres][4] =
//{
//{ 0.0, 0.0, 0.0, 0.0 },
//{ 5.0, 5.0, 5.0, 0.0 },
//};
//
//GLfloat sphere_ambients[nbrSpheres][4] =
//{
//{ 0.0, 0.0, 0.0, 1.0 }, // default
//{ 1.0, 1.0, 1.0, 1.0 },
//};
//
//GLfloat sphere_diffuses[nbrSpheres][4] =
//{
//{ 1.0, 1.0, 1.0, 1.0 }, // default
//{ 0.0, 0.0, 0.0, 1.0 },
//};
/*********************************************************************/
// raytracing
/*********************************************************************/

static void save(const string& filename = "image.ppm") {
	std::ofstream f;
	f.open(filename.c_str());
	f << "P3" << std::endl;
	f << _sample_width << " " << _sample_height << std::endl;
	f << "255" << std::endl;
	for(int i = 0; i < rayTracedImage.size(); i++) {
		toRGB(rayTracedImage[i]);
		f << rayTracedImage[i].x << " " << rayTracedImage[i].y << " " << rayTracedImage[i].z;
		if ((i + 1) % _sample_width != 0) {
			f << " ";
		} else {
			f << std::endl;
		}
	}
	f.close();
}

static void toRGB(float& x) {
	x = (int) (x * 255);
	if (x > 255) {
		std::cout << "x > 255" << std::endl;
		x = 0;
	}
}
static void toRGB(vec3& color) {
	toRGB(color.x);
	toRGB(color.y);
	toRGB(color.z);
}

static void prvec3(const vec3& v) {
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

static void init_lights() {
	// supposing, that GL_LIGHT0 == 0
	for (int i = 0; i < nbrLightSources; i++) {
		int light = GL_LIGHT0 + i;
		glLightfv(light, GL_POSITION, light_positions[i]);
		glLightfv(light, GL_AMBIENT, light_ambients[i]);
		glLightfv(light, GL_DIFFUSE, light_diffuses[i]);
		glEnable(light);
	}
}

static void get_color(vec3 &color, const Ray &r, const vec3 &vertex, const vec3 &normal, const vec3 &mat_amb, const vec3 &mat_diff) {
	// do net set color to (0,0,0) because of recursive calls
	//color = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < nbrLightSources; i++) {
		vec3 light_pos = vec3(*light_positions[i]);
		vec3 light_amb = vec3(*light_ambients[i]);
		vec3 light_diff = vec3(*light_diffuses[i]);
		// get light direction
		vec3 light_dir = normalize(vertex - light_pos);
		light_dir = normalize(light_pos - vertex);
		//prvec3(light_dir);
		// get half vector
		vec3 half = normalize(r.d + light_dir);
		// always add ambient light
//		prvec3(mat_amb);
//		prvec3(light_amb);
		color += mat_amb * light_amb;
//		prvec3(color);
		//prvec3(color);
		//bool intersected = false;
		Ray rayToLightSrc = Ray(vertex, light_dir);
		float t;
		vec3 dummy;
		bool intersected = scene.GetIntersectionPos(rayToLightSrc, t, &dummy, &dummy);
		intersected = (intersected && t > 0.1);
		// check light direction ...
		float norm_dot_light = dot(normal, light_dir);
		float norm_dot_h = dot(normal, half);
		// ... and add diffuse light only if reflection is possible and not behind another object
		if (norm_dot_light > 0.0 && !intersected) {
			// add diffuse light
			color += mat_diff * light_diff * norm_dot_light;
			if (norm_dot_h > 0.0) {
				// rs * L * (H.N)^n
				vec3 mat_spec = vec3(1, 1, 1);
				vec3 light_spec = vec3(1, 1, 1);
				float mat_shin = 2;
				color += mat_spec * light_spec * pow(norm_dot_h, mat_shin);
			}
		}
	}
	color /= nbrLightSources * 3.0f;
}


void clear_rays()
{
	rays.clear();
	rayTracedImage.clear();
}


static void unproject(const GLdouble* modelview_local, const GLdouble* projection_local, const GLint* viewport_local, vec3& myVec) {

	GLdouble posX;
	GLdouble posY;
	GLdouble posZ;

	gluUnProject(myVec.x, myVec.y, myVec.z, modelview_local, projection_local, viewport_local, &posX, &posY, &posZ);

	myVec.x = posX;
	myVec.y = posY;
	myVec.z = posZ;
}

static vec3 unHomogenise(const vec4& coordinateH) {

	return vec3(
			coordinateH.x / coordinateH.w,
			coordinateH.y / coordinateH.w,
			coordinateH.z / coordinateH.w
			);
}

static vec3 extractTransformationPart(const mat4& transMat) {

	return vec3(
				transMat[0][3] / transMat[3][3],
				transMat[1][3] / transMat[3][3],
				transMat[2][3] / transMat[3][3]);
}

static void printMat(const mat4& myMat) {

	for (int ri = 0; ri < myMat.row_size(); ++ri) {
		for (int ci = 0; ci < myMat.col_size(); ++ci) {
			std::cout << "\t" << myMat[ri][ci];
		}
		std::cout << std::endl;
	}
}

// Create rays for each sample of the image
void create_primary_rays(std::vector<Ray>& rays, int resx, int resy)
{
	mat4 modelview_inv = glm::inverse(modelview);
	mat4 modelview_trans = glm::transpose(modelview);
	mat4 modelview2_trans = glm::transpose(modelview2);

	for (int y = 0; y < resy; y++) {
		for (int x = 0; x < resx; x++) {
			// for these values, see cameraImagePlane
			float planeX = ((float)x / resx) * 2 - 1;
			float planeY = ((float)y / resy) * 2 - 1;
			float planeZ = -2;
			float planeW = 1;

			vec4 planePosH(planeX, planeY, planeZ, planeW);
			vec4 directionH = planePosH;

			// transform to where the camera plane is
			// coordinate transform:
			// c_new = M * c
			planePosH  = modelview_inv * planePosH;
			// normal transform:
			// d_new = M^{-T} * d
			directionH = modelview_trans * directionH;

			planePosH  = modelview2_inv * planePosH;
			directionH = modelview2_trans * directionH;

			vec3 direction = unHomogenise(directionH);
			vec3 planePos = unHomogenise(planePosH);

			// normalize
			direction /= direction.length();

			rays.push_back(Ray(planePos, direction));
		}
	}
}

void create_primary_rays_from_z_to_origin(std::vector<Ray>& rays, float z, int resx, int resy)
{
	vec3 origin = vec3(0, 0, z);
	for (int y = 0; y < resy; y++) {
		for (int x = 0; x < resx; x++) {
			vec3 direction = vec3(-resx / 2 + x, -resy / 2 + y, -2 * z);
			// normalize
			direction /= direction.length();
			rays.push_back(Ray(origin, direction));
		}
	}
}

// Ray trace the scene
void ray_trace()
{
	// Number of samples in x and y direction given the sampling factor
	int w = _sample_factor * _win_w;
	int h = _sample_factor * _win_h;

	_sample_width = w;
	_sample_height = h;

	std::cout << "raycast: w=" << w << " h=" << h << std::endl;

	clear_rays();

	create_primary_rays(rays, w, h);
//	create_primary_rays_from_z_to_origin(rays, 5.0, w, h);

	rayTracedImage.clear();
	rayTracedImage.resize(w*h, vec3(0, 1, 0));

	// TODO : write the samples with the correct color (i.e raytrace)
	glColor3f(1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vec3 matColor;
	vec3 newColor;
	vec3 mat_amb;
	vec3 mat_diff = vec3(0.5, 0.5, 0.5);
	vec3 surfaceNormal;
	const vec3 BACKGROUND_COLOR(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < rays.size(); i++) {
		const Ray& ray = rays.at(i);
		float t = -1.0f; // intersection multiplier
		bool intersected = scene.GetIntersectionPos(ray, t, &mat_amb, &surfaceNormal);
		if (intersected) {
			newColor = vec3(0.0f, 0.0f, 0.0f);
			get_color(newColor, ray, ray.att(t), surfaceNormal, mat_amb, mat_diff);
			rayTracedImage[i] = newColor;
		} else {
			rayTracedImage[i] = BACKGROUND_COLOR;
		}
	}

	// Create an openGL texture if it doesn't exist allready
	if(!rayTracedImageId)
	{
		glGenTextures(1, &rayTracedImageId);
		glBindTexture(GL_TEXTURE_2D, rayTracedImageId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	// Feed with raytraced image data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, &rayTracedImage[0]);

	// save to disk
	save();
}

// Draw the rays shot on the scene
void draw_rays()
{
	glPushMatrix();
	glMultMatrixf(&modelview2[0][0]);

	glBegin(GL_LINES);
	glColor3f(0,1,0);
	for (int i = 0; i < rays.size(); i++)
	{
		const vec3& from = rays.at(i).o;
		vec3 dir = rays.at(i).d;
		dir *= 100;
		const vec3 to = rays.at(i).o + dir;

		glVertex3fv(&from[0]);
		glVertex3fv(&to[0]);
	}
	glEnd();

	glPopMatrix();
}

// drawing utilities //////////////////////////////////
int get_vis_mode()
{
	bool can_show_image = (!_view_motion && !rayTracedImage.empty());
	return !can_show_image && _vis_mode == vis_default ? vis_opengl : _vis_mode;
}

void draw_camera()
{
	const vec3 c[4] =
	{
		vec3( 1,  1, -2),
		vec3( 1, -1, -2),
		vec3(-1, -1, -2),
		vec3(-1,  1, -2)
	};

	glBegin(GL_QUADS);
	glColor3f(0.2,0.2,0.2);
	for (int i = 0; i < 4; i++)
	{
		glVertex3fv(&c[i][0]);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1,1,0);
	for (int i = 0; i < 4; i++)
	{
		glVertex3f(0, 0, 0);
		glVertex3fv(&c[i][0]);
	}
	glEnd();
}

void full_screen_quad(){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_CULL_FACE);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3i(-1, -1, -1);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3i(1, -1, -1);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3i(1, 1, -1);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3i(-1, 1, -1);

	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void draw_raytraced_image()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, rayTracedImageId);
	glDisable(GL_LIGHTING);
	full_screen_quad();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void draw_string(int x, int y, const std::string& str)
{
	void* font = GLUT_BITMAP_HELVETICA_12;
	const int height = 15;
	const int tab = 18;

	vec3 colors[4] = { vec3(0, 0, 0), vec3(0.0, 1, 0.5), vec3(0.9, 0.3, 0.1), vec3(1, 1, 1) };

	glColor3fv(&colors[0][0]);

	glRasterPos2i(x, y);
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '\n')
		{
			y += height;
			glRasterPos2i(x, y);

			continue;
		}

		if (str[i] == '\t')
		{
			int p[4];
			glGetIntegerv(GL_CURRENT_RASTER_POSITION, p);

			int cx = ((p[0] + tab - 1) / tab) * tab;
			glRasterPos2i(cx, y);
		}

		if (str[i] == '$')
		{
			++i;

			int k = (int) str[i] - '0';
			if (k < 0 || k >= 4) k = 0;
			glColor3fv(&colors[k][0]);

			// update raster color, by setting current raster position
			int p[4];
			glGetIntegerv(GL_CURRENT_RASTER_POSITION, p);
			glRasterPos2i(p[0], y);

			continue;
		}

		glutBitmapCharacter(font, str[i]);
	}
}

// window drawing ///////////////////////////////////////

void main_reshape(int width,  int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	clear_rays();

	_win_w = (width - 3*_win_gap) / 2.0;
	_win_h = height - 2*_win_gap - 120;

	glutSetWindow(_id_screen);
	glutPositionWindow(_win_gap, _win_gap);
	glutReshapeWindow(_win_w, _win_h);

	glutSetWindow(_id_world);
	glutPositionWindow(_win_gap + _win_w + _win_gap, _win_gap);
	glutReshapeWindow(_win_w, _win_h);
}

void main_display()
{
	glClearColor(0.7, 0.7, 0.7, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::ostringstream text;
	text << "mouse \t: rotate scene, mouse+shift: change field of view\n";
	text << "r \t: raytrace scene (also triggered when user changes view or settings)\n";
	text << "tab   \t: change visualization (current: $1" << _vis_names[get_vis_mode()] << "$0)\n";

	std::ostringstream text2;
	text2 << "key s,S\t: +/- sampling factor $2" << _sample_factor << "$0\n";
	text2 << "key i\t: render image to file\n";

	draw_string(_win_gap, _win_gap + _win_h + _win_gap + 12, text.str());
	draw_string(2*_win_gap+_win_w, _win_gap + _win_h + _win_gap + 12, text2.str());

	glutSwapBuffers();
}

void draw_scene_openGL()
{
	glPushMatrix();
	glMultMatrixf(&modelview[0][0]);
	//glEnable(GL_LIGHTING);

	scene.Display();

	glPopMatrix();
}

void world_reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (float) width / height, 1.0, 256.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, -5.0, -30.0);
	glRotatef(_world_roty, 0.0, 1.0, 0.0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
}

void world_display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glMultMatrixf(&modelview2_inv[0][0]);

	glDisable(GL_LIGHTING);
	draw_camera();

	glMultMatrixf(&modelview[0][0]);
	draw_rays();

	draw_scene_openGL();

	glutSwapBuffers();
}

void screen_reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (float) width/height, zNear, zFar);
	glGetFloatv(GL_PROJECTION_MATRIX, &projection[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, &modelview2[0][0]);
	modelview2_inv = glm::inverse(modelview2);
	glRotatef(_view_rotx, 1, 0, 0);
	glRotatef(_view_roty, 0, 1, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, &modelview[0][0]);

	glClearColor(_clear_col[0], _clear_col[1], _clear_col[2], _clear_col[3]);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void screen_display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int vis = get_vis_mode();
	// Diplay the ray-traced image
	if (vis == vis_default)
	{
		draw_raytraced_image();
	}
	else
	{
		draw_scene_openGL();
	}

	glutSwapBuffers();
}

void redisplay_all()
{
	glutSetWindow(_id_window);
	glutPostRedisplay();

	glutSetWindow(_id_world);
	world_reshape(_win_w, _win_h);
	glutPostRedisplay();

	glutSetWindow(_id_screen);
	screen_reshape(_win_w, _win_h);
	glutPostRedisplay();
}

// gui interaction /////////////////////////////////////

void screen_mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT && state == GLUT_DOWN)
	{
		_view_motion = true;

		_view_oldx = x;
		_view_oldy = y;

		_view_dofovy = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
		_view_doshift = (glutGetModifiers() & GLUT_ACTIVE_ALT) != 0;
	}

	if (state == GLUT_UP)
	{
		_view_motion = false;
		ray_trace();
	}

	redisplay_all();
}

void screen_motion(int x, int y)
{
	if (_view_motion)
	{
		if (_view_dofovy)
		{
			fovy += 1.0 * (y - _view_oldy);
		}
		else if (_view_doshift)
		{
			center[0] += -0.3 * (x - _view_oldx);
			center[1] += 0.3 * (y - _view_oldy);
		}
		else
		{
			_view_roty += -1.0 * (x - _view_oldx);
			_view_rotx += 1.0 * (y - _view_oldy);
		}

		_view_oldx = x;
		_view_oldy = y;
	}

	redisplay_all();
}

void screen_passive_motion(int x, int y)
{
	if (x < 0 || y < 0 || x >= _win_w || y >= _win_h)
	{
		return;
	}
}

void screen_menu(int value)
{
	redisplay_all();
}

void world_mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT && state == GLUT_DOWN)
	{
		_world_oldx = x;
	}
}

void world_motion(int x, int y)
{
	_world_roty += -1.0 * (x - _world_oldx);
	_world_oldx = x;

	redisplay_all();
}

void world_menu(int value)
{
	redisplay_all();
}

void main_keyboard(unsigned char key, int x, int y)
{
	if((int)key == 9)
	{
		_view_motion = !_view_motion;
	}
	switch (key)
	{
		case 'q':
		case 27:
			exit(0);
			break;
		case 'r':
			ray_trace();
			break;
		case 'i':
			break;
		case 's':
			_sample_factor *= 2;
			std::cout << "sampling factor: " << _sample_factor << std::endl;
			break;
		case 'S':
			_sample_factor *= 0.5;
			std::cout << "sampling factor: " << _sample_factor << std::endl;
			break;
	}

	redisplay_all();
}

void idle()
{
}

int main(int argc, char** argv)
{
	// Init OpenGL stuffs
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1024, 600);
	glutInitWindowPosition(50, 50);
	glutInit(&argc, argv);

	// Create main window
	_id_window = glutCreateWindow("cg1 ex5 ws11/12 - raytracing");
	glutReshapeFunc(main_reshape);
	glutDisplayFunc(main_display);
	glutKeyboardFunc(main_keyboard);

	glutIdleFunc(idle);

	// Create world window
	_id_world = glutCreateSubWindow(_id_window, _win_gap, _win_gap, _win_w, _win_h);
	glutReshapeFunc(world_reshape);
	glutDisplayFunc(world_display);
	glutMouseFunc(world_mouse);
	glutMotionFunc(world_motion);
	glutKeyboardFunc(main_keyboard);
	glutCreateMenu(world_menu);
	glutAddMenuEntry("dummy menu entry (world)", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Create view window
	_id_screen = glutCreateSubWindow(_id_window, _win_gap + _win_w + _win_gap, _win_gap, _win_w, _win_h);
	glutReshapeFunc(screen_reshape);
	glutDisplayFunc(screen_display);
	glutMouseFunc(screen_mouse);
	glutMotionFunc(screen_motion);
	glutPassiveMotionFunc(screen_passive_motion);
	glutKeyboardFunc(main_keyboard);
	glutCreateMenu(screen_menu);
	glutAddMenuEntry("dummy menu entry (screen)", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Create scene
	scene.AddMesh("data/sphere.off", vec3(0.0f, 0.0f, 0.0f), 1.0f,   vec3(1.0f, 0.0f, 0.0f));
	scene.AddMesh("data/sphere.off", vec3(3.0f, 3.0f, 0.0f), 1.0f,   vec3(0.0f, 1.0f, 0.0f));
	scene.AddMesh("data/drei.off",   vec3(0.0f, 0.0f, -5.0f), 15.0f, vec3(0.3f, 0.3f, 0.3f));

	redisplay_all();

	ray_trace();

	glutMainLoop();

	return 0;
}
