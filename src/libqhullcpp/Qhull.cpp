/****************************************************************************
**
** Copyright (c) 2008-2020 C.B. Barber. All rights reserved.
** $Id: //main/2019/qhull/src/libqhullcpp/Qhull.cpp#12 $$Change: 3009 $
** $DateTime: 2020/07/30 19:25:22 $$Author: bbarber $
**
****************************************************************************/

#//! Qhull -- invoke qhull from C++
#//! Compile libqhull_r and Qhull together due to use of setjmp/longjmp()

#include "Qhull.h"

#include "QhullError.h"
#include "RboxPoints.h"
#include "QhullQh.h"
#include "QhullFacet.h"
#include "QhullFacetList.h"

#include <iostream>

using std::cerr;
using std::string;
using std::vector;
using std::ostream;

#ifdef _MSC_VER  // Microsoft Visual C++ -- warning level 4
#pragma warning( disable : 4611)  // interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning( disable : 4996)  // function was declared deprecated(strcpy, localtime, etc.)
#endif

namespace orgQhull{

#//!\name Global variables

const char s_unsupported_options[]=" Fd TI ";
const char s_not_output_options[]= " Fd TI A C d E H P Qa Qb QbB Qbb Qc Qf Qg Qi Qm QJ Qr QR Qs Qt Qv Qx Qz Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 Q10 Q11 Q15 R TA Tc TC TM TP TR Tv TV TW U v V W ";

#//!\name Constructor, destructor, etc.
Qhull::
Qhull()
: qh_qh(0)
, origin_point()
, run_called(false)
, feasible_point()
{
    allocateQhullQh();
}//Qhull

//! Invokes Qhull on rboxPoints
//! Same as runQhull()
//! For rbox commands, see http://www.qhull.org/html/rbox.htm or html/rbox.htm
//! For qhull commands, see http://www.qhull.org/html/qhull.htm or html/qhull.htm
Qhull::
Qhull(const RboxPoints &rboxPoints, const char *qhullCommand2)
: qh_qh(0)
, origin_point()
, run_called(false)
, feasible_point()
{
    allocateQhullQh();
    runQhull(rboxPoints, qhullCommand2);
}//Qhull rbox

//! Invokes Qhull on a set of input points
//! Same as runQhull()
//! For qhull commands, see http://www.qhull.org/html/qhull.htm or html/qhull.htm
Qhull::
Qhull(const char *inputComment2, int pointDimension, int pointCount, const realT *pointCoordinates, const char *qhullCommand2)
: qh_qh(0)
, origin_point()
, run_called(false)
, feasible_point()
{
    allocateQhullQh();
    runQhull(inputComment2, pointDimension, pointCount, pointCoordinates, qhullCommand2);
}//Qhull points

void Qhull::
allocateQhullQh()
{
  QHULL_LIB_CHECK /* Check for compatible library */

    qh_qh= new QhullQh;
    void *p= qh_qh;
    void *p2= static_cast<qhT *>(qh_qh);
    char *s= static_cast<char *>(p);
    char *s2= static_cast<char *>(p2);
    if(s!=s2){
        throw QhullError(10074, "Qhull error: QhullQh at a different address than base type QhT (%d bytes).  Please report compiler to qhull.org", int(s2-s));
    }
}//allocateQhullQh

Qhull::
~Qhull() throw()
{
    // Except for cerr, does not throw errors
    if(qh_qh->hasQhullMessage()){
        cerr<< "\nQhull messages at ~Qhull()\n"; // QH11005 FIX: where should error and log messages go on ~Qhull?
        cerr<< qh_qh->qhullMessage();
        qh_qh->clearQhullMessage();
    }
    delete qh_qh;
    qh_qh= 0;
}//~Qhull

#//!\name GetSet

void Qhull::
checkIfQhullInitialized()
{
    if(!initialized()){ // qh_initqhull_buffers() not called
        throw QhullError(10023, "Qhull error: checkIfQhullInitialized failed.  Call runQhull() first.");
    }
}//checkIfQhullInitialized

//! Return feasiblePoint for halfspace intersection
//! If called before runQhull(), then it returns the value from setFeasiblePoint.  qh.feasible_string overrides this value if it is defined.
Coordinates Qhull::
feasiblePoint() const
{
    Coordinates result;
    if(qh_qh->feasible_point){
        result.append(qh_qh->hull_dim, qh_qh->feasible_point);
    }else{
        result= feasible_point;
    }
    return result;
}//feasiblePoint

//! Return origin point for qh.input_dim
QhullPoint Qhull::
inputOrigin()
{
    QhullPoint result= origin();
    result.setDimension(qh_qh->input_dim);
    return result;
}//inputOrigin

#//!\name GetValue

double Qhull::
area(){
    checkIfQhullInitialized();
    if(!qh_qh->hasAreaVolume){
        QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
            qh_getarea(qh_qh, qh_qh->facet_list);
        }
        qh_qh->NOerrexit= true;
        qh_qh->maybeThrowQhullMessage(QH_TRY_status);
    }
    return qh_qh->totarea;
}//area

double Qhull::
volume(){
    checkIfQhullInitialized();
    if(!qh_qh->hasAreaVolume){
        QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
            qh_getarea(qh_qh, qh_qh->facet_list);
        }
        qh_qh->NOerrexit= true;
        qh_qh->maybeThrowQhullMessage(QH_TRY_status);
    }
    return qh_qh->totvol;
}//volume

#//!\name Foreach

//! Define QhullVertex::neighborFacets().
//! Automatically called if merging facets or computing the Voronoi diagram.
//! Noop if called multiple times.
void Qhull::
defineVertexNeighborFacets(){
    checkIfQhullInitialized();
    if(!qh_qh->hasAreaVolume){
        QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
            qh_vertexneighbors(qh_qh);
        }
        qh_qh->NOerrexit= true;
        qh_qh->maybeThrowQhullMessage(QH_TRY_status);
    }
}//defineVertexNeighborFacets

QhullFacetList Qhull::
facetList() const
{
    return QhullFacetList(beginFacet(), endFacet());
}//facetList

QhullPoints Qhull::
points() const
{
    return QhullPoints(qh_qh, qh_qh->hull_dim, qh_qh->num_points*qh_qh->hull_dim, qh_qh->first_point);
}//points

QhullPointSet Qhull::
otherPoints() const
{
    return QhullPointSet(qh_qh, qh_qh->other_points);
}//otherPoints

//! Return vertices of the convex hull in qh.vertex_list order
//! Vertices are not ordered by adjacency (see QhullFacet::nextFacet2d)
QhullVertexList Qhull::
vertexList() const
{
    return QhullVertexList(beginVertex(), endVertex());
}//vertexList

#//!\name Methods

void Qhull::
outputQhull()
{
    checkIfQhullInitialized();
    QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
        qh_produce_output2(qh_qh);
    }
    qh_qh->NOerrexit= true;
    qh_qh->maybeThrowQhullMessage(QH_TRY_status);
}//outputQhull

void Qhull::
outputQhull(const char *outputflags)
{
    checkIfQhullInitialized();
    string cmd(" "); // qh_checkflags skips first word
    cmd += outputflags;
    char *command= const_cast<char*>(cmd.c_str());
    QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
        qh_clear_outputflags(qh_qh);
        char *s= qh_qh->qhull_command + strlen(qh_qh->qhull_command) + 1; //space
        strncat(qh_qh->qhull_command, command, sizeof(qh_qh->qhull_command)-strlen(qh_qh->qhull_command)-1);
        qh_checkflags(qh_qh, command, const_cast<char *>(s_not_output_options));
        qh_initflags(qh_qh, s);
        qh_initqhull_outputflags(qh_qh);
        if(qh_qh->KEEPminArea < REALmax/2 || qh_qh->KEEParea || qh_qh->KEEPmerge || qh_qh->GOODvertex
                  || qh_qh->GOODpoint || qh_qh->GOODthreshold || qh_qh->SPLITthresholds){
            facetT *facet;
            qh_qh->ONLYgood= false;
            FORALLfacet_(qh_qh->facet_list){
                facet->good= true;
            }
            qh_prepare_output(qh_qh);
        }
        qh_produce_output2(qh_qh);
        if(qh_qh->VERIFYoutput && !qh_qh->FORCEoutput && !qh_qh->STOPadd && !qh_qh->STOPcone && !qh_qh->STOPpoint){
            qh_check_points(qh_qh);
        }
    }
    qh_qh->NOerrexit= true;
    qh_qh->maybeThrowQhullMessage(QH_TRY_status);
}//outputQhull

//! Prepare Qhull for Voronoi output
//! Calls qh_markvoronoi ('v o Fi Fo') and qh_order_vertexneighbors ('v Fi Fo')
void Qhull::
prepareVoronoi(bool *isLower, int *voronoiVertexCount)
{
  QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
    bool isLowerHull;
    vertexT *vertex;

    setT *vertices= qh_markvoronoi(qh_qh, facetList().first().getFacetT(), NULL, !qh_ALL, &isLowerHull, voronoiVertexCount);
    *isLower= isLowerHull;

    qhT *qh= qh_qh;
    FORALLvertices{
      qh_order_vertexneighbors(qh, vertex);
    }
    qh_settempfree(qh, &vertices);
  }
  qh_qh->NOerrexit= true;
  qh_qh->maybeThrowQhullMessage(QH_TRY_status);
}//prepareVoronoi

//! For qhull commands, see http://www.qhull.org/html/qhull.htm or html/qhull.htm
void Qhull::
runQhull(const RboxPoints &rboxPoints, const char *qhullCommand2)
{
    runQhull(rboxPoints.comment().c_str(), rboxPoints.dimension(), rboxPoints.count(), &*rboxPoints.coordinates(), qhullCommand2);
}//runQhull, RboxPoints

//! pointCoordinates is a array of points, input sites ('d' or 'v'), or halfspaces with offset last ('H')
//! Derived from qh_new_qhull [user.c]
//! For rbox commands, see http://www.qhull.org/html/rbox.htm or html/rbox.htm
//! For qhull commands, see http://www.qhull.org/html/qhull.htm or html/qhull.htm
void Qhull::
runQhull(const char *inputComment2, int pointDimension, int pointCount, const realT *pointCoordinates, const char *qhullCommand2)
{
  /* gcc may issue a "might be clobbered" warning for pointDimension and pointCoordinates [-Wclobbered].
     These parameters are not referenced after a longjmp() and hence not clobbered.
     See http://stackoverflow.com/questions/7721854/what-sense-do-these-clobbered-variable-warnings-make */
    if(run_called){
        throw QhullError(10027, "Qhull error: runQhull called twice.  Only one call allowed.");
    }
    run_called= true;
    string s("qhull ");
    s += qhullCommand2;
    char *command= const_cast<char*>(s.c_str());
    /************* Expansion of QH_TRY_ for debugging
    int QH_TRY_status;
    if(qh_qh->NOerrexit){
        qh_qh->NOerrexit= false;
        QH_TRY_status= setjmp(qh_qh->errexit);
    }else{
        QH_TRY_status= QH_TRY_ERROR;
    }
    if(!QH_TRY_status){
    *************/
    QH_TRY_(qh_qh){ // no object creation -- destructors are skipped on longjmp()
        qh_checkflags(qh_qh, command, const_cast<char *>(s_unsupported_options));
        qh_initflags(qh_qh, command);
        *qh_qh->rbox_command= '\0';
        strncat( qh_qh->rbox_command, inputComment2, sizeof(qh_qh->rbox_command)-1);
        if(qh_qh->DELAUNAY){
            qh_qh->PROJECTdelaunay= true;   // qh_init_B() calls qh_projectinput()
        }
        pointT *newPoints= const_cast<pointT*>(pointCoordinates);
        int newDimension= pointDimension;
        int newIsMalloc= false;
        if(qh_qh->HALFspace){
            --newDimension;
            initializeFeasiblePoint(newDimension);
            newPoints= qh_sethalfspace_all(qh_qh, pointDimension, pointCount, newPoints, qh_qh->feasible_point);
            newIsMalloc= true;
        }
        qh_init_B(qh_qh, newPoints, pointCount, newDimension, newIsMalloc);
        qh_qhull(qh_qh);
        qh_check_output(qh_qh);
        qh_prepare_output(qh_qh);
        if(qh_qh->VERIFYoutput && !qh_qh->FORCEoutput && !qh_qh->STOPadd && !qh_qh->STOPcone && !qh_qh->STOPpoint){
            qh_check_points(qh_qh);
        }
    }
    qh_qh->NOerrexit= true;
    for(int k= qh_qh->hull_dim; k--; ){  // Do not move into QH_TRY block.  It may throw an error
        origin_point << 0.0;
    }
    qh_qh->maybeThrowQhullMessage(QH_TRY_status);
}//runQhull

#//!\name Helpers -- be careful of allocating C++ objects due to setjmp/longjmp() error handling by qh_... routines

//! initialize qh.feasible_point for half-space intersection
//! Sets from qh.feasible_string if available, otherwise from Qhull::feasible_point
//! called only once from runQhull(), otherwise it leaks memory (the same as qh_setFeasible)
void Qhull::
initializeFeasiblePoint(int hulldim)
{
    if(qh_qh->feasible_string){
        qh_setfeasible(qh_qh, hulldim);
    }else{
        if(feasible_point.isEmpty()){
            qh_fprintf(qh_qh, qh_qh->ferr, 6209, "qhull error: missing feasible point for halfspace intersection.  Use option 'Hn,n' or Qhull::setFeasiblePoint before runQhull()\n");
            qh_errexit(qh_qh, qh_ERRmem, NULL, NULL);
        }
        if(feasible_point.size()!=static_cast<size_t>(hulldim)){
            qh_fprintf(qh_qh, qh_qh->ferr, 6210, "qhull error: dimension of feasiblePoint should be %d.  It is %u\n", hulldim, feasible_point.size());
            qh_errexit(qh_qh, qh_ERRmem, NULL, NULL);
        }
        qh_qh->feasible_point= static_cast<coordT*>(qh_malloc(static_cast<size_t>(hulldim) * sizeof(coordT)));
        if(!qh_qh->feasible_point){
            qh_fprintf(qh_qh, qh_qh->ferr, 6042, "qhull error (Qhull.cpp): insufficient memory for feasible point\n");
            qh_errexit(qh_qh, qh_ERRmem, NULL, NULL);
        }
        coordT *t= qh_qh->feasible_point;
        // No qh_... routines after here -- longjmp() ignores destructor
        for(Coordinates::ConstIterator p=feasible_point.begin(); p<feasible_point.end(); p++){
            *t++= *p;
        }
    }
}//initializeFeasiblePoint

}//namespace orgQhull

