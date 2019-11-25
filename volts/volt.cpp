#include "volt.h"

#include "generated/version.gen.h"

extern "C" {

const char* branch(void)
{
    return GIT_BRANCH;
}

int commit(void)
{
    return GIT_COMMIT;
}

}