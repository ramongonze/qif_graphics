#ifndef _data
#define _data

#include "graphics.h"
#include <exception>
#include <algorithm> // std::random_shuffle
#include <ctime> // std::time
#include <cstdlib> // std::srand
#include <string.h> // strcpy
#include <iostream>

#define ANIMATION_DURATION 1 // Animation in seconds
#define FPS 60
#define STEPS (ANIMATION_DURATION*FPS)
#define UPDATE_CIRCLES_BY_MOUSE -1

class Data{
public:
	Data();

	//------------------------------------------------------------------------------------
    // Attributes
    //------------------------------------------------------------------------------------

	Hyper hyper;				// Hyper-distribution
	vector<vector<long double>> channel;			// Channel matrix
	vector<long double> prior;	// Prior distribution
	int error;		// Indicates if there is error with prior or channel
	bool hyperReady;  // Flag that indicates wheter a hyper distribution has been built.
	bool mouseClickedOnPrior; // Flag that indicates wheter the mouse was clicked in the previous frame on the prior circle.
	bool fileSaved; // Flag that indicates wheter the current prior/channel has been saved
	int animation; // Animation control
	bool animationRunning;

	Circle priorCircle;
	Circle innersCircles[MAX_CHANNEL_OUTPUTS];
	long double xJumpAnimation[MAX_CHANNEL_OUTPUTS]; 		// x axis jump per inner
	long double yJumpAnimation[MAX_CHANNEL_OUTPUTS]; 		// y axis jump per inner

	//------------------------------------------------------------------------------------
    // Methods
    //------------------------------------------------------------------------------------

	/* Check if the numbers or fractions were typed correctly in the prior distribution.
	 * If so, convert text to long double values and add them to this->prior.
	 * Returns NO_ERROR or INVALID_VALUE */
	int checkPriorText(char prior_[NUMBER_SECRETS][CHAR_BUFFER_SIZE]);

	/* Check if the numbers or fractions were typed correctly in the channel.
	 * If so, conver text to long double values and add them to this->channel.
	 * Returns NO_ERROR or INVALID_VALUE */
	int checkChannelText(char channel_[NUMBER_SECRETS][MAX_CHANNEL_OUTPUTS][CHAR_BUFFER_SIZE], int numOutputs);

	/* Create circle points and radius.
	 * It asssumes that the hyper distribution attribute has already been built. */
	void buildCircles(Vector2 TrianglePoints[3]);

	/* Given three points, returns 0 if the orientation is colinear, 1 if it is
	 * clock wise or 2 if it is counterclock wise. */
	int orientation(Point p1, Point p2, Point p3);

	/* Point intersection between lines AB and CD. */
	Point pointIntersection(Point A, Point B, Point C, Point D);

	/* Adjust the new prior distribution. If the mouse position is outside the triangle,
	 * moves the prior circle to the closest point of an edge from the triangle. */
	Point adjustPrior(Vector2 TrianglePoints[3], Vector2 mouse);

	/* Update the hyper distribution if the user moves the prior distribution
     * This function assumes that the hyper distribution has already been built. 
	 *
	 * @Parameters:
	 *		TrianglePoints: From Layout object.
	 *
     */
	void updateHyper(Vector2 TrianglePoints[3]);

	/* Generates a new random prior and keeps it in attribute 'prior'. */
	void newRandomPrior();

	/* Generates a new random channel and keeps it in attribute 'channel'.
	 *
	 * @Parameters:
	 *		num_out: Number of outputs in L->TextBoxesChannelText.
	*/
	void newRandomChannel(int num_out);
};

#endif