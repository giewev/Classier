#include <inttypes.h>
#define bitBoard uint64_t
#define EVAL_CONST_COUNT 18

const double pawnValue = 1;
const double knightValue = 3;
const double bishopValue = 3;
const double rookValue = 5;
const double queenValue = 9;
const double knightBishopWeight = 0.1;
const double doubledPawnValue = -0.15;
const double isolatedPawnValue = -0.15;

const double centerPawnValue = 0.2;
const double semiCenterPawnValue = 0.15;
const double rimKnightValue = -0.10;
const double developmentValue = 0.25;
const double kingCornerValue = 0.05;

const double pawnAdvancementValue = 0.02;
const double passedPawnValue = 0.4;

const double mobilityValue = 0.01;

const double whitePawnPositionValue[64]
{
	.00, .00, .00, .00, .00, .00, .00, .00,
	.05, .05, .05, .05, .05, .05, .05, .05,
	.10, .10, .10, .10, .10, .10, .10, .10,
	.15, .15, .15, .25, .25, .15, .15, .15,
	.20, .20, .20, .25, .25, .20, .20, .20,
	.25, .25, .25, .25, .25, .25, .25, .25,
	.30, .30, .30, .30, .30, .30, .30, .30,
	.00, .00, .00, .00, .00, .00, .00, .00,
};

const double blackPawnPositionValue[64]
{
	.00, .00, .00, .00, .00, .00, .00, .00,
	.30, .30, .30, .30, .30, .30, .30, .30,
	.25, .25, .25, .25, .25, .25, .25, .25,
	.20, .20, .20, .25, .25, .20, .20, .20,
	.15, .15, .15, .25, .25, .15, .15, .15,
	.10, .10, .10, .10, .10, .10, .10, .10,
	.05, .05, .05, .05, .05, .05, .05, .05,
	.00, .00, .00, .00, .00, .00, .00, .00,
};

const double whiteKnightPositionValue[64]
{
   .00, .00, .00, .00, .00, .00, .00, .00, 
   .00, .10, .10, .10, .10, .10, .10, .00, 
   .00, .10, .20, .20, .20, .20, .10, .00, 
   .00, .10, .20, .30, .30, .20, .10, .00, 
   .00, .10, .20, .30, .30, .20, .10, .00, 
   .00, .10, .20, .20, .20, .20, .10, .00, 
   .00, .10, .10, .10, .10, .10, .10, .00, 
   .00, .00, .00, .00, .00, .00, .00, .00, 
};

const double blackKnightPositionValue[64]
{
   .00, .00, .00, .00, .00, .00, .00, .00,
   .00, .10, .10, .10, .10, .10, .10, .00,
   .00, .10, .20, .20, .20, .20, .10, .00,
   .00, .10, .20, .30, .30, .20, .10, .00,
   .00, .10, .20, .30, .30, .20, .10, .00,
   .00, .10, .20, .20, .20, .20, .10, .00,
   .00, .10, .10, .10, .10, .10, .10, .00,
   .00, .00, .00, .00, .00, .00, .00, .00,
};

const double whiteBishopPositionValue[64]
{
   .00, .00, .00, .00, .00, .00, .00, .00, 
   .00, .10, .10, .10, .10, .10, .10, .00, 
   .00, .10, .15, .15, .15, .15, .10, .00, 
   .00, .10, .15, .20, .20, .15, .10, .00, 
   .00, .10, .15, .20, .20, .15, .10, .00, 
   .00, .10, .15, .15, .15, .15, .10, .00, 
   .00, .10, .10, .10, .10, .10, .10, .00, 
   .00, .00, .00, .00, .00, .00, .00, .00, 
};

const double blackBishopPositionValue[64]
{
   .00, .00, .00, .00, .00, .00, .00, .00,
   .00, .10, .10, .10, .10, .10, .10, .00,
   .00, .10, .15, .15, .15, .15, .10, .00,
   .00, .10, .15, .20, .20, .15, .10, .00,
   .00, .10, .15, .20, .20, .15, .10, .00,
   .00, .10, .15, .15, .15, .15, .10, .00,
   .00, .10, .10, .10, .10, .10, .10, .00,
   .00, .00, .00, .00, .00, .00, .00, .00,
};
