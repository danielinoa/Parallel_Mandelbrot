// Linux Compile: gcc -o <name> mandelbrot.cpp -lGL -lglut -lGLU
//
//  Code to display a grayscale version of the mandelbrot set
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cmath>
#include <GL/glut.h> //GLUT library
#include <sys/time.h> //timing library

#define X_WINDOW 1000
#define Y_WINDOW 1000
unsigned char image[Y_WINDOW][X_WINDOW][3];
float center_real = -0.5;
float center_imaginary = 0.0;
float radius = 1.0;

/*
/  Calculate Z=Z^2+C for given C
*/
int mandelbrot(float C_real, float C_imaginary, int max_count)
{
   int count = 0;
   float Z_real = 0;
   float Z_imaginary = 0;
   float magnitude = 0;
   float max_magnitude = 100000;

   // Calculate Z=Z^2+C for given C
   while ((magnitude < max_magnitude) && (count++ < max_count))
   {
      float N_real = Z_real * Z_real - Z_imaginary * Z_imaginary + C_real;
      float N_imaginary = Z_real * Z_imaginary + Z_imaginary * Z_real + C_imaginary;
      Z_real = N_real;
      Z_imaginary = N_imaginary;
      magnitude = Z_real * Z_real + Z_imaginary * Z_imaginary;
   }

   // Return number of iterations 
   if (count >= max_count)
      return 0;
   else
      return count;
}

/*
/   Initialize mandelbrot image
*/
void mandelbrotImage(int max_count)
{
   for (int y = 0; y < Y_WINDOW; y++)
      for (int x = 0; x < X_WINDOW; x++)
      {
	 float C_real = center_real - radius + (2 * radius * x) / X_WINDOW;
	 float C_imaginary = center_imaginary - radius + (2 * radius * y) / Y_WINDOW;
	 image[y][x][0] = image[y][x][1] = image[y][x][2] = 4 * mandelbrot(C_real, C_imaginary, max_count);
      }
}

/*
/   Display callback for OpenGL
*/
void display()
{
   glClear(GL_COLOR_BUFFER_BIT);
   glDrawPixels(X_WINDOW, Y_WINDOW, GL_RGB, GL_UNSIGNED_BYTE, image);
   glFlush();
}

/*
/ Method to calculate the time differences in program execution
*/
double timeDifference (struct timeval * start, struct timeval * end)
{
	struct timeval diff;
  
	if (start->tv_sec == end->tv_sec) {
    	diff.tv_sec = 0;
    	diff.tv_usec = end->tv_usec - start->tv_usec;
	}
  	else {
    	diff.tv_usec = 1000000 - start->tv_usec;
    	diff.tv_sec = end->tv_sec - (start->tv_sec + 1);
    	diff.tv_usec += end->tv_usec;
    	
		if (diff.tv_usec >= 1000000) {
    	  	diff.tv_usec -= 1000000;
     		diff.tv_sec += 1;
    	}
  	}
	
  	return ((double)diff.tv_sec + (diff.tv_usec / 1.0e6)) * 1.0e6;
}

int main(int argc, char *argv[])
{
    struct timeval programStart, programEnd, calculationStart, calculationEnd;
    gettimeofday(&programStart, NULL);
    
    // set up the glut window
    glutInit(&argc, argv);
    glutInitWindowSize(X_WINDOW, Y_WINDOW);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutCreateWindow("CSC 415 - Serial Mandelbrot");
    glClearColor(0.0, 0.0, 0.0, 0.0);

    gettimeofday(&calculationStart, NULL);
    mandelbrotImage(512);
    gettimeofday(&calculationEnd, NULL);

    
   // Specify callback function
   glutDisplayFunc(display);

   gettimeofday(&calculationEnd, NULL);
   gettimeofday(&programEnd, NULL);  
   printf("Single-threaded Mandelbrot Fractal (Full Program): %.2f sec\n", timeDifference(&programStart, &programEnd)/1000000);
   printf("Single-threaded Mandelbrot Fractal (Calculation): %.2f sec\n", timeDifference(&calculationStart, &calculationEnd)/1000000);
   printf("Parallelizeable Percentage: %.2f %% \n", (timeDifference(&calculationStart, &calculationEnd) / timeDifference(&programStart, &programEnd))*100);
   printf("Potential Speedup: %.2f x \n",(timeDifference(&programStart, &programEnd)/(timeDifference(&programStart, &programEnd)-timeDifference(&calculationStart, &calculationEnd))));

   glutMainLoop();
   return 0;
}
