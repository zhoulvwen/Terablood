// --------------------------------------------------------------
// 3D Sphere suspended in shear flow
// Terminado Octubre 09 - 2010
// --------------------------------------------------------------

// Tipo de variables a utilizar 
typedef double Real;

// Librerias estandar
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Librerias del método
#include "../include/lbm.h"
#include "../include/ibm.h"
#include "../include/fem.h"

// Librerias para manejar objetos
#include "../include/fluid.h"
#include "../include/mesh.h"

using namespace std;

// ----------------------------------------------------------------
// 0. Conversion a cantidades adimensionales y parametros de flujo
// ----------------------------------------------------------------
float dt = 1.0;
float G = 0.01;
float Re = 0.02;
float ks = 0.0221311;
float u = 4.76e-3;
float tau = 1.0;
float H = 4e-5;
int X = 35;
int Y = 35;
int Z = 35;
float dx=H/X;
//float omega = 1.0/tau;
float R = (X/10.0);
int STEPS = 500000;
int VTK=50;

// -----------------------------------------------------------------------------
// Propiedades y variables para el fluido
// -----------------------------------------------------------------------------
fluid fluido;

// -----------------------------------------------------------------------------
// Propiedades y variables para la membrana
// -----------------------------------------------------------------------------
mesh membrana, referencia;

int main(int argc, char *argv[])
{
	// -----------------------------------------------------------------------//
	// 0. Inicializar
	// -----------------------------------------------------------------------//
	fluido.inicializar(X,Y,Z);
	fluido.setVelocidad(u);
	membrana.mesh_refine_tri4();
	membrana.mesh_refine_tri4();
	membrana.mesh_refine_tri4();
	membrana.proyectarEsfera(R);
	membrana.moverCentro((X-1.0)/2.0, (Y-1.0)/2.0, (Z-1.0)/2.0);
	membrana.iniciarVelocidad();
	membrana.iniciarFuerzas();		
	
	referencia.mesh_refine_tri4();
	referencia.mesh_refine_tri4();
	referencia.mesh_refine_tri4();
	referencia.proyectarEsfera(R);
	referencia.moverCentro((X-1.0)/2.0, (Y-1.0)/2.0, (Z-1.0)/2.0);
	
	for(int ts = 0 ; ts < STEPS ; ts++)
	{
	// -----------------------------------------------------------------------//
	// 1. Interpolation
	// -----------------------------------------------------------------------//
	interpolation(fluido, membrana, X, Y, Z);

	// -----------------------------------------------------------------------//
	// 2. Encontrar nuevas posiciones de la membrana
	// -----------------------------------------------------------------------//
	membrana.moverNodos(dt, dx);

	// -----------------------------------------------------------------------//
	// 3. Calcular fuerzas en los nodos de la membrana
	// -----------------------------------------------------------------------//
	membrana.calcularFuerzas(referencia);

	// -----------------------------------------------------------------------//
	// 4. Propagar la densidad de fuerza hacia el fluido
	// -----------------------------------------------------------------------//
	spread(fluido, membrana, X, Y, Z);

	// -----------------------------------------------------------------------//
	// 5. Solucionar la dinámica del fluido
	// -----------------------------------------------------------------------//
	fluido.stream();
	fluido.collide();
	
	// -----------------------------------------------------------------------//
	// 6. Calcular propiedades macro del fluido
	// -----------------------------------------------------------------------//
	fluido.calcularMacro();

	// -----------------------------------------------------------------------//
	// 7. Calcular propiedades macro de la membrana
	// -----------------------------------------------------------------------//

	// -----------------------------------------------------------------------//
	// 8. Visualización
	// -----------------------------------------------------------------------//
	if(ts%VTK==0)
	{
		fluido.guardar(ts);
		membrana.guardarVTU(ts);
		printf("%d\n",ts);
	}
	}
	return 0;
}