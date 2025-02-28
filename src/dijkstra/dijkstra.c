/*PGR-GNU*****************************************************************

File: many_to_many_dijkstra.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: vicky_vergara@hotmail.com

Copyright (c) 2020 The combinations_sql signature is added by Mahmoud SAKR
and Esteban ZIMANYI
mail: m_attia_sakr@yahoo.com, estebanzimanyi@gmail.com

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

#include <stdbool.h>

#include "c_common/postgres_connection.h"
#include "utils/array.h"


#include "c_types/path_rt.h"
#include "c_common/debug_macro.h"
#include "c_common/e_report.h"
#include "c_common/time_msg.h"
#include "c_common/arrays_input.h"
#include "c_common/pgdata_getters.h"
#include "drivers/dijkstra/dijkstra_driver.h"

PG_MODULE_MAGIC;

PGDLLEXPORT Datum _pgr_dijkstra(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(_pgr_dijkstra);

static
void
process(
        char* edges_sql,
        ArrayType *starts,
        ArrayType *ends,
        bool directed,
        bool only_cost,
        bool normal,
        int64_t n_goals,
        bool global,
        Path_rt **result_tuples,
        size_t *result_count) {
    pgr_SPI_connect();

    int64_t* start_vidsArr = NULL;
    size_t size_start_vidsArr = 0;

    int64_t* end_vidsArr = NULL;
    size_t size_end_vidsArr = 0;

    Edge_t *edges = NULL;
    size_t total_edges = 0;
    if (normal) {
        pgr_get_edges(edges_sql, &edges, &total_edges, true, false);
        start_vidsArr = (int64_t*)
            pgr_get_bigIntArray(&size_start_vidsArr, starts);
        end_vidsArr = (int64_t*)
            pgr_get_bigIntArray(&size_end_vidsArr, ends);
    } else {
        pgr_get_edges(edges_sql, &edges, &total_edges, false, false);
        end_vidsArr = (int64_t*)
            pgr_get_bigIntArray(&size_end_vidsArr, starts);
        start_vidsArr = (int64_t*)
            pgr_get_bigIntArray(&size_start_vidsArr, ends);
    }

    if (total_edges == 0) {
        if (end_vidsArr) pfree(end_vidsArr);
        if (start_vidsArr) pfree(start_vidsArr);
        pgr_SPI_finish();
        return;
    }

    clock_t start_t = clock();
    char* log_msg = NULL;
    char* notice_msg = NULL;
    char* err_msg = NULL;
    do_pgr_many_to_many_dijkstra(
            edges, total_edges,
            start_vidsArr, size_start_vidsArr,
            end_vidsArr, size_end_vidsArr,

            directed,
            only_cost,
            normal,
            n_goals,
            global,

            result_tuples,
            result_count,

            &log_msg,
            &notice_msg,
            &err_msg);

    if (only_cost) {
        if (n_goals > 0) {
            time_msg("processing pgr_dijkstraNearCost", start_t, clock());
        } else {
            time_msg("processing pgr_dijkstraCost", start_t, clock());
        }
    } else {
        if (n_goals > 0) {
            time_msg("processing pgr_dijkstraNear", start_t, clock());
        } else {
            time_msg("processing pgr_dijkstra", start_t, clock());
        }
    }


    if (err_msg && (*result_tuples)) {
        pfree(*result_tuples);
        (*result_tuples) = NULL;
        (*result_count) = 0;
    }

    pgr_global_report(log_msg, notice_msg, err_msg);

    if (log_msg) pfree(log_msg);
    if (notice_msg) pfree(notice_msg);
    if (err_msg) pfree(err_msg);
    if (edges) pfree(edges);
    if (start_vidsArr) pfree(start_vidsArr);
    if (end_vidsArr) pfree(end_vidsArr);
    pgr_SPI_finish();
}



static
void
process_combinations(
        char* edges_sql,
        char* combinations_sql,
        bool directed,
        bool only_cost,
        int64_t n_goals,
        bool global,
        Path_rt **result_tuples,
        size_t *result_count) {
    pgr_SPI_connect();

    Edge_t *edges = NULL;
    size_t total_edges = 0;

    II_t_rt *combinations = NULL;
    size_t total_combinations = 0;

    pgr_get_edges(edges_sql, &edges, &total_edges, true, false);

    if (total_edges == 0) {
        pgr_SPI_finish();
        return;
    }
    pgr_get_combinations(combinations_sql, &combinations, &total_combinations);
    if (total_combinations == 0) {
        if (edges) pfree(edges);
        pgr_SPI_finish();
        return;
    }

    clock_t start_t = clock();
    char* log_msg = NULL;
    char* notice_msg = NULL;
    char* err_msg = NULL;
    do_pgr_combinations_dijkstra(
            edges, total_edges,
            combinations, total_combinations,
            directed,
            only_cost,
            true,
            n_goals,
            global,

            result_tuples,
            result_count,

            &log_msg,
            &notice_msg,
            &err_msg);

    if (only_cost) {
        if (n_goals > 0) {
            time_msg("Processing pgr_dijkstraNearCost", start_t, clock());
        } else {
            time_msg("Processing pgr_dijkstraCost", start_t, clock());
        }
    } else {
        if (n_goals > 0) {
            time_msg("Processing pgr_dijkstraNear", start_t, clock());
        } else {
            time_msg("Processing pgr_dijkstra", start_t, clock());
        }
    }

    if (err_msg && (*result_tuples)) {
        pfree(*result_tuples);
        (*result_tuples) = NULL;
        (*result_count) = 0;
    }

    pgr_global_report(log_msg, notice_msg, err_msg);

    if (log_msg) pfree(log_msg);
    if (notice_msg) pfree(notice_msg);
    if (err_msg) pfree(err_msg);
    if (edges) pfree(edges);
    if (combinations) pfree(combinations);
    pgr_SPI_finish();
}




PGDLLEXPORT Datum
_pgr_dijkstra(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
    TupleDesc            tuple_desc;

    /**********************************************************************/
    Path_rt  *result_tuples = NULL;
    size_t result_count = 0;
    /**********************************************************************/

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext   oldcontext;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
        if (PG_NARGS() == 7) {
            /**********************************************************************/
            // pgr_dijkstra(
            // sql TEXT,
            // start_vids ANYARRAY,
            // end_vids ANYARRAY,
            // directed BOOLEAN default true,
            // only_cost BOOLEAN default false
            // normal BOOLEAN default true

            process(
                    text_to_cstring(PG_GETARG_TEXT_P(0)),
                    PG_GETARG_ARRAYTYPE_P(1),
                    PG_GETARG_ARRAYTYPE_P(2),
                    PG_GETARG_BOOL(3),
                    PG_GETARG_BOOL(4),
                    PG_GETARG_BOOL(5),
                    PG_GETARG_INT64(6),
                    true,
                    &result_tuples,
                    &result_count);

            /**********************************************************************/
        } else if (PG_NARGS() == 5) {
            /**********************************************************************/
            // pgr_dijkstra(
            // edge_sql TEXT,
            // combinations_sql TEXT,
            // directed BOOLEAN default true,
            // only_cost BOOLEAN default false

            process_combinations(
                    text_to_cstring(PG_GETARG_TEXT_P(0)),
                    text_to_cstring(PG_GETARG_TEXT_P(1)),
                    PG_GETARG_BOOL(2),
                    PG_GETARG_BOOL(3),
                    0, true,
                    &result_tuples,
                    &result_count);

        } else if (PG_NARGS() == 8) {
            process(
                    text_to_cstring(PG_GETARG_TEXT_P(0)),
                    PG_GETARG_ARRAYTYPE_P(1),
                    PG_GETARG_ARRAYTYPE_P(2),
                    PG_GETARG_BOOL(3),
                    PG_GETARG_BOOL(4),
                    PG_GETARG_BOOL(5),
                    PG_GETARG_INT64(6),
                    PG_GETARG_BOOL(7),
                    &result_tuples,
                    &result_count);

            /**********************************************************************/
        } else /* (PG_NARGS() == 6) */ {
            /**********************************************************************/
            // pgr_dijkstra(
            // edge_sql TEXT,
            // combinations_sql TEXT,
            // directed BOOLEAN default true,
            // only_cost BOOLEAN default false

            process_combinations(
                    text_to_cstring(PG_GETARG_TEXT_P(0)),
                    text_to_cstring(PG_GETARG_TEXT_P(1)),
                    PG_GETARG_BOOL(2),
                    PG_GETARG_BOOL(3),
                    PG_GETARG_INT64(4),
                    PG_GETARG_BOOL(5),
                    &result_tuples,
                    &result_count);

            /**********************************************************************/
        }

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
        size_t       call_cntr = funcctx->call_cntr;

        /**********************************************************************/
        // OUT seq INTEGER,
        // OUT path_seq INTEGER,
        // OUT start_vid BIGINT,
        // OUT end_vid BIGINT,
        // OUT node BIGINT,
        // OUT edge BIGINT,
        // OUT cost FLOAT,
        // OUT agg_cost FLOAT)

        size_t numb = 8;
        values = palloc(numb * sizeof(Datum));
        nulls = palloc(numb * sizeof(bool));

        size_t i;
        for (i = 0; i < numb; ++i) {
            nulls[i] = false;
        }

        values[0] = Int32GetDatum(call_cntr + 1);
        values[1] = Int32GetDatum(result_tuples[call_cntr].seq);
        values[2] = Int64GetDatum(result_tuples[call_cntr].start_id);
        values[3] = Int64GetDatum(result_tuples[call_cntr].end_id);
        values[4] = Int64GetDatum(result_tuples[call_cntr].node);
        values[5] = Int64GetDatum(result_tuples[call_cntr].edge);
        values[6] = Float8GetDatum(result_tuples[call_cntr].cost);
        values[7] = Float8GetDatum(result_tuples[call_cntr].agg_cost);
        /**********************************************************************/

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        SRF_RETURN_DONE(funcctx);
    }
}

