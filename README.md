# rebez

Cubic bezier implementation in Reason / OCaml.

Adapted from the JavaScript version in https://github.com/gre/bezier-easing.

BezierEasing provides Cubic Bezier Curve easing which generalizes easing functions (ease-in, ease-out, ease-in-out, etc) exactly like in CSS Transitions.

Implementing efficient lookup is not easy because it implies projecting the X coordinate to a Bezier Curve. This micro library uses fast heuristics (involving dichotomic search, newton-raphson, sampling) to focus on performance and precision.

> It is heavily based on implementations available in Firefox and Chrome (for the CSS transition-timing-function property).


## Install

With `esy`, add to your `package.json`:

```json
{
  "dependencies": {
    "rebez": "*",
  },
  "resolutions": {
    "rebez": "github:jchavarri/rebez",
  },
}
```

## Using

In Reason:

```reason
let easing = Rebez.make(0., 0.99, 0., 0.99);
// `easing` is a function that can receive values from 0.0 to 1.0
let value = easing(0.01); // 0.512011914581
```

In OCaml:

```ocaml
let easing = Rebez.make 0. 0.99 0. 0.99
(* `easing` is a function that can receive values from 0.0 to 1.0 *)
let value = easing 0.01  (* 0.512011914581 *)
```

## Developing:

```
npm install -g esy
git clone https://github.com/jchavarri/rebez/
esy install
esy build
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
