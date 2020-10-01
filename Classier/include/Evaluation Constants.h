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

const double whitePawnPositionValue[8][8]
{
  { .00, .05, .05, .10, .20, .25, .30, .00, },
  { .00, .05, .10, .15, .20, .25, .30, .00, },
  { .00, .05, .10, .15, .20, .25, .30, .00, },
  { .00, .05, .10, .20, .25, .25, .30, .00, },
  { .00, .05, .10, .20, .25, .25, .30, .00, },
  { .00, .05, .10, .15, .20, .25, .30, .00, },
  { .00, .05, .10, .15, .20, .25, .30, .00, },
  { .00, .05, .05, .10, .20, .25, .30, .00, },
};

const double blackPawnPositionValue[8][8]
{
  { .00, .30, .25, .20, .10, .05, .05, .00, },
  { .00, .30, .25, .20, .15, .10, .05, .00, },
  { .00, .30, .25, .20, .15, .10, .05, .00, },
  { .00, .30, .25, .25, .20, .10, .05, .00, },
  { .00, .30, .25, .25, .20, .10, .05, .00, },
  { .00, .30, .25, .20, .15, .10, .05, .00, },
  { .00, .30, .25, .20, .15, .10, .05, .00, },
  { .00, .30, .25, .20, .10, .05, .05, .00, },
};