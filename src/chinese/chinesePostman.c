/*PGR-GNU*****************************************************************
File: chinesePostman.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2018 Maoguang Wang
Mail: xjtumg1007@gmail.com

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

/** @file chinesePostman.c */

#include <stdbool.h>
#include "c_common/postgres_connection.h"


#include "c_types/path_rt.h"
#include "c_common/debug_macro.h"
#include "c_common/e_report.h"
#include "c_common/time_msg.h"
#include "c_common/pgdata_getters.h"

#include "drivers/chinese/chinesePostman_driver.h"

PGDLLEXPORT Datum _pgr_chinesepostman(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(_pgr_chinesepostman);


/******************************************************************************/
static
void
process(
        char* edges_sql,
        bool only_cost,
        Path_rt **result_tuples,
        size_t *result_count) {
    pgr_SPI_connect();

    Edge_t *edges = NULL;
    size_t total_edges = 0;

    pgr_get_edges(edges_sql, &edges, &total_edges, true, false);

    if (total_edges == 0) {
        pgr_SPI_finish();
        return;
    }

    clock_t start_t = clock();
    char *log_msg = NULL;
    char *notice_msg = NULL;
    char *err_msg = NULL;

    do_pgr_directedChPP(
            edges, total_edges,
            only_cost,

            result_tuples, result_count,

            &log_msg,
            &notice_msg,
            &err_msg);

    if (only_cost) {
        time_msg(" processing pgr_chinesePostmanCost", start_t, clock());
    } else {
        time_msg(" processing pgr_chinesePostman", start_t, clock());
    }

    if (edges) pfree(edges);

    if (err_msg && (*result_tuples)) {
        pfree(*result_tuples);
        (*result_tuples) = NULL;
        (*result_count) = 0;
    }

    pgr_global_report(log_msg, notice_msg, err_msg);

    if (log_msg) pfree(log_msg);
    if (notice_msg) pfree(notice_msg);
    if (err_msg) pfree(err_msg);

    pgr_SPI_finish();
}


PGDLLEXPORT Datum _pgr_chinesepostman(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
    TupleDesc           tuple_desc;

    /**************************************************************************/
    Path_rt *result_tuples = NULL;
    size_t result_count = 0;
    /**************************************************************************/

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext   oldcontext;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);


        /**********************************************************************/

        process(
                text_to_cstring(PG_GETARG_TEXT_P(0)),
                PG_GETARG_BOOL(1),
                &result_tuples,
                &result_count);

        /**********************************************************************/


        funcctx->max_calls = result_count;

        funcctx->user_fctx = result_tuples;
        if (get_call_result_type(fcinfo, NULL, &tuple_desc)
                != TYPEFUNC_COMPOSITE) {
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                         "that cannot accept type record")));
        }

        funcctx->tuple_desc = tuple_desc;
        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();
    tuple_desc = funcctx->tuple_desc;
    result_tuples = (Path_rt*) funcctx->user_fctx;

    if (funcctx->call_cntr < funcctx->max_calls) {
        HeapTuple    tuple;
        Datum        result;
        Datum        *values;
        bool*        nulls;
        size_t call_cntr = funcctx->call_cntr;

        /**********************************************************************/
        /*                          MODIFY AS NEEDED                          */
        /*
         ***********************************************************************/

        size_t numb = 5;
        values =(Datum *)palloc(numb * sizeof(Datum));
        nulls = palloc(numb * sizeof(bool));


        size_t i;
        for (i = 0; i < numb; ++i) {
            nulls[i] = false;
        }
        values[0] = Int32GetDatum(funcctx->call_cntr + 1);
        values[1] = Int64GetDatum(result_tuples[call_cntr].node);
        values[2] = Int64GetDatum(result_tuples[call_cntr].edge);
        values[3] = Float8GetDatum(result_tuples[call_cntr].cost);
        values[4] = Float8GetDatum(result_tuples[call_cntr].agg_cost);
        /**********************************************************************/

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        SRF_RETURN_DONE(funcctx);
    }
}

