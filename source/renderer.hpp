#ifndef ORTLE_RENDERER_HPP
#define ORTLE_RENDERER_HPP


#include "window_manager.hpp"

#include "opengl/core330.hpp"
#include "opengl/buffer.hpp"
#include "opengl/program.hpp"
#include "opengl/vertex_array.hpp"




class Renderer {

public:

	Renderer();

	Renderer(Renderer&& other);
	Renderer& operator=(Renderer&& other);

	~Renderer();

	friend void swap(Renderer& first, Renderer& second);


public:

	void set_viewport(unsigned int width, unsigned int height);

	
public:

	void render(WindowManager::Iterator begin, WindowManager::Iterator end);


public:

	void draw_quad();
	void draw_shadow(float size, float x, float y, float w, float h);
	void setNormal();
	void setShadow();

	void set_border_width(float border_width);
	void set_window_geometry(float x, float y, float width, float height);
	void set_rectangle_geometry(float x, float y, float width, float height);

	void set_shadow_side(bool t, bool r, bool b, bool l);


private:

	OpenGL::Program m_program;
	OpenGL::Program m_program_shadow;

	OpenGL::Buffer m_vertex_buffer;
	OpenGL::Buffer m_index_buffer;

	OpenGL::VertexArray m_vertex_array;

	GLint m_u_projection_matrix;
	GLint m_u_texture;
	GLint m_u_border_width;
	GLint m_u_window_geometry;
	GLint m_u_rectangle_geometry;
	GLint m_u_shadow;

	GLfloat m_projection_matrix[16];

};


#endif

