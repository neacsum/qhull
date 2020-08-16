#pragma once
/*!
  \file libqhull.h user-level header file for using qhull.a library

  see qh-qhull.htm, qhull_a.h

  Copyright (c) 1993-2020 The Geometry Center.
  $Id: //main/2019/qhull/src/libqhull/libqhull.h#11 $$Change: 2958 $
  $DateTime: 2020/05/26 16:17:49 $$Author: bbarber $

  NOTE: access to qh_qh is via the 'qh' macro.  This allows
  qh_qh to be either a pointer or a structure.  An example
  of using qh is "qh.DROPdim" which accesses the DROPdim
  field of qh_qh.  Similarly, access to qh_qhstat is via
  the 'qhstat' macro.

  includes function prototypes for libqhull.c, geom.c, global.c, io.c, user.c

  use mem.h for mem.c
  use qset.h for qset.c

  see unix.c for an example of using libqhull.h

  recompile qhull if you change this file
*/


/*=========================== -included files ==============*/

/* user.h first for QHULL_CRTDBG */
#include "user.h"      /* user definable constants (e.g., realT). */

#include "mem.h"   /* Neede  d for qhT in libqhull_r.h.  Here for compatibility */
#include "qset.h"   /* Needed for QHULL_LIB_CHECK */
/* include stat.h after defining bool.  Needed for qhT in libqhull_r.h.  Here for compatibility */

#include <setjmp.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#if defined(__MWERKS__) && defined(__POWERPC__)
#include  <SIOUX.h>
#include  <Files.h>
#include        <Desk.h>
#endif

#ifndef __STDC__
#ifndef __cplusplus
#if     !defined(_MSC_VER)
#error  Neither __STDC__ nor __cplusplus is defined.  Please use strict ANSI C or C++ to compile
#error  Qhull.  You may need to turn off compiler extensions in your project configuration.  If
#error  your compiler is a standard C compiler, you can delete this warning from libqhull.h
#endif
#endif
#endif

/*============ constants and basic types ====================*/

extern const char qh_version[]; /* defined in global.c */
extern const char qh_version2[]; /* defined in global.c */

/*!
  coordinates and coefficients are stored as realT (i.e., double)

  notes:
    Qhull works well if realT is 'float'.  If so joggle (QJ) is not effective.

    Could use 'float' for data and 'double' for calculations (realT vs. coordT)
    This requires many type casts, and adjusted error bounds.
    Also C compilers may do expressions in double anyway.
*/
typedef realT coordT;

/*!
  a point is an array of coordinates, usually qh.hull_dim
*/
typedef coordT pointT;

/*!
  qh_pointid returns
    - qh_IDnone if point==0 or qh is undefined
    - qh_IDinterior for qh.interior_point
    - qh_IDunknown if point is neither in qh.first_point... nor qh.other_points

  notes:
    qh.STOPcone and qh.STOPpoint assume that qh_IDunknown==-1 (other negative numbers indicate points)
    qh_IDunknown is also returned by getid_() for unknown facet, ridge, or vertex
*/
typedef enum
{
    qh_IDnone= -3, qh_IDinterior= -2, qh_IDunknown= -1
}
qh_pointT;

/*!
    Boolean flag as a bit
*/
typedef unsigned int flagT;


#include "stat.h"  /* needs bool */

/*-<a                             href="qh-qhull.htm#TOC"
  >--------------------------------</a><a name="CENTERtype">-</a>

  qh_CENTER
    to distinguish facet->center
*/
typedef enum
{
    qh_ASnone= 0,    /* If not MERGING and not VORONOI */
    qh_ASvoronoi,    /* Set by qh_clearcenters on qh_prepare_output, or if not MERGING and VORONOI */
    qh_AScentrum     /* If MERGING (assumed during merging) */
}
qh_CENTER;

/*-<a                             href="qh-qhull.htm#TOC"
  >--------------------------------</a><a name="qh_PRINT">-</a>

  qh_PRINT
    output formats for printing (qh.PRINTout).
    'Fa' 'FV' 'Fc' 'FC'


   notes:
   some of these names are similar to qhT names.  The similar names are only
   used in switch statements in qh_printbegin() etc.
*/
typedef enum {qh_PRINTnone= 0,
  qh_PRINTarea, qh_PRINTaverage,           /* 'Fa' 'FV' 'Fc' 'FC' */
  qh_PRINTcoplanars, qh_PRINTcentrums,
  qh_PRINTfacets, qh_PRINTfacets_xridge,   /* 'f' 'FF' 'G' 'FI' 'Fi' 'Fn' */
  qh_PRINTgeom, qh_PRINTids, qh_PRINTinner, qh_PRINTneighbors,
  qh_PRINTnormals, qh_PRINTouter, qh_PRINTmaple, /* 'n' 'Fo' 'i' 'm' 'Fm' 'FM', 'o' */
  qh_PRINTincidences, qh_PRINTmathematica, qh_PRINTmerges, qh_PRINToff,
  qh_PRINToptions, qh_PRINTpointintersect, /* 'FO' 'Fp' 'FP' 'p' 'FQ' 'FS' */
  qh_PRINTpointnearest, qh_PRINTpoints, qh_PRINTqhull, qh_PRINTsize,
  qh_PRINTsummary, qh_PRINTtriangles,      /* 'Fs' 'Ft' 'Fv' 'FN' 'Fx' */
  qh_PRINTvertices, qh_PRINTvneighbors, qh_PRINTextremes,
  qh_PRINTEND} qh_PRINT;

/*-<a                             href="qh-qhull.htm#TOC"
  >--------------------------------</a><a name="qh_ALL">-</a>

  qh_ALL
    argument flag for selecting everything
*/
#define qh_ALL      true
#define qh_NOupper  true      /* argument for qh_findbest */
#define qh_IScheckmax  true   /* argument for qh_findbesthorizon */
#define qh_ISnewfacets  true  /* argument for qh_findbest */
#define qh_RESETvisible  true /* argument for qh_resetlists */

/*-<a                             href="qh-qhull.htm#TOC"
  >--------------------------------</a><a name="qh_ERR">-</a>

  qh_ERR...
    Qhull exit status codes, for indicating errors
    See: MSG_ERROR (6000) and MSG_WARNING (7000) [user.h]
*/
#define qh_ERRnone  0    /* no error occurred during qhull */
#define qh_ERRinput 1    /* input inconsistency */
#define qh_ERRsingular 2 /* singular input data, calls qh_printhelp_singular */
#define qh_ERRprec  3    /* precision error, calls qh_printhelp_degenerate */
#define qh_ERRmem   4    /* insufficient memory, matches mem.h */
#define qh_ERRqhull 5    /* internal error detected, matches mem.h, calls qh_printhelp_internal */
#define qh_ERRother 6    /* other error detected */
#define qh_ERRtopology 7 /* topology error, maybe due to nearly adjacent vertices, calls qh_printhelp_topology */
#define qh_ERRwide 8     /* wide facet error, maybe due to nearly adjacent vertices, calls qh_printhelp_wide */
#define qh_ERRdebug 9    /* qh_errexit from debugging code */

/*-<a                             href="qh-qhull.htm#TOC"
>--------------------------------</a><a name="qh_FILEstderr">-</a>

qh_FILEstderr
Fake stderr to distinguish error output from normal output
For C++ interface.  Must redefine qh_fprintf_qhull
*/
#define qh_FILEstderr ((FILE *)1)

/* ============ -structures- ====================
   each of the following structures is defined by a typedef
   all realT and coordT fields occur at the beginning of a structure
        (otherwise space may be wasted due to alignment)
   define all flags together and pack into 32-bit number

   DEFsetT is likewise defined in mem.h and qset.h
*/

typedef struct vertexT vertexT;
typedef struct ridgeT ridgeT;
typedef struct facetT facetT;
typedef struct qhT qhT;
#ifndef DEFsetT
#define DEFsetT 1
typedef struct setT setT;        /* defined in qset.h */
#endif

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="facetT">-</a>

  facetT
    defines a facet

  notes:
   qhull() generates the hull as a list of facets.

  topological information:
    f.previous,next     doubly-linked list of facets, next is always defined
    f.vertices          set of vertices
    f.ridges            set of ridges
    f.neighbors         set of neighbors
    f.toporient         true if facet has top-orientation (else bottom)

  geometric information:
    f.offset,normal     hyperplane equation
    f.maxoutside        offset to outer plane -- all points inside
    f.center            centrum for testing convexity or Voronoi center for output
    f.simplicial        true if facet is simplicial
    f.flipped           true if facet does not include qh.interior_point

  for constructing hull:
    f.visible           true if facet on list of visible facets (will be deleted)
    f.newfacet          true if facet on list of newly created facets
    f.coplanarset       set of points coplanar with this facet
                        (includes near-inside points for later testing)
    f.outsideset        set of points outside of this facet
    f.furthestdist      distance to furthest point of outside set
    f.visitid           marks visited facets during a loop
    f.replace           replacement facet for to-be-deleted, visible facets
    f.samecycle,newcycle cycle of facets for merging into horizon facet

  see below for other flags and fields
*/
struct facetT {
#if !qh_COMPUTEfurthest
  coordT   furthestdist;/* distance to furthest point of outsideset */
#endif
#if qh_MAXoutside
  coordT   maxoutside;  /* max computed distance of point to facet
                        Before QHULLfinished this is an approximation
                        since maxdist not always set for qh_mergefacet
                        Actual outer plane is +DISTround and
                        computed outer plane is +2*DISTround.
                        Initial maxoutside is qh.DISTround, otherwise distance tests need to account for DISTround */
#endif
  coordT   offset;      /* exact offset of hyperplane from origin */
  coordT  *normal;      /* normal of hyperplane, hull_dim coefficients */
                        /*   if f.tricoplanar, shared with a neighbor */
  union {               /* in order of testing */
   realT   area;        /* area of facet, only in io.c if  f.isarea */
   facetT *replace;     /* replacement facet for qh.NEWfacets with f.visible
                             NULL if qh_mergedegen_redundant, interior, or !NEWfacets */
   facetT *samecycle;   /* cycle of facets from the same visible/horizon intersection,
                             if ->newfacet */
   facetT *newcycle;    /*  in horizon facet, current samecycle of new facets */
   facetT *trivisible;  /* visible facet for ->tricoplanar facets during qh_triangulate() */
   facetT *triowner;    /* owner facet for ->tricoplanar, !isarea facets w/ ->keepcentrum */
  }f;
  coordT  *center;      /* set according to qh.CENTERtype */
                        /*   qh_ASnone:    no center (not MERGING) */
                        /*   qh_AScentrum: centrum for testing convexity (qh_getcentrum) */
                        /*                 assumed qh_AScentrum while merging */
                        /*   qh_ASvoronoi: Voronoi center (qh_facetcenter) */
                        /* after constructing the hull, it may be changed (qh_clearcenter) */
                        /* if tricoplanar and !keepcentrum, shared with a neighbor */
  facetT  *previous;    /* previous facet in the facet_list or NULL, for C++ interface */
  facetT  *next;        /* next facet in the facet_list or facet_tail */
  setT    *vertices;    /* vertices for this facet, inverse sorted by ID
                           if simplicial, 1st vertex was apex/furthest
                           qh_reduce_vertices removes extraneous vertices via qh_remove_extravertices
                           if f.visible, vertices may be on qh.del_vertices */
  setT    *ridges;      /* explicit ridges for nonsimplicial facets or nonsimplicial neighbors.
                           For simplicial facets, neighbors define the ridges
                           qh_makeridges() converts simplicial facets by creating ridges prior to merging
                           If qh.NEWtentative, new facets have horizon ridge, but not vice versa
                           if f.visible && qh.NEWfacets, ridges is empty */
  setT    *neighbors;   /* neighbors of the facet.  Neighbors may be f.visible
                           If simplicial, the kth neighbor is opposite the kth vertex and the
                           first neighbor is the horizon facet for the first vertex.
                           dupridges marked by qh_DUPLICATEridge (0x01) and qh_MERGEridge (0x02)
                           if f.visible && qh.NEWfacets, neighbors is empty */
  setT    *outsideset;  /* set of points outside this facet
                           if non-empty, last point is furthest
                           if NARROWhull, includes coplanars (less than qh.MINoutside) for partitioning*/
  setT    *coplanarset; /* set of points coplanar with this facet
                           >= qh.min_vertex and <= facet->max_outside
                           a point is assigned to the furthest facet
                           if non-empty, last point is furthest away */
  unsigned int visitid; /* visit_id, for visiting all neighbors,
                           all uses are independent */
  unsigned int id;      /* unique identifier from qh.facet_id, 1..qh.facet_id, 0 is sentinel, printed as 'f%d' */
  unsigned int nummerge:9; /* number of merges */
#define qh_MAXnummerge 511 /* 2^9-1 */
                        /* 23 flags (at most 23 due to nummerge), printed by "flags:" in io.c */
  flagT    tricoplanar:1; /* true if TRIangulate and simplicial and coplanar with a neighbor */
                          /*   all tricoplanars share the same apex */
                          /*   all tricoplanars share the same ->center, ->normal, ->offset, ->maxoutside */
                          /*     ->keepcentrum is true for the owner.  It has the ->coplanareset */
                          /*   if ->degenerate, does not span facet (one logical ridge) */
                          /*   during qh_triangulate, f.trivisible points to original facet */
  flagT    newfacet:1;  /* true if facet on qh.newfacet_list (new/qh.first_newfacet or merged) */
  flagT    visible:1;   /* true if visible facet (will be deleted) */
  flagT    toporient:1; /* true if created with top orientation
                           after merging, use ridge orientation */
  flagT    simplicial:1;/* true if simplicial facet, ->ridges may be implicit */
  flagT    seen:1;      /* used to perform operations only once, like visitid */
  flagT    seen2:1;     /* used to perform operations only once, like visitid */
  flagT    flipped:1;   /* true if facet is flipped */
  flagT    upperdelaunay:1; /* true if facet is upper envelope of Delaunay triangulation */
  flagT    notfurthest:1; /* true if last point of outsideset is not furthest */

/*-------- flags primarily for output ---------*/
  flagT    good:1;      /* true if a facet marked good for output */
  flagT    isarea:1;    /* true if facet->f.area is defined */

/*-------- flags for merging ------------------*/
  flagT    dupridge:1;  /* true if facet has one or more dupridge in a new facet (qh_matchneighbor),
                             a dupridge has a subridge shared by more than one new facet */
  flagT    mergeridge:1; /* true if facet or neighbor has a qh_MERGEridge (qh_mark_dupridges)
                            ->normal defined for mergeridge and mergeridge2 */
  flagT    mergeridge2:1; /* true if neighbor has a qh_MERGEridge (qh_mark_dupridges) */
  flagT    coplanarhorizon:1;  /* true if horizon facet is coplanar at last use */
  flagT     mergehorizon:1; /* true if will merge into horizon (its first neighbor w/ f.coplanarhorizon). */
  flagT     cycledone:1;/* true if mergecycle_all already done */
  flagT    tested:1;    /* true if facet convexity has been tested (false after merge */
  flagT    keepcentrum:1; /* true if keep old centrum after a merge, or marks owner for ->tricoplanar
                             Set by qh_updatetested if more than qh_MAXnewcentrum extra vertices
                             Set by qh_mergefacet if |maxdist| > qh.WIDEfacet */
  flagT    newmerge:1;  /* true if facet is newly merged for reducevertices */
  flagT    degenerate:1; /* true if facet is degenerate (degen_mergeset or ->tricoplanar) */
  flagT    redundant:1;  /* true if facet is redundant (degen_mergeset)
                         Maybe merge degenerate and redundant to gain another flag */
};


/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="ridgeT">-</a>

  ridgeT
    defines a ridge

  notes:
  a ridge is hull_dim-1 simplex between two neighboring facets.  If the
  facets are non-simplicial, there may be more than one ridge between
  two facets.  E.G. a 4-d hypercube has two triangles between each pair
  of neighboring facets.

  topological information:
    vertices            a set of vertices
    top,bottom          neighboring facets with orientation

  geometric information:
    tested              true if ridge is clearly convex
    nonconvex           true if ridge is non-convex
*/
struct ridgeT {
  setT    *vertices;    /* vertices belonging to this ridge, inverse sorted by ID
                           NULL if a degen ridge (matchsame) */
  facetT  *top;         /* top facet for this ridge */
  facetT  *bottom;      /* bottom facet for this ridge
                        ridge oriented by odd/even vertex order and top/bottom */
  unsigned int id;      /* unique identifier.  Same size as vertex_id, printed as 'r%d' */
  flagT    seen:1;      /* used to perform operations only once */
  flagT    tested:1;    /* true when ridge is tested for convexity by centrum or opposite vertices */
  flagT    nonconvex:1; /* true if getmergeset detected a non-convex neighbor
                           only one ridge between neighbors may have nonconvex */
  flagT    mergevertex:1; /* true if pending qh_appendvertexmerge due to
                             qh_maybe_duplicateridge or qh_maybe_duplicateridges
                             disables check for duplicate vertices in qh_checkfacet */
  flagT    mergevertex2:1; /* true if qh_drop_mergevertex of MRGvertices, printed but not used */
  flagT    simplicialtop:1; /* true if top was simplicial (original vertices) */
  flagT    simplicialbot:1; /* true if bottom was simplicial (original vertices)
                             use qh_test_centrum_merge if top and bot, need to retest since centrum may change */
};

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="vertexT">-</a>

  vertexT
     defines a vertex

  topological information:
    next,previous       doubly-linked list of all vertices
    neighbors           set of adjacent facets (only if qh.VERTEXneighbors)

  geometric information:
    point               array of DIM3 coordinates
*/
struct vertexT {
  vertexT *next;        /* next vertex in vertex_list or vertex_tail */
  vertexT *previous;    /* previous vertex in vertex_list or NULL, for C++ interface */
  pointT  *point;       /* hull_dim coordinates (coordT) */
  setT    *neighbors;   /* neighboring facets of vertex, qh_vertexneighbors()
                           initialized in io.c or after first merge
                           qh_update_vertices for qh_addpoint or qh_triangulate
                           updated by merges
                           qh_order_vertexneighbors by 2-d (orientation) 3-d (adjacency), n-d (f.visitid,id) */
  unsigned int id;      /* unique identifier, 1..qh.vertex_id,  0 for sentinel, printed as 'r%d' */
  unsigned int visitid; /* for use with qh.vertex_visit, size must match */
  flagT    seen:1;      /* used to perform operations only once */
  flagT    seen2:1;     /* another seen flag */
  flagT    deleted:1;   /* vertex will be deleted via qh.del_vertices */
  flagT    delridge:1;  /* vertex belonged to a deleted ridge, cleared by qh_reducevertices */
  flagT    newfacet:1;  /* true if vertex is in a new facet
                           vertex is on qh.newvertex_list and it has a facet on qh.newfacet_list
                           or vertex is on qh.newvertex_list due to qh_newvertices while merging
                           cleared by qh_resetlists */
  flagT    partitioned:1; /* true if deleted vertex has been partitioned */
};

/*======= -global variables -qh ============================*/

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh">-</a>

  qhT
   All global variables for qhull are in qhT, qhmemT, and qhstatT

  notes:
   qhmem is defined in mem.h, qhstat is defined in stat.h, qhrbox is defined in rboxpoints.h
   Access to qh_qh is via the "qh" macro.  See qh_QHpointer in user.h

   All global variables for qhull are in qh, qhmem, and qhstat
   qh must be unique for each instance of qhull
   qhstat may be shared between qhull instances.
   qhmem may be shared across multiple instances of Qhull.
   Rbox uses global variables rbox_inuse and rbox, but does not persist data across calls.

   Qhull is not multi-threaded.  Global state could be stored in thread-local storage.

   QHULL_LIB_CHECK checks that a program and the corresponding
   qhull library were built with the same type of header files.

   QHULL_LIB_TYPE is QHULL_NON_REENTRANT, QHULL_QH_POINTER, or QHULL_REENTRANT
*/

#define QHULL_NON_REENTRANT 0
#define QHULL_QH_POINTER 1
#define QHULL_REENTRANT 2

#ifdef qh_QHpointer_dllimport
#define qh qh_qh->
__declspec(dllimport) extern qhT *qh_qh;     /* allocated in global.c */
#define QHULL_LIB_TYPE QHULL_QH_POINTER

#elif qh_QHpointer
#define qh qh_qh->
extern qhT *qh_qh;     /* allocated in global.c */
#define QHULL_LIB_TYPE QHULL_QH_POINTER

#elif defined(qh_dllimport)
#define qh qh_qh.
__declspec(dllimport) extern qhT qh_qh;      /* allocated in global.c */
#define QHULL_LIB_TYPE QHULL_NON_REENTRANT

#else
#define qh qh_qh.
extern qhT qh_qh;
#define QHULL_LIB_TYPE QHULL_NON_REENTRANT
#endif

#define QHULL_LIB_CHECK qh_lib_check(QHULL_LIB_TYPE, sizeof(qhT), sizeof(vertexT), sizeof(ridgeT), sizeof(facetT), sizeof(setT), sizeof(qhmemT));
#define QHULL_LIB_CHECK_RBOX qh_lib_check(QHULL_LIB_TYPE, sizeof(qhT), sizeof(vertexT), sizeof(ridgeT), sizeof(facetT), 0, 0);

struct qhT {

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh-const">-</a>

  qh constants
    configuration flags and constants for Qhull

  notes:
    The user configures Qhull by defining flags.  They are
    copied into qh by qh_setflags().  qh-quick.htm#options defines the flags.
*/
  bool ALLpoints;        /* true 'Qs' if search all points for initial simplex */
  bool ALLOWshort;       /* true 'Qa' allow input with fewer or more points than coordinates */
  bool ALLOWwarning;     /* true 'Qw' if allow option warnings */
  bool ALLOWwide;        /* true 'Q12' if allow wide facets and wide dupridges, c.f. qh_WIDEmaxoutside */
  bool ANGLEmerge;       /* true 'Q1' if sort potential merges by type/angle instead of type/distance  */
  bool APPROXhull;       /* true 'Wn' if MINoutside set */
  realT MINoutside;       /*   Minimum distance for an outside point ('Wn' or 2*qh.MINvisible) */
  bool ANNOTATEoutput;   /* true 'Ta' if annotate output with message codes */
  bool ATinfinity;       /* true 'Qz' if point num_points-1 is "at-infinity"
                             for improving precision in Delaunay triangulations */
  bool AVOIDold;         /* true 'Q4' if avoid old->new merges */
  bool BESToutside;      /* true 'Qf' if partition points into best outsideset */
  bool CDDinput;         /* true 'Pc' if input uses CDD format (1.0/offset first) */
  bool CDDoutput;        /* true 'PC' if print normals in CDD format (offset first) */
  bool CHECKduplicates;  /* true 'Q15' if qh_maybe_duplicateridges after each qh_mergefacet */
  bool CHECKfrequently;  /* true 'Tc' if checking frequently */
  realT premerge_cos;     /*   'A-n'   cos_max when pre merging */
  realT postmerge_cos;    /*   'An'    cos_max when post merging */
  bool DELAUNAY;         /* true 'd' or 'v' if computing DELAUNAY triangulation */
  bool DOintersections;  /* true 'Gh' if print hyperplane intersections */
  int   DROPdim;          /* drops dim 'GDn' for 4-d -> 3-d output */
  bool FLUSHprint;       /* true 'Tf' if flush after qh_fprintf for segfaults */
  bool FORCEoutput;      /* true 'Po' if forcing output despite degeneracies */
  int   GOODpoint;        /* 'QGn' or 'QG-n' (n+1, n-1), good facet if visible from point n (or not) */
  pointT *GOODpointp;     /*   the actual point */
  bool GOODthreshold;    /* true 'Pd/PD' if qh.lower_threshold/upper_threshold defined
                             set if qh.UPPERdelaunay (qh_initbuild)
                             false if qh.SPLITthreshold */
  int   GOODvertex;       /* 'QVn' or 'QV-n' (n+1, n-1), good facet if vertex for point n (or not) */
  pointT *GOODvertexp;     /*   the actual point */
  bool HALFspace;        /* true 'Hn,n,n' if halfspace intersection */
  bool ISqhullQh;        /* Set by Qhull.cpp on initialization */
  int   IStracing;        /* 'Tn' trace execution, 0=none, 1=least, 4=most, -1=events */
  int   KEEParea;         /* 'PAn' number of largest facets to keep */
  bool KEEPcoplanar;     /* true 'Qc' if keeping nearest facet for coplanar points */
  bool KEEPinside;       /* true 'Qi' if keeping nearest facet for inside points
                              set automatically if 'd Qc' */
  int   KEEPmerge;        /* 'PMn' number of facets to keep with most merges */
  realT KEEPminArea;      /* 'PFn' minimum facet area to keep */
  realT MAXcoplanar;      /* 'Un' max distance below a facet to be coplanar*/
  int   MAXwide;          /* 'QWn' max ratio for wide facet, otherwise error unless Q12-allow-wide */
  bool MERGEexact;       /* true 'Qx' if exact merges (concave, degen, dupridge, flipped)
                             tested by qh_checkzero and qh_test_*_merge */
  bool MERGEindependent; /* true if merging independent sets of coplanar facets. 'Q2' disables */
  bool MERGING;          /* true if exact-, pre- or post-merging, with angle and centrum tests */
  realT   premerge_centrum;  /*   'C-n' centrum_radius when pre merging.  Default is round-off */
  realT   postmerge_centrum; /*   'Cn' centrum_radius when post merging.  Default is round-off */
  bool MERGEpinched;     /* true 'Q14' if merging pinched vertices due to dupridge */
  bool MERGEvertices;    /* true if merging redundant vertices, 'Q3' disables or qh.hull_dim > qh_DIMmergeVertex */
  realT MINvisible;       /* 'Vn' min. distance for a facet to be visible */
  bool NOnarrow;         /* true 'Q10' if no special processing for narrow distributions */
  bool NOnearinside;     /* true 'Q8' if ignore near-inside points when partitioning, qh_check_points may fail */
  bool NOpremerge;       /* true 'Q0' if no defaults for C-0 or Qx */
  bool ONLYgood;         /* true 'Qg' if process points with good visible or horizon facets */
  bool ONLYmax;          /* true 'Qm' if only process points that increase max_outside */
  bool PICKfurthest;     /* true 'Q9' if process furthest of furthest points*/
  bool POSTmerge;        /* true if merging after buildhull ('Cn' or 'An') */
  bool PREmerge;         /* true if merging during buildhull ('C-n' or 'A-n') */
                        /* NOTE: some of these names are similar to qh_PRINT names */
  bool PRINTcentrums;    /* true 'Gc' if printing centrums */
  bool PRINTcoplanar;    /* true 'Gp' if printing coplanar points */
  int   PRINTdim;         /* print dimension for Geomview output */
  bool PRINTdots;        /* true 'Ga' if printing all points as dots */
  bool PRINTgood;        /* true 'Pg' if printing good facets
                             PGood set if 'd', 'PAn', 'PFn', 'PMn', 'QGn', 'QG-n', 'QVn', or 'QV-n' */
  bool PRINTinner;       /* true 'Gi' if printing inner planes */
  bool PRINTneighbors;   /* true 'PG' if printing neighbors of good facets */
  bool PRINTnoplanes;    /* true 'Gn' if printing no planes */
  bool PRINToptions1st;  /* true 'FO' if printing options to stderr */
  bool PRINTouter;       /* true 'Go' if printing outer planes */
  bool PRINTprecision;   /* false 'Pp' if not reporting precision problems */
  qh_PRINT PRINTout[qh_PRINTEND]; /* list of output formats to print */
  bool PRINTridges;      /* true 'Gr' if print ridges */
  bool PRINTspheres;     /* true 'Gv' if print vertices as spheres */
  bool PRINTstatistics;  /* true 'Ts' if printing statistics to stderr */
  bool PRINTsummary;     /* true 's' if printing summary to stderr */
  bool PRINTtransparent; /* true 'Gt' if print transparent outer ridges */
  bool PROJECTdelaunay;  /* true if DELAUNAY, no readpoints() and
                             need projectinput() for Delaunay in qh_init_B */
  int   PROJECTinput;     /* number of projected dimensions 'bn:0Bn:0' */
  bool RANDOMdist;       /* true 'Rn' if randomly change distplane and setfacetplane */
  realT RANDOMfactor;     /*    maximum random perturbation */
  realT RANDOMa;          /*    qh_randomfactor is randr * RANDOMa + RANDOMb */
  realT RANDOMb;
  bool RANDOMoutside;    /* true 'Qr' if select a random outside point */
  int   REPORTfreq;       /* 'TFn' buildtracing reports every n facets */
  int   REPORTfreq2;      /* tracemerging reports every REPORTfreq/2 facets */
  int   RERUN;            /* 'TRn' rerun qhull n times (qh.build_cnt) */
  int   ROTATErandom;     /* 'QRn' n<-1 random seed, n==-1 time is seed, n==0 random rotation by time, n>0 rotate input */
  bool SCALEinput;       /* true 'Qbk' if scaling input */
  bool SCALElast;        /* true 'Qbb' if scale last coord to max prev coord */
  bool SETroundoff;      /* true 'En' if qh.DISTround is predefined */
  bool SKIPcheckmax;     /* true 'Q5' if skip qh_check_maxout, qh_check_points may fail */
  bool SKIPconvex;       /* true 'Q6' if skip convexity testing during pre-merge */
  bool SPLITthresholds;  /* true 'Pd/PD' if upper_/lower_threshold defines a region
                               else qh.GOODthresholds
                               set if qh.DELAUNAY (qh_initbuild)
                               used only for printing (!for qh.ONLYgood) */
  int   STOPadd;          /* 'TAn' 1+n for stop after adding n vertices */
  int   STOPcone;         /* 'TCn' 1+n for stopping after cone for point n */
                          /*       also used by qh_build_withresart for err exit*/
  int   STOPpoint;        /* 'TVn' 'TV-n' 1+n for stopping after/before(-)
                                        adding point n */
  int   TESTpoints;       /* 'QTn' num of test points after qh.num_points.  Test points always coplanar. */
  bool TESTvneighbors;   /*  true 'Qv' if test vertex neighbors at end */
  int   TRACElevel;       /* 'Tn' conditional IStracing level */
  int   TRACElastrun;     /*  qh.TRACElevel applies to last qh.RERUN */
  int   TRACEpoint;       /* 'TPn' start tracing when point n is a vertex, use qh_IDunknown (-1) after qh_buildhull and qh_postmerge */
  realT TRACEdist;        /* 'TWn' start tracing when merge distance too big */
  int   TRACEmerge;       /* 'TMn' start tracing before this merge */
  bool TRIangulate;      /* true 'Qt' if triangulate non-simplicial facets */
  bool TRInormals;       /* true 'Q11' if triangulate duplicates ->normal and ->center (sets Qt) */
  bool UPPERdelaunay;    /* true 'Qu' if computing furthest-site Delaunay */
  bool USEstdout;        /* true 'Tz' if using stdout instead of stderr */
  bool VERIFYoutput;     /* true 'Tv' if verify output at end of qhull */
  bool VIRTUALmemory;    /* true 'Q7' if depth-first processing in buildhull */
  bool VORONOI;          /* true 'v' if computing Voronoi diagram, also sets qh.DELAUNAY */

  /*--------input constants ---------*/
  realT AREAfactor;       /* 1/(hull_dim-1)! for converting det's to area */
  bool DOcheckmax;       /* true if calling qh_check_maxout (!qh.SKIPcheckmax && qh.MERGING) */
  char  *feasible_string;  /* feasible point 'Hn,n,n' for halfspace intersection */
  coordT *feasible_point;  /*    as coordinates, both malloc'd */
  bool GETarea;          /* true 'Fa', 'FA', 'FS', 'PAn', 'PFn' if compute facet area/Voronoi volume in io.c */
  bool KEEPnearinside;   /* true if near-inside points in coplanarset */
  int   hull_dim;         /* dimension of hull, set by initbuffers */
  int   input_dim;        /* dimension of input, set by initbuffers */
  int   num_points;       /* number of input points */
  pointT *first_point;    /* array of input points, see POINTSmalloc */
  bool POINTSmalloc;     /*   true if qh.first_point/num_points allocated */
  pointT *input_points;   /* copy of original qh.first_point for input points for qh_joggleinput */
  bool input_malloc;     /* true if qh.input_points malloc'd */
  char  qhull_command[256];/* command line that invoked this program */
  int   qhull_commandsiz2; /*    size of qhull_command at qh_clear_outputflags */
  char  rbox_command[256]; /* command line that produced the input points */
  char  qhull_options[512];/* descriptive list of options */
  int   qhull_optionlen;  /*    length of last line */
  int   qhull_optionsiz;  /*    size of qhull_options at qh_build_withrestart */
  int   qhull_optionsiz2; /*    size of qhull_options at qh_clear_outputflags */
  int   run_id;           /* non-zero, random identifier for this instance of qhull */
  bool VERTEXneighbors;  /* true if maintaining vertex neighbors */
  bool ZEROcentrum;      /* true if 'C-0' or 'C-0 Qx' and not post-merging or 'A-n'.  Sets ZEROall_ok */
  realT *upper_threshold; /* don't print if facet->normal[k]>=upper_threshold[k]
                             must set either GOODthreshold or SPLITthreshold
                             if qh.DELAUNAY, default is 0.0 for upper envelope (qh_initbuild) */
  realT *lower_threshold; /* don't print if facet->normal[k] <=lower_threshold[k] */
  realT *upper_bound;     /* scale point[k] to new upper bound */
  realT *lower_bound;     /* scale point[k] to new lower bound
                             project if both upper_ and lower_bound == 0 */

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh-prec">-</a>

  qh precision constants
    precision constants for Qhull

  notes:
    qh_detroundoff [geom2.c] computes the maximum roundoff error for distance
    and other computations.  It also sets default values for the
    qh constants above.
*/
  realT ANGLEround;       /* max round off error for angles */
  realT centrum_radius;   /* max centrum radius for convexity ('Cn' + 2*qh.DISTround) */
  realT cos_max;          /* max cosine for convexity (roundoff added) */
  realT DISTround;        /* max round off error for distances, qh.SETroundoff ('En') overrides qh_distround */
  realT MAXabs_coord;     /* max absolute coordinate */
  realT MAXlastcoord;     /* max last coordinate for qh_scalelast */
  realT MAXoutside;       /* max target for qh.max_outside/f.maxoutside, base for qh_RATIO...
                             recomputed at qh_addpoint, unrelated to qh_MAXoutside */
  realT MAXsumcoord;      /* max sum of coordinates */
  realT MAXwidth;         /* max rectilinear width of point coordinates */
  realT MINdenom_1;       /* min. abs. value for 1/x */
  realT MINdenom;         /*    use divzero if denominator < MINdenom */
  realT MINdenom_1_2;     /* min. abs. val for 1/x that allows normalization */
  realT MINdenom_2;       /*    use divzero if denominator < MINdenom_2 */
  realT MINlastcoord;     /* min. last coordinate for qh_scalelast */
  realT *NEARzero;        /* hull_dim array for near zero in gausselim */
  realT NEARinside;       /* keep points for qh_check_maxout if close to facet */
  realT ONEmerge;         /* max distance for merging simplicial facets */
  realT outside_err;      /* application's epsilon for coplanar points
                             qh_check_bestdist() qh_check_points() reports error if point outside */
  realT WIDEfacet;        /* size of wide facet for skipping ridge in
                             area computation and locking centrum */
  bool NARROWhull;       /* set in qh_initialhull if angle < qh_MAXnarrow */

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh-codetern">-</a>

  qh internal constants
    internal constants for Qhull
*/
  char qhull[sizeof("qhull")]; /* "qhull" for checking ownership while debugging */
  jmp_buf errexit;        /* exit label for qh_errexit, defined by setjmp() and NOerrexit */
  char    jmpXtra[40];    /* extra bytes in case jmp_buf is defined wrong by compiler */
  jmp_buf restartexit;    /* restart label for qh_errexit, defined by setjmp() and ALLOWrestart */
  char    jmpXtra2[40];   /* extra bytes in case jmp_buf is defined wrong by compiler*/
  FILE *  fin;            /* pointer to input file, init by qh_initqhull_start2 */
  FILE *  fout;           /* pointer to output file */
  FILE *  ferr;           /* pointer to error file */
  pointT *interior_point; /* center point of the initial simplex*/
  int     normal_size;    /* size in bytes for facet normals and point coords */
  int     center_size;    /* size in bytes for Voronoi centers */
  int     TEMPsize;       /* size for small, temporary sets (in quick mem) */

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh-lists">-</a>

  qh facet and vertex lists
    defines lists of facets, new facets, visible facets, vertices, and
    new vertices.  Includes counts, next ids, and trace ids.
  see:
    qh_resetlists()
*/
  facetT *facet_list;     /* first facet */
  facetT *facet_tail;     /* end of facet_list (dummy facet with id 0 and next==NULL) */
  facetT *facet_next;     /* next facet for buildhull()
                             previous facets do not have outside sets
                             NARROWhull: previous facets may have coplanar outside sets for qh_outcoplanar */
  facetT *newfacet_list;  /* list of new facets to end of facet_list
                             qh_postmerge sets newfacet_list to facet_list */
  facetT *visible_list;   /* list of visible facets preceding newfacet_list,
                             end of visible list if !facet->visible, same as newfacet_list
                             qh_findhorizon sets visible_list at end of facet_list
                             qh_willdelete prepends to visible_list
                             qh_triangulate appends mirror facets to visible_list at end of facet_list
                             qh_postmerge sets visible_list to facet_list
                             qh_deletevisible deletes the visible facets */
  int       num_visible;  /* current number of visible facets */
  unsigned int tracefacet_id; /* set at init, then can print whenever */
  facetT  *tracefacet;    /*   set in newfacet/mergefacet, undone in delfacet and qh_errexit */
  unsigned int traceridge_id; /* set at init, then can print whenever */
  ridgeT  *traceridge;    /*   set in newridge, undone in delridge, errexit, errexit2, makenew_nonsimplicial, mergecycle_ridges */
  unsigned int tracevertex_id; /* set at buildtracing, can print whenever */
  vertexT *tracevertex;   /*   set in newvertex, undone in delvertex and qh_errexit */
  vertexT *vertex_list;   /* list of all vertices, to vertex_tail */
  vertexT *vertex_tail;   /*      end of vertex_list (dummy vertex with ID 0, next NULL) */
  vertexT *newvertex_list; /* list of vertices in newfacet_list, to vertex_tail
                             all vertices have 'newfacet' set */
  int   num_facets;       /* number of facets in facet_list
                             includes visible faces (num_visible) */
  int   num_vertices;     /* number of vertices in facet_list */
  int   num_outside;      /* number of points in outsidesets (for tracing and RANDOMoutside)
                               includes coplanar outsideset points for NARROWhull/qh_outcoplanar() */
  int   num_good;         /* number of good facets (after qh_findgood_all or qh_markkeep) */
  unsigned int facet_id;  /* ID of next, new facet from newfacet() */
  unsigned int ridge_id;  /* ID of next, new ridge from newridge() */
  unsigned int vertex_id; /* ID of next, new vertex from newvertex() */
  unsigned int first_newfacet; /* ID of first_newfacet for qh_buildcone, or 0 if none */

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh-var">-</a>

  qh global variables
    defines minimum and maximum distances, next visit ids, several flags,
    and other global variables.
    initialize in qh_initbuild or qh_maxmin if used in qh_buildhull
*/
  unsigned long hulltime; /* ignore time to set up input and randomize */
                          /*   use 'unsigned long' to avoid wrap-around errors */
  bool ALLOWrestart;     /* true if qh_joggle_restart can use qh.restartexit */
  int   build_cnt;        /* number of calls to qh_initbuild */
  qh_CENTER CENTERtype;   /* current type of facet->center, qh_CENTER */
  int   furthest_id;      /* pointid of furthest point, for tracing */
  int   last_errcode;     /* last errcode from qh_fprintf, reset in qh_build_withrestart */
  facetT *GOODclosest;    /* closest facet to GOODthreshold in qh_findgood */
  pointT *coplanar_apex;  /* last apex declared a coplanar point by qh_getpinchedmerges, prevents infinite loop */
  bool hasAreaVolume;    /* true if totarea, totvol was defined by qh_getarea */
  bool hasTriangulation; /* true if triangulation created by qh_triangulate */
  bool isRenameVertex;   /* true during qh_merge_pinchedvertices, disables duplicate ridge vertices in qh_checkfacet */
  realT JOGGLEmax;        /* set 'QJn' if randomly joggle input. 'QJ'/'QJ0.0' sets default (qh_detjoggle) */
  bool maxoutdone;       /* set qh_check_maxout(), cleared by qh_addpoint() */
  realT max_outside;      /* maximum distance from a point to a facet,
                               before roundoff, not simplicial vertices
                               actual outer plane is +DISTround and
                               computed outer plane is +2*DISTround */
  realT max_vertex;       /* maximum distance (>0) from vertex to a facet,
                               before roundoff, due to a merge */
  realT min_vertex;       /* minimum distance (<0) from vertex to a facet,
                               before roundoff, due to a merge
                               if qh.JOGGLEmax, qh_makenewplanes sets it
                               recomputed if qh.DOcheckmax, default -qh.DISTround */
  bool NEWfacets;        /* true while visible facets invalid due to new or merge
                              from qh_makecone/qh_attachnewfacets to qh_resetlists */
  bool NEWtentative;     /* true while new facets are tentative due to !qh.IGNOREpinched or qh.ONLYgood
                              from qh_makecone to qh_attachnewfacets */
  bool findbestnew;      /* true if partitioning calls qh_findbestnew */
  bool findbest_notsharp; /* true if new facets are at least 90 degrees */
  bool NOerrexit;        /* true if qh.errexit is not available, cleared after setjmp.  See qh.ERREXITcalled */
  realT PRINTcradius;     /* radius for printing centrums */
  realT PRINTradius;      /* radius for printing vertex spheres and points */
  bool POSTmerging;      /* true when post merging */
  int   printoutvar;      /* temporary variable for qh_printbegin, etc. */
  int   printoutnum;      /* number of facets printed */
  unsigned int repart_facetid; /* previous facetid to prevent recursive qh_partitioncoplanar+qh_partitionpoint */
  int   retry_addpoint;   /* number of retries of qh_addpoint due to merging pinched vertices */
  bool QHULLfinished;    /* true after qhull() is finished */
  realT totarea;          /* 'FA': total facet area computed by qh_getarea, hasAreaVolume */
  realT totvol;           /* 'FA': total volume computed by qh_getarea, hasAreaVolume */
  unsigned int visit_id;  /* unique ID for searching neighborhoods, */
  unsigned int vertex_visit; /* unique ID for searching vertices, reset with qh_buildtracing */
  bool WAScoplanar;      /* true if qh_partitioncoplanar (qh_check_maxout) */
  bool ZEROall_ok;       /* true if qh_checkzero always succeeds */

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh-set">-</a>

  qh global sets
    defines sets for merging, initial simplex, hashing, extra input points,
    and deleted vertices
*/
  setT *facet_mergeset;   /* temporary set of merges to be done */
  setT *degen_mergeset;   /* temporary set of degenerate and redundant merges */
  setT *vertex_mergeset;  /* temporary set of vertex merges */
  setT *hash_table;       /* hash table for matching ridges in qh_matchfacets
                             size is setsize() */
  setT *other_points;     /* additional points */
  setT *del_vertices;     /* vertices to partition and delete with visible
                             facets.  v.deleted is set for checkfacet */

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh-buf">-</a>

  qh global buffers
    defines buffers for maxtrix operations, input, and error messages
*/
  coordT *gm_matrix;      /* (dim+1)Xdim matrix for geom.c */
  coordT **gm_row;        /* array of gm_matrix rows */
  char* line;             /* malloc'd input line of maxline+1 chars */
  int maxline;
  coordT *half_space;     /* malloc'd input array for halfspace (qh.normal_size+coordT) */
  coordT *temp_malloc;    /* malloc'd input array for points */

/*-<a                             href="qh-globa.htm#TOC"
  >--------------------------------</a><a name="qh-static">-</a>

  qh static variables
    defines static variables for individual functions

  notes:
    do not use 'static' within a function.  Multiple instances of qhull
    may exist.

    do not assume zero initialization, 'QPn' may cause a restart
*/
  bool ERREXITcalled;    /* true during qh_errexit (prevents duplicate calls).  see qh.NOerrexit */
  bool firstcentrum;     /* for qh_printcentrum */
  bool old_randomdist;   /* save RANDOMdist flag during io, tracing, or statistics */
  setT *coplanarfacetset; /* set of coplanar facets for searching qh_findbesthorizon() */
  realT last_low;         /* qh_scalelast parameters for qh_setdelaunay */
  realT last_high;
  realT last_newhigh;
  realT lastcpu;          /* for qh_buildtracing */
  int   lastfacets;       /*   last qh.num_facets */
  int   lastmerges;       /*   last zzval_(Ztotmerge) */ 
  int   lastplanes;       /*   last zzval_(Zsetplane) */ 
  int   lastdist;         /*   last zzval_(Zdistplane) */ 
  unsigned int lastreport; /*  last qh.facet_id */
  int mergereport;        /* for qh_tracemerging */
  qhstatT *old_qhstat;    /* for saving qh_qhstat in save_qhull() and UsingLibQhull.  Free with qh_free() */
  setT *old_tempstack;    /* for saving qhmem.tempstack in save_qhull */
  int   ridgeoutnum;      /* number of ridges for 4OFF output (qh_printbegin,etc) */
};

/*=========== -macros- =========================*/

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="otherfacet_">-</a>

  otherfacet_(ridge, facet)
    return neighboring facet for a ridge in facet
*/
#define otherfacet_(ridge, facet) \
                        (((ridge)->top == (facet)) ? (ridge)->bottom : (ridge)->top)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="getid_">-</a>

  getid_(p)
    return int ID for facet, ridge, or vertex
    return qh_IDunknown(-1) if NULL
    return 0 if facet_tail or vertex_tail
*/
#define getid_(p)       ((p) ? (int)((p)->id) : qh_IDunknown)

/*============== FORALL macros ===================*/

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FORALLfacets">-</a>

  FORALLfacets { ... }
    assign 'facet' to each facet in qh.facet_list

  notes:
    uses 'facetT *facet;'
    assumes last facet is a sentinel

  see:
    FORALLfacet_( facetlist )
*/
#define FORALLfacets for (facet=qh facet_list;facet && facet->next;facet=facet->next)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FORALLpoints">-</a>

  FORALLpoints { ... }
    assign 'point' to each point in qh.first_point, qh.num_points

  notes:

  declare:
    coordT *point, *pointtemp;
*/
#define FORALLpoints FORALLpoint_(qh first_point, qh num_points)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FORALLpoint_">-</a>

  FORALLpoint_(points, num) { ... }
    assign 'point' to each point in points array of num points

  declare:
    coordT *point, *pointtemp;
*/
#define FORALLpoint_(points, num) for (point=(points), \
      pointtemp= (points)+qh hull_dim*(num); point < pointtemp; point += qh hull_dim)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FORALLvertices">-</a>

  FORALLvertices { ... }
    assign 'vertex' to each vertex in qh.vertex_list

  declare:
    vertexT *vertex;

  notes:
    assumes qh.vertex_list terminated by NULL or a sentinel (v.next==NULL)
*/
#define FORALLvertices for (vertex=qh vertex_list;vertex && vertex->next;vertex= vertex->next)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHfacet_">-</a>

  FOREACHfacet_( facets ) { ... }
    assign 'facet' to each facet in facets

  declare:
    facetT *facet, **facetp;

  notes:
    assumes set is not modified

  see:
    <a href="qset.h#FOREACHsetelement_">FOREACHsetelement_</a>
*/
#define FOREACHfacet_(facets)    FOREACHsetelement_(facetT, facets, facet)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHneighbor_">-</a>

  FOREACHneighbor_( facet ) { ... }
    assign 'neighbor' to each neighbor in facet->neighbors

  FOREACHneighbor_( vertex ) { ... }
    assign 'neighbor' to each neighbor in vertex->neighbors

  declare:
    facetT *neighbor, **neighborp;

  notes:
    assumes set is not modified

  see:
    <a href="qset.h#FOREACHsetelement_">FOREACHsetelement_</a>
*/
#define FOREACHneighbor_(facet)  FOREACHsetelement_(facetT, facet->neighbors, neighbor)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHpoint_">-</a>

  FOREACHpoint_( points ) { ... }
    assign 'point' to each point in points set

  declare:
    pointT *point, **pointp;

  notes:
    assumes set is not modified

  see:
    <a href="qset.h#FOREACHsetelement_">FOREACHsetelement_</a>
*/
#define FOREACHpoint_(points)    FOREACHsetelement_(pointT, points, point)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHridge_">-</a>

  FOREACHridge_( ridges ) { ... }
    assign 'ridge' to each ridge in ridges set

  declare:
    ridgeT *ridge, **ridgep;

  notes:
    assumes set is not modified

  see:
    <a href="qset.h#FOREACHsetelement_">FOREACHsetelement_</a>
*/
#define FOREACHridge_(ridges)    FOREACHsetelement_(ridgeT, ridges, ridge)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHvertex_">-</a>

  FOREACHvertex_( vertices ) { ... }
    assign 'vertex' to each vertex in vertices set

  declare:
    vertexT *vertex, **vertexp;

  notes:
    assumes set is not modified

  see:
    <a href="qset.h#FOREACHsetelement_">FOREACHsetelement_</a>
*/
#define FOREACHvertex_(vertices) FOREACHsetelement_(vertexT, vertices,vertex)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHfacet_i_">-</a>

  FOREACHfacet_i_( facets ) { ... }
    assign 'facet' and 'facet_i' for each facet in facets set

  declare:
    facetT *facet;
    int     facet_n, facet_i;

  see:
    <a href="qset.h#FOREACHsetelement_i_">FOREACHsetelement_i_</a>
*/
#define FOREACHfacet_i_(facets)    FOREACHsetelement_i_(facetT, facets, facet)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHneighbor_i_">-</a>

  FOREACHneighbor_i_( facet ) { ... }
    assign 'neighbor' and 'neighbor_i' for each neighbor in facet->neighbors

  declare:
    facetT *neighbor;
    int     neighbor_n, neighbor_i;

  notes:
    see <a href="qset.h#FOREACHsetelement_i_">FOREACHsetelement_i_</a>
    for facet neighbors of vertex, need to define a new macro
*/
#define FOREACHneighbor_i_(facet)  FOREACHsetelement_i_(facetT, facet->neighbors, neighbor)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHpoint_i_">-</a>

  FOREACHpoint_i_( points ) { ... }
    assign 'point' and 'point_i' for each point in points set

  declare:
    pointT *point;
    int     point_n, point_i;

  see:
    <a href="qset.h#FOREACHsetelement_i_">FOREACHsetelement_i_</a>
*/
#define FOREACHpoint_i_(points)    FOREACHsetelement_i_(pointT, points, point)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHridge_i_">-</a>

  FOREACHridge_i_( ridges ) { ... }
    assign 'ridge' and 'ridge_i' for each ridge in ridges set

  declare:
    ridgeT *ridge;
    int     ridge_n, ridge_i;

  see:
    <a href="qset.h#FOREACHsetelement_i_">FOREACHsetelement_i_</a>
*/
#define FOREACHridge_i_(ridges)    FOREACHsetelement_i_(ridgeT, ridges, ridge)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHvertex_i_">-</a>

  FOREACHvertex_i_( vertices ) { ... }
    assign 'vertex' and 'vertex_i' for each vertex in vertices set

  declare:
    vertexT *vertex;
    int     vertex_n, vertex_i;

  see:
    <a href="qset.h#FOREACHsetelement_i_">FOREACHsetelement_i_</a>
*/
#define FOREACHvertex_i_(vertices) FOREACHsetelement_i_(vertexT, vertices, vertex)

/********* -libqhull.c prototypes (duplicated from qhull_a.h) **********************/

void    qh_qhull(void);
bool   qh_addpoint(pointT *furthest, facetT *facet, bool checkdist);
void    qh_errexit2(int exitcode, facetT *facet, facetT *otherfacet);
void    qh_printsummary(FILE *fp);

/********* -user.c prototypes (alphabetical) **********************/

void    qh_errexit(int exitcode, facetT *facet, ridgeT *ridge);
void    qh_errprint(const char* string, facetT *atfacet, facetT *otherfacet, ridgeT *atridge, vertexT *atvertex);
int     qh_new_qhull(int dim, int numpoints, coordT *points, bool ismalloc,
                char *qhull_cmd, FILE *outfile, FILE *errfile);
void    qh_printfacetlist(facetT *facetlist, setT *facets, bool printall);
void    qh_printhelp_degenerate(FILE *fp);
void    qh_printhelp_internal(FILE *fp);
void    qh_printhelp_narrowhull(FILE *fp, realT minangle);
void    qh_printhelp_singular(FILE *fp);
void    qh_printhelp_topology(FILE *fp);
void    qh_printhelp_wide(FILE *fp);
void    qh_user_memsizes(void);

/********* -usermem.c prototypes (alphabetical) **********************/
void    qh_exit(int exitcode);
void    qh_fprintf_stderr(int msgcode, const char *fmt, ... );
void    qh_free(void *mem);
void   *qh_malloc(size_t size);

/********* -userprintf.c and userprintf_rbox.c prototypes **********************/
void    qh_fprintf(FILE *fp, int msgcode, const char *fmt, ... );
void    qh_fprintf_rbox(FILE *fp, int msgcode, const char *fmt, ... );

/***** -geom.c/geom2.c/random.c prototypes (duplicated from geom.h, random.h) ****************/

facetT *qh_findbest(pointT *point, facetT *startfacet,
                     bool bestoutside, bool newfacets, bool noupper,
                     realT *dist, bool *isoutside, int *numpart);
facetT *qh_findbestnew(pointT *point, facetT *startfacet,
                     realT *dist, bool bestoutside, bool *isoutside, int *numpart);
bool   qh_gram_schmidt(int dim, realT **rows);
void    qh_outerinner(facetT *facet, realT *outerplane, realT *innerplane);
void    qh_printsummary(FILE *fp);
void    qh_projectinput(void);
void    qh_randommatrix(realT *buffer, int dim, realT **row);
void    qh_rotateinput(realT **rows);
void    qh_scaleinput(void);
void    qh_setdelaunay(int dim, int count, pointT *points);
coordT  *qh_sethalfspace_all(int dim, int count, coordT *halfspaces, pointT *feasible);

/***** -global.c prototypes (alphabetical) ***********************/

unsigned long qh_clock(void);
void    qh_checkflags(char *command, char *hiddenflags);
void    qh_clear_outputflags(void);
void    qh_freebuffers(void);
void    qh_freeqhull(bool allmem);
void    qh_freeqhull2(bool allmem);
void    qh_init_A(FILE *infile, FILE *outfile, FILE *errfile, int argc, char *argv[]);
void    qh_init_B(coordT *points, int numpoints, int dim, bool ismalloc);
void    qh_init_qhull_command(int argc, char *argv[]);
void    qh_initbuffers(coordT *points, int numpoints, int dim, bool ismalloc);
void    qh_initflags(char *command);
void    qh_initqhull_buffers(void);
void    qh_initqhull_globals(coordT *points, int numpoints, int dim, bool ismalloc);
void    qh_initqhull_mem(void);
void    qh_initqhull_outputflags(void);
void    qh_initqhull_start(FILE *infile, FILE *outfile, FILE *errfile);
void    qh_initqhull_start2(FILE *infile, FILE *outfile, FILE *errfile);
void    qh_initthresholds(char *command);
void    qh_lib_check(int qhullLibraryType, int qhTsize, int vertexTsize, int ridgeTsize, int facetTsize, int setTsize, int qhmemTsize);
void    qh_option(const char *option, int *i, realT *r);
#if qh_QHpointer
void    qh_restore_qhull(qhT **oldqh);
qhT    *qh_save_qhull(void);
#endif

/***** -io.c prototypes (duplicated from io.h) ***********************/

void    qh_dfacet(unsigned int id);
void    qh_dvertex(unsigned int id);
void    qh_printneighborhood(FILE *fp, qh_PRINT format, facetT *facetA, facetT *facetB, bool printall);
void    qh_produce_output(void);
coordT *qh_readpoints(int *numpoints, int *dimension, bool *ismalloc);


/********* -mem.c prototypes (duplicated from mem.h) **********************/

void qh_meminit(FILE *ferr);
void qh_memfreeshort(int *curlong, int *totlong);

/********* -poly.c/poly2.c prototypes (duplicated from poly.h) **********************/

void    qh_check_output(void);
void    qh_check_points(void);
setT   *qh_facetvertices(facetT *facetlist, setT *facets, bool allfacets);
facetT *qh_findbestfacet(pointT *point, bool bestoutside,
           realT *bestdist, bool *isoutside);
vertexT *qh_nearvertex(facetT *facet, pointT *point, realT *bestdistp);
pointT *qh_point(int id);
setT   *qh_pointfacet(void /* qh.facet_list */);
int     qh_pointid(pointT *point);
setT   *qh_pointvertex(void /* qh.facet_list */);
void    qh_setvoronoi_all(void);
void    qh_triangulate(void /* qh.facet_list */);

/********* -rboxlib.c prototypes **********************/
int     qh_rboxpoints(FILE* fout, FILE* ferr, char* rbox_command);
void    qh_errexit_rbox(int exitcode);

/********* -stat.c prototypes (duplicated from stat.h) **********************/

void    qh_collectstatistics(void);
void    qh_printallstatistics(FILE *fp, const char *string);
