#include <QApplication>
#include <QGLViewer/simple_viewer.h>
#include <matrices.h>
#include <primitives.h>


const Vec3 ROUGE   = {1,0,0};
const Vec3 VERT    = {0,1,0};
const Vec3 BLEU    = {0,0,1};
const Vec3 JAUNE   = {1,1,0};
const Vec3 CYAN    = {0,1,1};
const Vec3 MAGENTA = {1,0,1};
const Vec3 BLANC   = {1,1,1};
const Vec3 GRIS    = {0.5,0.5,0.5};
const Vec3 NOIR    = {0,0,0};

/// primitives(sphere/cube/cylindre/cone)
Primitives prim;

/// compteur (incremente dans animate)
int compteur = 0;

void fleche(const Mat4& trf, Vec3 coul) {
    prim.draw_cone(trf * translate(0,0,3), coul);
    prim.draw_cylinder(trf * translate(0,0,1.5)*scale(0.5f,0.5f,2.0f), coul);

}

void doigt(const Mat4& tr, int angle , int taille) {
    prim.draw_sphere(tr * rotateZ(angle), BLANC);
    prim.draw_cube(tr*rotateZ(angle) *translate(2,0,0) *scale(taille*3,1,1), ROUGE);
    prim.draw_sphere(tr*rotateZ(angle)* translate(4,0,0), BLANC);
    prim.draw_cube(tr*rotateZ(angle) *translate(6,0,0) *scale(taille*3,1,1), VERT);
    prim.draw_sphere(tr* rotateZ(angle) * translate(8,0,0), BLANC);
    prim.draw_cube(tr*rotateZ(angle) *translate(9.5,0,0) *scale(taille*2,1,1), BLEU);
}

void paume(const Mat4& tr) {
    doigt(tr,-10,1);
    doigt(tr* translate(0,2,0),0,1);
    doigt(tr* translate(0,4,0),10,1);
    doigt(tr* translate(-2,-1,0),-80,1);
    prim.draw_cube(tr * translate(-3,2,0) * scale(5,5,1), VERT);
}


void draw_repere(const Mat4& tr)
{
    //sphere origine
    prim.draw_sphere(tr,BLANC);

    //fleche sur z
   // prim.draw_cylinder(translate(0,0,1.5)*scale(0.5f,0.5f,2.0f), BLEU);
    //prim.draw_cone(translate(0,0,3), BLEU);
    fleche(tr, BLEU);

    //fleche sur y
    //prim.draw_cylinder(rotateX(-90)*translate(0,0,1.5)*scale(0.5f,0.5f,2.0f), VERT);
    //prim.draw_cone(rotateX(-90)*translate(0,0,3), VERT);
    fleche(tr * rotateX(-90), VERT);

    //fleche sur x
    //prim.draw_cylinder(rotateY(90)*translate(0,0,1.5)*scale(0.5f,0.5f,2.0f), ROUGE);
    //prim.draw_cone(rotateY(90)*translate(0,0,3), ROUGE);
    fleche(tr *rotateY(90), ROUGE);

}



void articulations(const Mat4& transfo){
    paume(transfo);
}



int main(int argc, char *argv[])
{
	// init du viewer
	QApplication a(argc, argv);
	SimpleViewer::init_gl();
	// ouvre une fenetre QGLViewer avec fond noir et une scene de rayon 10
	SimpleViewer viewer(NOIR,10);

	// GL init
	viewer.f_init = [&] ()
	{
		prim.gl_init();
	};

	// drawing
	viewer.f_draw = [&] ()
	{
		prim.set_matrices(viewer.getCurrentModelViewMatrix(), viewer.getCurrentProjectionMatrix());

        //prim.draw_sphere(Mat4(), BLANC);
        //prim.draw_cube(translate(2,0,0), ROUGE);
        //prim.draw_cone(translate(0,2,0), VERT);
        //prim.draw_cylinder(translate(0,0,2), BLEU);
        // draw_repere(translate(0,0,5)*scale(0.1,0.1,0.1));


        /*draw_repere(Mat4());
        for(int b=0; b<360; b+=36) { //10 repères séparés par 36 degres
            int a = b+compteur;
            Mat4 tr = rotateX(20)*rotateZ(a) * translate(0,-10,0) * rotateX(a)*scale(0.5,0.5,0.5);

        }
        draw_repere(tr);*/

     /*  doigt(Mat4(),-10,1);
        doigt(Mat4()* translate(0,2,0),0,1);
        doigt(Mat4()* translate(0,4,0),10,1);
        doigt(Mat4()* translate(-2,-0.5,0),-80,0.5);*/

        paume(Mat4());

	//	articulations(Mat4());
	};


	// to do when key pressed
	viewer.f_keyPress = [&] (int key, Qt::KeyboardModifiers /*mod*/)
	{
		switch(key)
		{
			case Qt::Key_A: // touche 'a'
				if (viewer.animationIsStarted())
					viewer.stopAnimation();
				else
					viewer.startAnimation();
				break;
			default:
				break;
		}
	};

	// to do every 50th of sec
	viewer.f_animate = [&] ()
	{
		compteur++;
		std::cout << "animate :" << compteur<< std::endl;
	};


	viewer.show();
	return a.exec();
}
