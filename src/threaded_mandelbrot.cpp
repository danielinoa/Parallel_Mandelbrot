// Linux Compile: gcc -o <compiled_code_name> <code_name>.cpp -lGL -lglut -lGLU -lpthread -lpthread -lstdc++
//
//  Code to display a grayscale version of the mandelbrot set
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cmath>

// #include <GLUT/glut.h> // Used in Xcode
#include <GL/glut.h> //GLUT library - Used in URI Clusters

#include <sys/time.h> //timing library
#include <pthread.h> // Pthreads

#define X_WINDOW 1000
#define Y_WINDOW 1000
#define MAX_COUNT 512

unsigned char image[Y_WINDOW][X_WINDOW][3];
float center_real = -0.5;
float center_imaginary = 0.0;
float radius = 1.0;

// Structure used to handle parameters within pointer based mandelbrot function.
struct obj{
    int lo, hi;
    int x, y;
    float re, im;
    int count;
};

/*
 /  Calculate Z=Z^2+C for given C
 */
int mandelbrot(float C_real, float C_imaginary, int max_count){
    int count = 0;
    float Z_real = 0;
    float Z_imaginary = 0;
    float magnitude = 0;
    float max_magnitude = 100000;
    
    // Calculate Z=Z^2+C for given C
    while ((magnitude < max_magnitude) && (count++ < max_count)){
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
 Initialize mandelbrot image
 */
void *mandelbrotImage(void *arguments){
    
    struct obj *args = (struct obj *)arguments;
    int hi = (args->hi);    // storing structure hi value into local hi value
    int lo = (args->lo);    // storing structure lo value into local lo value
    
    //    printf("%d \n", lo);
    //    printf("%d \n", hi);
    
    // if lo is bigger than hi stop process by returning 0;
    if(lo > hi){
        return 0;
        // if lo equals hi we have stripped the range of y values into one y value
    } else if (lo == hi){
        
        int y = hi;
        
        // with a fixed value of y we iterate through all the x's to populate the 3D image array
        for (int x = 0; x < X_WINDOW; x++){
            
            float C_real = center_real - radius + (2 * radius * x) / X_WINDOW;
            float C_imaginary = center_imaginary - radius + (2 * radius * y) / Y_WINDOW;
            
            if(y < Y_WINDOW)
                image[y][x][0] = image[y][x][1] = image[y][x][2] = 4 * mandelbrot(C_real, C_imaginary, MAX_COUNT);
            else
                break;
        }
        /* 
         if hi is greater than lo then we keep diving,
            the first half is dispatched on a new thread
            the second half is recursively dispatched on the main thread.
         */
    } else {
        
        int mid = (lo+hi)/2;
        
        obj *obj1 = new struct obj;
        obj1->lo = mid+1, obj1->hi = hi;
        
            pthread_t thread1;
            pthread_create( &thread1, NULL, mandelbrotImage, obj1);
        
        obj *obj2 = new struct obj;
        obj2->lo = lo, obj2->hi = mid;
        mandelbrotImage(obj2);
        
        // pthread_join(thread1, NULL); // Joining the incoming Pthread would slow down performance, and it is not needed since no returning value is expected.
    }
    
    return 0;
}



/*
 /   Display callback for OpenGL
 */
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(X_WINDOW, Y_WINDOW, GL_RGB, GL_UNSIGNED_BYTE, image);
    glFlush();
}

/*
 / Method to calculate the time differences in program execution
 */
double timeDifference (struct timeval * start, struct timeval * end){
    struct timeval diff;
    
    if (start->tv_sec == end->tv_sec) {
        diff.tv_sec = 0;
        diff.tv_usec = end->tv_usec - start->tv_usec;
    } else {
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

int main(int argc, char *argv[]){
    
    struct timeval programStart, programEnd, calculationStart, calculationEnd;
    gettimeofday(&programStart, NULL);
    
    // set up the glut window
    glutInit(&argc, argv);
    glutInitWindowSize(X_WINDOW, Y_WINDOW);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutCreateWindow("CSC 415 - Parallelized Mandelbrot");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    gettimeofday(&calculationStart, NULL);
    
    // Prepare the structure with the low and high value
    obj *obj = new struct obj;
    obj->lo = 0;
    obj->hi = Y_WINDOW;
    
    // dispatch prepared structure into recursive mandelbrot function.
    mandelbrotImage(obj);
    
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
