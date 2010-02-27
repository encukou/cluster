#include "reporting.h"
#include "process_h.h"
#include "cbdata.h"

#if defined __GNUC__
// There are lots of unused parameters here, and leaving out their names
// would just result in confusion. I think this is a sufficient reason
// to disable the unused-parameter warning
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

void PrintHeader(int quietLevel) {}

void PrintIterationKM(int quietLevel, int i, int iter, double error,
                      double time, void* codebook) {
    QVariantMap results;
    results["error"] = error;
    results["output"] = QVariant::fromValue<DataWrapperPtr>(DataWrapperPtr(new CBData((CODEBOOK*)codebook)));
    Process::c_report_static(results);
}

void PrintRepeat(int quietLevel, int repeats, int i, int iter, double error,
                 double time, int better) {}

void PrintFooterKM(int quietLevel, double error, int repeats,
                   double totalTime, int totalIter) {}

void PrintIterationRLS(int quietLevel, int iter, double error, double time,
                       int better, void* codebook) {
    if (better)
    {
        QVariantMap results;
        results["error"] = error;
        results["output"] = QVariant::fromValue<DataWrapperPtr>(DataWrapperPtr(new CBData((CODEBOOK*)codebook)));
        results["iteration_num"] = iter;
        Process::c_report_static(results);
    }
}

void PrintFooterRLS(int quietLevel, int iter, double error, double time) {
    Q_ASSERT_X(false, __FILE__, "// TODO: RLS reporting");
}

