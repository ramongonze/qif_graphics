#ifndef _qif_graphics
#define _qif_graphics

#include "../libs/qif/qif.h"
#include "../libs/raylib/src/raylib.h"
#include <string>

using namespace std;

// Errors ------------------------------------------------------------------------------------
#define NO_ERROR 0
#define INVALID_CHANNEL 1
#define INVALID_PRIOR 2 
#define INVALID_VALUE 3 // i.e. "1/$2"
#define INVALID_QIF_FILE 4

// Settings ------------------------------------------------------------------------------------
#define WINDOWS_WIDTH 750
#define WINDOWS_HEIGHT 630
#define TEXTBOX_SIZE 50
#define PROB_PRECISION 3 // Precision of float numbers (# digits after .)
#define CHAR_BUFFER_SIZE 128
#define NUMBER_SECRETS 3
#define MAX_CHANNEL_OUTPUTS 100

// Color scheme ------------------------------------------------------------------------------------
// GUI
#define BG_BASE_COLOR_LIGHT  CLITERAL(Color){210, 210, 210, 255}
#define BG_BASE_COLOR_DARK  CLITERAL(Color){179, 179, 179, 255}
#define MENU_BASE_COLOR_NORMAL  CLITERAL(Color){25, 41, 51, 255}
#define MENU_BASE_COLOR_FOCUSED  CLITERAL(Color){76, 124, 154, 255}
#define MENU_BASE_COLOR_PRESSED  CLITERAL(Color){50, 83, 103, 255}
#define TITLES_BASE_COLOR  CLITERAL(Color){27, 58, 130, 255}
#define TITLES_BASE_COLOR_DARKER  CLITERAL(Color){17, 38, 85, 255}

// Circles
#define PRIOR_COLOR  CLITERAL(Color){0, 102, 204, 210}
#define PRIOR_COLOR_LINES  CLITERAL(Color){0, 102, 204, 240}
#define INNERS_COLOR  CLITERAL(Color){40, 164, 40, 210}
#define INNERS_COLOR_LINES  CLITERAL(Color){40, 164, 40, 240}

// Prior probability distribution radius (in pixels) ----------------------------------------------/
#define PRIOR_RADIUS 50

class Point{
	public:
		long double x;
		long double y;

		Point();
		Point(long double x, long double y);
};

typedef struct Circle{
	Point center; // Pixel coordinates
	float radius;
}Circle;

/* Transforms a probability distribution on 3 elements in a barycentric coordinate
 * Parameters:
 * 		prior: Probability distribution
 *
 * Return: A 'Point' structure containing a barycentric coordinate.
 */
Point dist2Bary(Distribution &prior);

/* Transforms a probability distribution on 3 elements in a barycentric coordinate
 * Parameters:
 * 		x1: Probability of the first element
 * 		x2: Probability of the second element
 * 		x3: Probability of the third element
 *
 * Return: A 'Point' structure containing a barycentric coordinate.
 */
Point dist2Bary(long double x1, long double x2, long double x3);

/* Transforms a barycentric coordiante in a probability distribution on 3 elements.
 * Parameters:
 *		p: 	  Point containing a barycentric coordinate
 *		prob: A vector which will receive the new probability distribution
 *
 * Return: true if a probability distribution was succesfully generated or false otherwise.
 */
bool bary2Dist(Point p, vector<long double> &prob);

// Euclidian distance between two points
long double euclidianDistance(Point a, Point b);
long double euclidianDistance(Point a, Vector2 b);

/* Transforms a pixel coordinate in barycentric coordinate
 * Parameters:
 *		x: value (in pixels) in x-axis
 *		y: value (in pixels) in y-axis
 *		TrianglePoints: values (in pixels) of triangle points
 *
 * Return: A 'Point' structure containing a barycentric coordinate.
 */
Point pixel2Bary(double x, double y, Vector2 TrianglePoints[3]);

/* Transforms barycentric coordinate in pixel coordinate
 * Parameters:
 *		x: value (in barycentric coordinate) in x-axis
 *		y: value (in barycentric coordinate) in y-axis
 *		TrianglePoints: values (in pixels) of triangle points
 *
 * Return: A 'Point' structure containing a barycentric coordinate.
 */
Point bary2Pixel(double x, double y, Vector2 TrianglePoints[3]);

/* Truncate a probability distribution to a given precision.
   The truncation can increase or decrease the probability of the last element.
   Returns a vector of strings containing the probability distribution.
 */
vector<string> getStrTruncatedDist(Distribution dist, int precision);

#endif