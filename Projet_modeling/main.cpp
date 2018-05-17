
#include <QApplication>
#include <QGLViewer/simple_viewer.h>
#include <matrices.h>
#include <primitives.h>
#include <meshquad.h>

const Vec3 ROUGE   = {1,0,0};
const Vec3 VERT    = {0,1,0};
const Vec3 BLEU    = {0,0,1};
const Vec3 JAUNE   = {1,1,0};
const Vec3 CYAN    = {0,1,1};
const Vec3 MAGENTA = {1,0,1};
const Vec3 BLANC   = {1,1,1};
const Vec3 GRIS    = {0.5,0.5,0.5};
const Vec3 NOIR    = {0,0,0};

void fleche(const Primitives& prim, const Mat4& trf, Vec3 coul) {
    prim.draw_cone(trf * translate(0,0,3), coul);
    prim.draw_cylinder(trf * translate(0,0,1.5)*scale(0.5f,0.5f,2.0f), coul);

}

void draw_repere(const Primitives& prim, const Mat4& tr)
{
    //sphere origine
    prim.draw_sphere(tr,BLANC);

    //fleche sur z
    fleche(prim,tr, BLEU);

    //fleche sur y
    fleche(prim,tr * rotateX(-90), VERT);

    //fleche sur x
    fleche(prim,tr *rotateY(90), ROUGE);
}


void star(MeshQuad& m)
{
	m.create_cube();
    int facecourante = 0;
    while(facecourante <=20) {
        for(int i=1 ; i<20;i++) {
            m.extrude_quad(facecourante);
            m.shrink_quad(facecourante, 0.8);
            m.tourne_quad(facecourante,7);
        }
    facecourante = facecourante + 4;
    }


}

void glace(MeshQuad& m) {
    m.create_cube();
    int ftoextrude = 0;
    int ftoturn1 = 4;
    int ftoturn2 = 12;
    int ftoturn3 = 16;
    int ftoturn4 = 20;

    float angle1 = 10;
    float angle2 = 20;
    float decalage = 5;

    m.decale_quad(ftoextrude,decalage);

    for(int i=0; i<100;i++) {

        if(i%10 == 0)
        {
            decalage = 5 - (i/20.0f);
        }

        m.tourne_quad(ftoturn1, angle1);
        m.tourne_quad(ftoturn2, -1*angle1);
        m.tourne_quad(ftoturn3, angle2);
        m.tourne_quad(ftoturn4, -1*angle2);
        m.extrude_quad(ftoextrude);

        m.decale_quad(ftoextrude,decalage);

        if(ftoturn1 == 4) {
            ftoturn1 = 36;
            ftoturn2 = 32;
            ftoturn3 = 28;
            ftoturn4 = 24;
        }
        else {
            ftoturn1 = ftoturn1 +16;
            ftoturn2 = ftoturn2 +16;
            ftoturn3 = ftoturn3 +16;
            ftoturn4 = ftoturn4 +16;
        }
    }


}

void recursif(MeshQuad& m, int q) {
    if(q=0) {

    }
}

void appelrecursif(MeshQuad& m) {
    m.create_cube();
    recursif(m,0);
}


int main(int argc, char *argv[])
{
	Primitives prim;
	int selected_quad = -1;
	glm::mat4 selected_frame;
	MeshQuad mesh;

	// init du viewer
	QApplication a(argc, argv);
	SimpleViewer::init_gl();
	SimpleViewer viewer({0.1,0.1,0.1},5);

	// GL init
	viewer.f_init = [&] ()
	{
		prim.gl_init();
		mesh.gl_init();
	};

	// drawing
	viewer.f_draw = [&] ()
	{
		mesh.set_matrices(viewer.getCurrentModelViewMatrix(),viewer.getCurrentProjectionMatrix());
		prim.set_matrices(viewer.getCurrentModelViewMatrix(),viewer.getCurrentProjectionMatrix());

		mesh.draw(CYAN);

		if (selected_quad>=0)
			draw_repere(prim,selected_frame);
	};

	// to do when key pressed
	viewer.f_keyPress = [&] (int key, Qt::KeyboardModifiers mod)
	{
		switch(key)
		{
			case Qt::Key_C:
				if (!(mod & Qt::ControlModifier))
					mesh.create_cube();
				break;
			// e extrusion
            case Qt::Key_E:
            if(selected_quad != -1) {
                mesh.extrude_quad(selected_quad);
            }
            break;
			// +/- decale
            case Qt::Key_Plus:
            if(selected_quad != -1) {
                mesh.decale_quad(selected_quad, 1);
            }
            break;
            case Qt::Key_Minus:
            if(selected_quad != -1) {
                mesh.decale_quad(selected_quad, -1);
            }
            break;
			// z/Z shrink
            case Qt::Key_Z:
            if(selected_quad != -1) {
                if(mod && Qt::Key_Shift) {
                    mesh.shrink_quad(selected_quad, 1.1);
                } else {
                    mesh.shrink_quad(selected_quad, 0.9);
                }
            }
            break;
			// t/T tourne
            case Qt::Key_T:
            if(selected_quad != -1) {
                if(mod && Qt::Key_Shift) {
                    mesh.tourne_quad(selected_quad, 1);
                } else {
                    mesh.tourne_quad(selected_quad, -1);
                }
            }
            break;

			// Attention au cas m_selected_quad == -1

			// generation d'objet
            //etoile
			case Qt::Key_S:
				star(mesh);
				break;
            // glace
        case Qt::Key_G:
            glace(mesh);
            break;


			default:
				break;
		}

		Vec3 sc;
		float r;
		mesh.bounding_sphere(sc,r);
		viewer.setSceneCenter(qglviewer::Vec(sc[0],sc[1],sc[2]));
		viewer.setSceneRadius(r);
		viewer.camera()->centerScene();
		viewer.update();
	};

	// to do when mouse clicked (P + Dir = demi-droite (en espace objet) orthogonale à l'écran passant par le point cliqué
	viewer.f_mousePress3D = [&] (Qt::MouseButton /*b*/, const glm::vec3& P, const glm::vec3& Dir)
	{
		selected_quad = mesh.intersected_closest(P,Dir);
		if (selected_quad>=0)
			selected_frame = mesh.local_frame(selected_quad);
		std::cout << selected_quad << std::endl;
	};

	viewer.clearShortcuts();
	viewer.setShortcut(QGLViewer::EXIT_VIEWER,Qt::Key_Escape);
	viewer.show();
	return a.exec();
}
