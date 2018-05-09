#include "meshquad.h"
#include "matrices.h"


void MeshQuad::clear()
{
    m_points.clear();
    m_quad_indices.clear();
}

int MeshQuad::add_vertex(const Vec3& P)
{
    m_points.push_back(P);
    return m_points.size()-1;
}


void MeshQuad::add_quad(int i1, int i2, int i3, int i4)
{
    m_quad_indices.push_back(i1);
    m_quad_indices.push_back(i2);
    m_quad_indices.push_back(i3);
    m_quad_indices.push_back(i4);
}

void MeshQuad::convert_quads_to_tris(const std::vector<int>& quads, std::vector<int>& tris)
{
	tris.clear();
	tris.reserve(3*quads.size()/2);
	// Pour chaque quad on genere 2 triangles
	// Attention a repecter l'orientation des triangles

    for(unsigned int i=0; i<quads.size(); i=i+4) {
        tris.push_back(quads[i]);
        tris.push_back(quads[i+1]);
        tris.push_back(quads[i+3]);
        tris.push_back(quads[i+1]);
        tris.push_back(quads[i+2]);
        tris.push_back(quads[i+3]);
    }

}

void MeshQuad::convert_quads_to_edges(const std::vector<int>& quads, std::vector<int>& edges)
{
	edges.clear();
    edges.reserve(quads.size()*2); // ( *2 /2 !)
	// Pour chaque quad on genere 4 aretes, 1 arete = 2 indices.
	// Mais chaque arete est commune a 2 quads voisins !
	// Comment n'avoir qu'une seule fois chaque arete ?

    for(unsigned int i =0 ; i<quads.size() ; i = i+4) {
        edges.push_back(quads[i]);
        edges.push_back(quads[i+1]);

        edges.push_back(quads[i+1]);
        edges.push_back(quads[i+2]);

        edges.push_back(quads[i+2]);
        edges.push_back(quads[i+3]);

        edges.push_back(quads[i+3]);
        edges.push_back(quads[i]);

    }

}


void MeshQuad::bounding_sphere(Vec3& C, float& R)
{
       // C = Moyenne de tous les vertex
       // R = Maximum du parcours de tout les vertex -> calcule de la distance entre centre et chaque vertex

        float x = 0.0;
        float y = 0.0;
        float z = 0.0;

        //on fait la moyenne
        for(unsigned int i = 0; i <m_points.size(); i++)
        {
            x += m_points[i].x;
            y += m_points[i].y;
            z += m_points[i].z;
        }

        x = x/m_points.size();
        y = y/m_points.size();
        z = z/m_points.size();

        C.x = x;
        C.y = y;
        C.z = z;


        float max = 0.0;
        int indice = 0;

        for(unsigned int i = 0; i <m_points.size(); i++)
        {
            float comparaison = m_points[i].x*m_points[i].x + m_points[i].y*m_points[i].y + m_points[i].z*m_points[i].z;
            if( comparaison > max)
            {
                max = comparaison;
                indice = i;
            }
        }


        R = sqrt(max);
}


void MeshQuad::create_cube()
{
	clear();
	// ajouter 8 sommets (-1 +1)
    int pt0 = add_vertex(Vec3(1,1,0));
    int pt1 = add_vertex(Vec3(0,1,0));
    int pt2 = add_vertex(Vec3(0,0,0));
    int pt3 = add_vertex(Vec3(1,0,0));
    int pt4 = add_vertex(Vec3(1,1,-1));
    int pt5 = add_vertex(Vec3(0,1,-1));
    int pt6 = add_vertex(Vec3(0,0,-1));
    int pt7 = add_vertex(Vec3(1,0,-1));

	// ajouter 6 faces (sens trigo)
    add_quad(pt0,pt1,pt2,pt3);
    add_quad(pt4,pt5,pt1,pt0);
    add_quad(pt7,pt6,pt5,pt4);
    add_quad(pt3,pt2,pt6,pt7);
    add_quad(pt4,pt0,pt3,pt7);
    add_quad(pt1,pt5,pt6,pt2);

	gl_update();
}

Vec3 MeshQuad::normal_of(const Vec3& A, const Vec3& B, const Vec3& C)
{
	// Attention a l'ordre des points !
	// le produit vectoriel n'est pas commutatif U ^ V = - V ^ U
	// ne pas oublier de normaliser le resultat.

    Vec3 AB = Vec3(B.x-A.x, B.y-A.y , B.z-A.z);
    Vec3 BC = Vec3(C.x-B.x , C.y-B.y , C.z-B.z);

    Vec3 ABBC = glm::cross(AB,BC);
    Vec3 res = glm::normalize(ABBC);

    return res;
}


bool MeshQuad::is_points_in_quad(const Vec3& P, const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D)
{
	// On sait que P est dans le plan du quad.

	// P est-il au dessus des 4 plans contenant chacun la normale au quad et une arete AB/BC/CD/DA ?
	// si oui il est dans le quad

    float res = -1;
    float d = 0;

    Vec3 n = normal_of(A,B,C);
    Vec3 AB = Vec3(B.x-A.x, B.y-A.y , B.z-A.z);
    Vec3 BC = Vec3(C.x-B.x , C.y-B.y , C.z-B.z);
    Vec3 CD = Vec3(D.x-C.x , D.y-C.y , D.z - C.z);
    Vec3 DA = Vec3(A.x-D.x, A.y-D.y, A.z-D.z);

    //normale et AB
    Vec3 n1 = vec_cross(n,AB);
    d = vec_dot(n1,A);
    res = vec_dot(n1,P) - d;
    if(res < 0) {
        return false;
    }

    //normale et BC
    Vec3 n2 = vec_cross(n,BC);
    d= vec_dot(n2,B);
    res = vec_dot(n2,P) -d;
    if(res < 0) {
        return false;
    }

    //normale et CD
    Vec3 n3 = vec_cross(n,CD);
    d= vec_dot(n3,C);
    res = vec_dot(n3,P) -d;
    if(res < 0) {
        return false;
    }

    //normale et DA
    Vec3 n4 = vec_cross(n,DA);
    d= vec_dot(n4,D);
    res = vec_dot(n4,P) -d;
    if(res < 0) {
        return false;
    }

    return true;
}

bool MeshQuad::intersect_ray_quad(const Vec3& P, const Vec3& Dir, int q, Vec3& inter)
{
	// recuperation des indices de points
    int i1 = m_quad_indices[q];
    int i2 = m_quad_indices[q+1];
    int i3 = m_quad_indices[q+2];
    int i4 = m_quad_indices[q+3];

    // recuperation des points
    Vec3 pt1 = m_points[i1];
    Vec3 pt2 = m_points[i2];
    Vec3 pt3 = m_points[i3];
    Vec3 pt4 = m_points[i4];

	// calcul de l'equation du plan (N+d)
    Vec3 n = normal_of(pt1,pt2,pt3);
    int d = vec_dot(pt1,n);

    // calcul de l'intersection rayon plan
    float t = (d-vec_dot(P,n))/(vec_dot(Dir,n));

        if( t == INFINITY) {
            return false;
        }

        // alpha => calcul de I
        Vec3 I = P+t*Dir;

        // I dans le quad ?
        if(is_points_in_quad(I,pt1,pt2,pt3,pt4)) {
            inter = I;
            return true;
        }

    return false;
}


int MeshQuad::intersected_closest(const Vec3& P, const Vec3& Dir)
{
	// on parcours tous les quads
	// on teste si il y a intersection avec le rayon
	// on garde le plus proche (de P)

	int inter = -1;
    Vec3 intercourante;
    float min = 0;

    for(int i =0 ; i<m_quad_indices.size(); i=i+4) {
        if(intersect_ray_quad(P, Dir, i,intercourante)) {
            if(inter ==-1) {
                min = (intercourante.x-P.x)*(intercourante.x-P.x)+(intercourante.y-P.y)*(intercourante.y-P.y)+(intercourante.z-P.z)*(intercourante.z-P.z);
                inter = i;
            }
            else {
                float compare = (intercourante.x-P.x)*(intercourante.x-P.x)+(intercourante.y-P.y)*(intercourante.y-P.y)+(intercourante.z-P.z)*(intercourante.z-P.z);
                if(compare<min) {
                    min = compare;
                    inter = i;
                }
            }
        }
    }

	return inter;
}


Mat4 MeshQuad::local_frame(int q)
{
    /*
        Repere locale = Matrice de transfo avec
        les trois premieres colones: X,Y,Z locaux
        la derniere colonne l'origine du repere
        -> Mat4 = Vec4 X + Vec4 Y + Vec4 Z + Vec4 O
        ici Z = N et X = AB
        Origine le centre de la face
        longueur des axes : [AB]/2
        */

    // recuperation des indices de points
    int i1 = m_quad_indices[q];
    int i2 = m_quad_indices[q+1];
    int i3 = m_quad_indices[q+2];
    int i4 = m_quad_indices[q+3];

    // recuperation des points
    Vec3 pt1 = m_points[i1];
    Vec3 pt2 = m_points[i2];
    Vec3 pt3 = m_points[i3];
    Vec3 pt4 = m_points[i4];

	// calcul de Z:N / X:AB -> Y
    Vec3 Z = glm::normalize(normal_of(pt1, pt2, pt3));
    Vec3 X = glm::normalize(pt2 - pt1);
    Vec3 Y = glm::normalize(glm::cross(X, Z));


	// calcul du centre
    float Ox = (pt1.x+pt2.x+pt3.x+pt4.x) / 4 ;
    float Oy = (pt1.y+pt2.y+pt3.y+pt4.y) / 4;
    float Oz = (pt1.z+pt2.z+pt3.z+pt4.z) / 4 ;
    Vec3 O = Vec3(Ox,Oy,Oz);

    // calcul de la taille
    float taille = lenght(pt2-pt1) /2 ;

	// calcul de la matrice
    Vec4 X4 = Vec4(X.x,X.y,X.z,0);
    Vec4 Y4 = Vec4(Y.x,Y.y,Y.z,0);
    Vec4 Z4 = Vec4(Z.x,Z.y,Z.z,0);
    Vec4 O4 = Vec4(O.x,O.y,O.z,1);

    Mat4 res = Mat4(X4,Y4,Z4,O4);
    res = res*scale(taille);


    return res;
}

void MeshQuad::extrude_quad(int q)
{
    // recuperation des indices de points
    int i1 = m_quad_indices[q];
    int i2 = m_quad_indices[q+1];
    int i3 = m_quad_indices[q+2];
    int i4 = m_quad_indices[q+3];

    // recuperation des points
    Vec3 pt1 = m_points[i1];
    Vec3 pt2 = m_points[i2];
    Vec3 pt3 = m_points[i3];
    Vec3 pt4 = m_points[i4];

	// calcul de la normale
    Vec3 n = normal_of(pt1,pt2,pt3);

	// calcul de la hauteur

	// calcul et ajout des 4 nouveaux points

	// on remplace le quad initial par le quad du dessu

	// on ajoute les 4 quads des cotes

   gl_update();
}

void MeshQuad::transfo_quad(int q, const glm::mat4& tr)
{
	// recuperation des indices de points
	// recuperation des (references de) points

	// generation de la matrice de transfo globale:
	// indice utilisation de glm::inverse() et de local_frame

	// Application au 4 points du quad
}

void MeshQuad::decale_quad(int q, float d)
{

}

void MeshQuad::shrink_quad(int q, float s)
{

}

void MeshQuad::tourne_quad(int q, float a)
{

}





MeshQuad::MeshQuad():
	m_nb_ind_edges(0)
{}


void MeshQuad::gl_init()
{
	m_shader_flat = new ShaderProgramFlat();
	m_shader_color = new ShaderProgramColor();

	//VBO
	glGenBuffers(1, &m_vbo);

	//VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_flat->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_flat->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &m_vao2);
	glBindVertexArray(m_vao2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_color->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_color->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	//EBO indices
	glGenBuffers(1, &m_ebo);
	glGenBuffers(1, &m_ebo2);
}

void MeshQuad::gl_update()
{
	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * m_points.size() * sizeof(GLfloat), &(m_points[0][0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::vector<int> tri_indices;
	convert_quads_to_tris(m_quad_indices,tri_indices);

	//EBO indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,tri_indices.size() * sizeof(int), &(tri_indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<int> edge_indices;
	convert_quads_to_edges(m_quad_indices,edge_indices);
	m_nb_ind_edges = edge_indices.size();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,m_nb_ind_edges * sizeof(int), &(edge_indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



void MeshQuad::set_matrices(const Mat4& view, const Mat4& projection)
{
	viewMatrix = view;
	projectionMatrix = projection;
}

void MeshQuad::draw(const Vec3& color)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	m_shader_flat->startUseProgram();
	m_shader_flat->sendViewMatrix(viewMatrix);
	m_shader_flat->sendProjectionMatrix(projectionMatrix);
	glUniform3fv(m_shader_flat->idOfColorUniform, 1, glm::value_ptr(color));
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo);
	glDrawElements(GL_TRIANGLES, 3*m_quad_indices.size()/2,GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	m_shader_flat->stopUseProgram();

	glDisable(GL_POLYGON_OFFSET_FILL);

	m_shader_color->startUseProgram();
	m_shader_color->sendViewMatrix(viewMatrix);
	m_shader_color->sendProjectionMatrix(projectionMatrix);
	glUniform3f(m_shader_color->idOfColorUniform, 0.0f,0.0f,0.0f);
	glBindVertexArray(m_vao2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo2);
	glDrawElements(GL_LINES, m_nb_ind_edges,GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	m_shader_color->stopUseProgram();
	glDisable(GL_CULL_FACE);
}

