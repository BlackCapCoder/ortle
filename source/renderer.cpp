#include "renderer.hpp"

#include "managed_window.hpp"
#include "window_manager.hpp"

#include "opengl/core330.hpp"
#include "opengl/buffer.hpp"
#include "opengl/program.hpp"
#include "opengl/shader.hpp"
#include "opengl/vertex_array.hpp"

#include "utility/trace.hpp"

#include <cassert>

#include <algorithm>
#include <utility>




namespace {


char const* l_vertex_shader_source = R"(

# version 330


// global uniforms

uniform mat4 u_projection;
uniform sampler2D u_texture;




// per-window uniforms

// this is the difference between (0, 0) in window coordinates and (0, 0) in 
// texture coordinates.  this offset is used when calculating texture 
// coordinates.

uniform float u_border_width;


// this is the area of the screen that the window (excluding its border) 
// occupies:  vec4(x, y, width, heght); note that the texture of the window 
// may be a little bigger to include the border.  these values are in screen
// coordinates.

uniform vec4 u_window_geometry;




// rectangle uniform

// this is the subrectangle of the window of the window that we are trying to 
// render.  vec4(x, y, width, height); note that this may actually be bigger 
// than u_window_geometry (or outside its apparent area) to include the 
// window's border.  these values are in window coordinates!

uniform vec4 u_rectangle_geometry;




// vertex attributes

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec2 in_texture_coordinates;




// output to the fragment shader

smooth out vec2 s_texture_coordinates;




void main()
{
	vec4 position = in_position;
	vec2 texture_size = vec2(2.0f * u_border_width + u_window_geometry.z, 2.0f * u_border_width + u_window_geometry.w);


	// calculate the position of this vertex in screen coordinates:

	position.x = in_position.x * u_rectangle_geometry.z + u_window_geometry.x + (u_rectangle_geometry.x + u_border_width);
	position.y = in_position.y * u_rectangle_geometry.w + u_window_geometry.y + (u_rectangle_geometry.y + u_border_width);


	// calculate the texture coordinates of this vertex:

	s_texture_coordinates.x = (in_texture_coordinates.x * u_rectangle_geometry.z + (u_rectangle_geometry.x + u_border_width)) / texture_size.x;
	s_texture_coordinates.y = (in_texture_coordinates.y * u_rectangle_geometry.w + (u_rectangle_geometry.y + u_border_width)) / texture_size.y;


	// commit the values

	gl_Position = u_projection * position;

}

)";


char const* l_fragment_shader_source = R"(

#version 330

uniform sampler2D u_texture;

smooth in vec2 s_texture_coordinates;

out vec4 out_color;

void main()
{
	out_color = texture(u_texture, s_texture_coordinates);
}

)";


GLfloat const l_projection_matrix[] = {

	 1.0f,  0.0f,  0.0f,  0.0f,
	 0.0f, -1.0f,  0.0f,  0.0f,
	 0.0f,  0.0f,  0.0f,  0.0f,
	 -1.0f, 1.0f,  0.0f,  1.0f

};


GLfloat const l_vertex_data[] = {

	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f,

	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f

};


GLushort const l_index_data[] = {

	3, 2, 1,
	1, 3, 0

};


} // namespace




Renderer::Renderer()
	: m_program(0)
	, m_vertex_buffer()
	, m_index_buffer()
	, m_vertex_array()
	, m_u_projection_matrix(0)
	, m_u_texture(0)
	, m_u_border_width(0)
	, m_u_window_geometry(0)
	, m_u_rectangle_geometry(0)
	, m_projection_matrix{ 0.0f }
{

	TRACE("creating new renderer");


	OpenGL::Shader vertex_shader(gl::VERTEX_SHADER, l_vertex_shader_source);
	OpenGL::Shader fragment_shader(gl::FRAGMENT_SHADER, l_fragment_shader_source);

	m_program = OpenGL::Program{ &vertex_shader, &fragment_shader };


	gl::BindBuffer(gl::ARRAY_BUFFER, m_vertex_buffer);
	gl::BufferData(gl::ARRAY_BUFFER, 24 * sizeof(GLfloat), l_vertex_data, gl::STATIC_DRAW);
	gl::BindBuffer(gl::ARRAY_BUFFER, 0);

	gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, m_index_buffer);
	gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), l_index_data, gl::STATIC_DRAW);
	gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, 0);


	gl::BindVertexArray(m_vertex_array);

	gl::BindBuffer(gl::ARRAY_BUFFER, m_vertex_buffer);
	gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, m_index_buffer);

	gl::EnableVertexAttribArray(0);
	gl::VertexAttribPointer(0, 4, gl::FLOAT, gl::FALSE_, 6 * sizeof(GLfloat), 0);

	gl::EnableVertexAttribArray(1);
	gl::VertexAttribPointer(1, 2, gl::FLOAT, gl::FALSE_, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(4 * sizeof(GLfloat)));

	gl::BindBuffer(gl::ARRAY_BUFFER, 0);

	gl::BindVertexArray(0);
	

	m_u_projection_matrix = gl::GetUniformLocation(m_program, "u_projection");
	m_u_texture = gl::GetUniformLocation(m_program, "u_texture");
	m_u_border_width = gl::GetUniformLocation(m_program, "u_border_width");
	m_u_window_geometry = gl::GetUniformLocation(m_program, "u_window_geometry");
	m_u_rectangle_geometry = gl::GetUniformLocation(m_program, "u_rectangle_geometry");


	std::copy(l_projection_matrix, l_projection_matrix + 16, m_projection_matrix);


	gl::ActiveTexture(gl::TEXTURE0);

	gl::Enable(gl::BLEND);
	gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
}




Renderer::Renderer(Renderer&& other)
	: m_program(0)
	, m_vertex_buffer(0)
	, m_index_buffer(0)
	, m_vertex_array(0)
	, m_u_projection_matrix(0)
	, m_u_texture(0)
	, m_u_border_width(0)
	, m_u_window_geometry(0)
	, m_u_rectangle_geometry(0)
	, m_projection_matrix{ 0.0f }
{
	swap(*this, other);
}


Renderer& Renderer::operator=(Renderer&& other)
{
	swap(*this, other);
	return *this;
}




Renderer::~Renderer()
{
	if (m_program != 0) {
		TRACE("destroying renderer");
	}
}




void swap(Renderer& first, Renderer& second)
{
	using std::swap;

	swap(first.m_program, second.m_program);
	swap(first.m_vertex_buffer, second.m_vertex_buffer);
	swap(first.m_index_buffer, second.m_index_buffer);
	swap(first.m_vertex_array, second.m_vertex_array);
	swap(first.m_u_projection_matrix, second.m_u_projection_matrix);
	swap(first.m_u_texture, second.m_u_texture);
	swap(first.m_u_border_width, second.m_u_border_width);
	swap(first.m_u_window_geometry, second.m_u_window_geometry);
	swap(first.m_u_rectangle_geometry, second.m_u_rectangle_geometry);
	swap(first.m_projection_matrix, second.m_projection_matrix);
}



void Renderer::set_viewport(unsigned int width, unsigned int height)
{
	assert(m_program != 0);

	if (width < 2) {
		width = 2;
	}

	if (height < 2) {
		height = 2;
	}

	m_projection_matrix[0] = 2.0f / static_cast<float>(width);
	m_projection_matrix[5] = -2.0f / static_cast<float>(height);

	gl::Viewport(0, 0, width, height);
}


void Renderer::render(WindowManager::Iterator begin, WindowManager::Iterator end)
{
	assert(m_program != 0);


	gl::ActiveTexture(gl::TEXTURE0);

	gl::UseProgram(m_program);

	gl::UniformMatrix4fv(m_u_projection_matrix, 1, gl::FALSE_, m_projection_matrix);
	gl::Uniform1i(m_u_texture, 0);

	gl::BindVertexArray(m_vertex_array);


	for (auto it = begin; it != end; ++it) {
		(*it)->render(*this);
	}


	gl::BindVertexArray(0);

	gl::UseProgram(0);
}




void Renderer::draw_quad()
{
	gl::DrawElements(gl::TRIANGLES, 6, gl::UNSIGNED_SHORT, 0);	
}




void Renderer::set_border_width(float border_width)
{
	// TRACE("setting border width", border_width);

	gl::Uniform1f(m_u_border_width, border_width);
}


void Renderer::set_window_geometry(float x, float y, float width, float height)
{
	// TRACE("setting window geometry", x, y, width, height);

	gl::Uniform4f(m_u_window_geometry, x, y, width, height);
}


void Renderer::set_rectangle_geometry(float x, float y, float width, float height)
{
	// TRACE("setting rectangle geometry", x, y, width, height);

	gl::Uniform4f(m_u_rectangle_geometry, x, y, width, height);
}

