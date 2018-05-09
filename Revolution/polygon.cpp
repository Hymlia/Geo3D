#include "polygon.h"
#include <cstdint>



bool intersecte(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D)
{
}


void PolygonEditor::add_vertex(float x, float y)
{
	//faire une version améliorée qui interdit les intersections.
    m_points.push_back(Vec3(x,y,0));
}


void PolygonEditor::remove_last()
{
    m_points.pop_back();
}


void PolygonEditor::clear()
{
    m_points.clear();
}


void PolygonEditor::lisse()
{
	// vecteur du polygone lissé
	std::vector<Vec3> subd;

    for(int i=0; i<m_points.size()-1; i++) {
        Vec3 p1 = m_points[i];
        Vec3 p2 = m_points[i+1];

        //Vec3 pp1 =Vec3( (p1.x+ p2.x)/4 , (p1.y + p2.y)/4, (p1.z+p2.z)/4);
        //Vec3 pp2=Vec3( 3*(p1.x+ p2.x)/4 , 3*(p1.y + p2.y)/4, 3*(p1.z+p2.z)/4);

        Vec3 pp1 = 0.75f * p1 + 0.25f* p2;
        Vec3 pp2 = 0.75f * p2 + 0.25f * p1;
        subd.push_back(pp1);
        subd.push_back(pp2);
    }

	// échange des 2 vecteurs de points
	m_points.swap(subd);
}





PolygonEditor::PolygonEditor()
{}

void PolygonEditor::gl_init()
{
	// SHADER
	m_shader_color = new ShaderProgramColor();

	//	//VBO
	glGenBuffers(1, &m_vbo);

	// genere 1 VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_color->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_color->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
}

void PolygonEditor::draw(const Vec3& color)
{
	if (m_points.empty())
		return;

	Mat4 id;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_points.size()*sizeof(Vec3), &m_points.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_shader_color->startUseProgram();
	m_shader_color->sendViewMatrix(id);
	m_shader_color->sendProjectionMatrix(id);

	glUniform3fv(m_shader_color->idOfColorUniform, 1, glm::value_ptr(color));

	glBindVertexArray(m_vao);
	glPointSize(4.0);
	glDrawArrays(GL_POINTS, 0, m_points.size());
	glDrawArrays(GL_LINE_STRIP, 0, m_points.size());
	glBindVertexArray(0);
	m_shader_color->stopUseProgram();
}


