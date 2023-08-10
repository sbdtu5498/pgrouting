/*PGR-GNU*****************************************************************
File: depthFirstSearch.sql

Generated with Template by:
Copyright (c) 2023 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2023 Sanskar Bhushan
Mail: sbdtu5498@gmail.com

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

--------------------------
-- pgr_edgeBetweennessCentrality
--------------------------

--v3.6.0
CREATE FUNCTION pgr_edgeBetweennessCentrality(
    TEXT,     -- edges_sql (required)

    directed BOOLEAN DEFAULT true,

    OUT id BIGINT,
    OUT source BIGINT,
    OUT target BIGINT,
    OUT cost FLOAT,
    OUT absolute_bc FLOAT,
    OUT relative_bc FLOAT)

RETURNS SETOF RECORD AS
$BODY$
BEGIN
    RETURN QUERY
    SELECT *
    FROM _pgr_edgeBetweennessCentrality(_pgr_get_statement($1), directed);
END;
$BODY$
LANGUAGE plpgsql VOLATILE STRICT;

-- COMMENTS

COMMENT ON FUNCTION pgr_edgeBetweennessCentrality(TEXT, BOOLEAN)
IS 'pgr_edgeBetweennessCentrality
- EXPERIMENTAL
- Parameters:
    - Edges SQL with columns: id, source, target, cost [,reverse_cost]
- Optional parameters
    - directed := true
- Documentation:
    - ${PROJECT_DOC_LINK}/pgr_edgeBetweennessCentrality.html
';
