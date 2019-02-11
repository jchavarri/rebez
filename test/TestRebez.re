let id = x => x;
Random.self_init();

let assertMessage = (cond, message) => {
  assert(
    {
      if (!cond) {
        print_endline(message);
      };
      cond;
    },
  );
};

let assertRaises = f => {
  let res =
    try (
      {
        f();
        false;
      }
    ) {
    | e => true
    };
  assert(res);
};

let assertClose = (precision, a, b, message) => {
  assertMessage(abs_float(a -. b) < precision, message);
};

let allEquals = (~assertion=assertClose(0.000001), be1, be2, samples) => {
  for (i in 0 to samples) {
    let x = float_of_int(i) /. float_of_int(samples);
    assertion(
      be1(x),
      be2(x),
      "comparing "  /* ++ toString(be1) ++ " and " ++ be2*/
      ++ " for value "
      ++ string_of_float(x),
    );
  };
};

let repeat = (n, f) => {
  for (i in 0 to n - 1) {
    f(i);
  };
};

// it('should fail with wrong arguments', let () => {
assertRaises(() => Rebez.make(0.5, 0.5, -5., 0.5));
assertRaises(() => Rebez.make(0.5, 0.5, 5., 0.5));
assertRaises(() => Rebez.make(-2., 0.5, 0.5, 0.5));
assertRaises(() => Rebez.make(2., 0.5, 0.5, 0.5));

/*
 describe('linear curves', let () => {
   it('should be linear', let () => {
 */
allEquals(Rebez.make(0., 0., 1., 1.), Rebez.make(1., 1., 0., 0.), 100);
allEquals(Rebez.make(0., 0., 1., 1.), id, 100);

//  describe('Rebez.make', function(){
//    describe('common properties', let () => {
//      it('should be the right value at extremes', let () => {
repeat(
  1000,
  i => {
    let a = Random.float(1.0);
    let b = 2. *. Random.float(1.0) -. 0.5;
    let c = Random.float(1.0);
    let d = 2. *. Random.float(1.0) -. 0.5;
    let easing = Rebez.make(a, b, c, d);
    // Using value comparison, as floats are reference types
    assertMessage(easing(0.) == 0., "(0) should be 0.");
    assertMessage(easing(1.) == 1., "(1) should be 1.");
  },
);

// it('should approach the projected value of its x=y projected curve', let () => {
repeat(
  1000,
  _ => {
    let a = Random.float(1.0);
    let b = Random.float(1.0);
    let c = Random.float(1.0);
    let d = Random.float(1.0);
    let easing = Rebez.make(a, b, c, d);
    let projected = Rebez.make(b, a, d, c);
    let composed = x => {
      projected(easing(x));
    };
    allEquals(~assertion=assertClose(0.05), id, composed, 100);
  },
);

// describe('two same instances', let () => {
//   it('should be strictly equals', let () => {
repeat(
  100,
  _ => {
    let a = Random.float(1.0);
    let b = 2. *. Random.float(1.0) -. 0.5;
    let c = Random.float(1.0);
    let d = 2. *. Random.float(1.0) -. 0.5;
    allEquals(Rebez.make(a, b, c, d), Rebez.make(a, b, c, d), 100);
  },
);

// describe('symmetric curves', let () => {
//      it('should have a central value y~=0.5 at x=0.5', let () => {
repeat(
  100,
  _ => {
    let a = Random.float(1.0);
    let b = 2. *. Random.float(1.0) -. 0.5;
    let c = 1. -. a;
    let d = 1. -. b;
    let easing = Rebez.make(a, b, c, d);
    let easingZeroPointFive = easing(0.5);
    assertClose(
      // Can't get too precise as values close to the limit like
      // Rebez.make(0.999933706888, -0.386955038209, 6.62931115362e-05, 1.38695503821)
      // will lead to too edgy results
      0.01,
      easingZeroPointFive,
      0.5,
      "(0.5) should be 0.5, was "
      ++ string_of_float(easingZeroPointFive)
      ++ ", with a: "
      ++ string_of_float(a)
      ++ ", b: "
      ++ string_of_float(b)
      ++ ", c: "
      ++ string_of_float(c)
      ++ ", d: "
      ++ string_of_float(d),
    );
  },
);

// it('should be symmetrical', let () => {
repeat(
  100,
  _ => {
    let a = Random.float(1.0);
    let b = 2. *. Random.float(1.0) -. 0.5;
    let c = 1. -. a;
    let d = 1. -. b;
    let easing = Rebez.make(a, b, c, d);
    let sym = x => 1. -. easing(1. -. x);
    // Can't get too precise as values close to the limits will lead to too edgy results
    allEquals(~assertion=assertClose(0.01), easing, sym, 100);
  },
);

//  it('should return the right value for Rebez.make(0., 0.99, 0., 0.99, 0.01)', let () => {
let easing = Rebez.make(0., 0.99, 0., 0.99);
let x = 0.01;
assertClose(
  0.000001,
  easing(0.01),
  0.512011914581,
  "Rebez.make(0., 0.99, 0., 0.99, 0.01) should be roughly 0.512011914581",
);