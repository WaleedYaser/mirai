#include "core/asserts.h"
#include "core/logger.h"

void
mc_report_assertion_failure(const char *expression, const char *message, const char *file, i32 line)
{
    mc_log(MC_LOG_LEVEL_FATAL, "Assertion Failure: %s, message: %s, in file: %s, line: %d\n",
        expression, message, file, line);
}
