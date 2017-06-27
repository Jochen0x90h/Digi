// fabs
BIND_C_FUNCTION(module, "fabs", (double (*)(double))fabs);

// floor
BIND_C_FUNCTION(module, "floor", (double (*)(double))floor);

// ceil
BIND_C_FUNCTION(module, "ceil", (double (*)(double))ceil);

// round
BIND_C_FUNCTION(module, "round", (double (*)(double))round);

// trunc
BIND_C_FUNCTION(module, "trunc", (double (*)(double))trunc);

// fmod
BIND_C_FUNCTION(module, "fmod", (double (*)(double, double))fmod);

// sqrt
BIND_C_FUNCTION(module, "sqrt", (double (*)(double))sqrt);

// sqrtf
BIND_C_FUNCTION(module, "sqrtf", (float (*)(float))sqrtf);

// exp
BIND_C_FUNCTION(module, "exp", (double (*)(double))exp);

// pow
BIND_C_FUNCTION(module, "pow", (double (*)(double, double))pow);

// log
BIND_C_FUNCTION(module, "log", (double (*)(double))log);

// log10
BIND_C_FUNCTION(module, "log10", (double (*)(double))log10);

// sin
BIND_C_FUNCTION(module, "sin", (double (*)(double))sin);

// sinf
BIND_C_FUNCTION(module, "sinf", (float (*)(float))sinf);

// cos
BIND_C_FUNCTION(module, "cos", (double (*)(double))cos);

// cosf
BIND_C_FUNCTION(module, "cosf", (float (*)(float))cosf);

// tan
BIND_C_FUNCTION(module, "tan", (double (*)(double))tan);

// asin
BIND_C_FUNCTION(module, "asin", (double (*)(double))asin);

// acos
BIND_C_FUNCTION(module, "acos", (double (*)(double))acos);

// atan
BIND_C_FUNCTION(module, "atan", (double (*)(double))atan);

// atan2
BIND_C_FUNCTION(module, "atan2", (double (*)(double, double))atan2);

