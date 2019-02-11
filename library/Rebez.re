// Adapted from https://github.com/gre/bezier-easing

exception RangeError(string);

// These values are established by empiricism with tests (tradeoff: performance VS precision)
let newtonIterations = 4;
let newtonMinSlope = 0.001;
let subdivisionPrecision = 0.0000001;
let subdivisionMaxIterations = 10;

let kSplineTableSize = 11;
let kSampleStepSize = 1.0 /. (float_of_int(kSplineTableSize) -. 1.0);

let a = (aA1, aA2) => {
  1.0 -. 3.0 *. aA2 +. 3.0 *. aA1;
};
let b = (aA1, aA2) => {
  3.0 *. aA2 -. 6.0 *. aA1;
};
let c = aA1 => {
  3.0 *. aA1;
};

// Returns x(t) given t, x1, and x2, or y(t) given t, y1, and y2.
let calcBezier = (aT, aA1, aA2) => {
  ((a(aA1, aA2) *. aT +. b(aA1, aA2)) *. aT +. c(aA1)) *. aT;
};

// Returns dx/dt given t, x1, and x2, or dy/dt given t, y1, and y2.
let getSlope = (aT, aA1, aA2) => {
  3.0 *. a(aA1, aA2) *. aT *. aT +. 2.0 *. b(aA1, aA2) *. aT +. c(aA1);
};

let binarySubdivide = (aX, aA, aB, mX1, mX2) => {
  let rec subdivide = (i, aA, aB) => {
    let currentT = aA +. (aB -. aA) /. 2.0;
    let currentX = calcBezier(currentT, mX1, mX2) -. aX;
    let continue =
      abs_float(currentX) > subdivisionPrecision
      && i
      + 1 < subdivisionMaxIterations;
    if (continue) {
      currentX > 0.0
        ? subdivide(i + 1, aA, currentT) : subdivide(i + 1, currentT, aB);
    } else {
      currentT;
    };
  };
  subdivide(0, aA, aB);
};

let newtonRaphsonIterate = (aX, aGuessT, mX1, mX2) => {
  let rec newton = (guess, attempts) => {
    let currentSlope = getSlope(guess, mX1, mX2);
    let goodEnough = currentSlope == 0.0;
    if (goodEnough || attempts === newtonIterations - 1) {
      guess;
    } else {
      let bez = calcBezier(guess, mX1, mX2);
      let currentX = bez -. aX;
      let newGuess = guess -. currentX /. currentSlope;
      newton(newGuess, attempts + 1);
    };
  };
  newton(aGuessT, 0);
};

let linearEasing = x => x;

let makeEasing = (mX1, mY1, mX2, mY2) => {
  // Precompute samples table
  let sampleValues = Array.make(kSplineTableSize, 0.);
  for (i in 0 to kSplineTableSize - 1) {
    sampleValues[i] =
      calcBezier(float_of_int(i) *. kSampleStepSize, mX1, mX2);
  };
  let getTForX = aX => {
    let lastSample = kSplineTableSize - 1;
    let rec getCurrentSample = (currentSample, intervalStart) => {
      currentSample !== lastSample && sampleValues[currentSample] <= aX
        ? getCurrentSample(
            currentSample + 1,
            intervalStart +. kSampleStepSize,
          )
        : (currentSample - 1, intervalStart);
    };
    let (currentSample, intervalStart) = getCurrentSample(1, 0.0);
    // Interpolate to provide an initial guess for t
    let dist =
      (aX -. sampleValues[currentSample])
      /. (sampleValues[currentSample + 1] -. sampleValues[currentSample]);
    let guessForT = intervalStart +. dist *. kSampleStepSize;

    let initialSlope = getSlope(guessForT, mX1, mX2);
    if (initialSlope >= newtonMinSlope) {
      newtonRaphsonIterate(aX, guessForT, mX1, mX2);
    } else if (initialSlope === 0.0) {
      guessForT;
    } else {
      binarySubdivide(
        aX,
        intervalStart,
        intervalStart +. kSampleStepSize,
        mX1,
        mX2,
      );
    };
  };

  let easing = x =>
    // We should guarantee the extremes are right.
    switch (x) {
    | 0. => 0.
    | 1. => 1.
    | _ => calcBezier(getTForX(x), mY1, mY2)
    };
  easing;
};

let make = (mX1, mY1, mX2, mY2) => {
  if (!(0. <= mX1 && mX1 <= 1. && 0. <= mX2 && mX2 <= 1.)) {
    raise(RangeError("bezier x values must be in [0, 1] range"));
  };

  if (mX1 === mY1 && mX2 === mY2) {
    linearEasing;
  } else {
    makeEasing(mX1, mY1, mX2, mY2);
  };
};