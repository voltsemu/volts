#pragma once

#define STR_INNER(X) #X
#define STR(X) STR_INNER(X)

#define ROUND2(N, A) ((N + A - 1) & -A)