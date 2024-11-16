#pragma once

#include <iostream>
#include <vcg/complex/complex.h>

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/selection.h>
#include <vcg/complex/algorithms/update/quality.h>   

#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>

using namespace vcg;
using namespace tri;

class MyEdge;
class MyFace;
class MyVertex;
struct MyUsedTypes : public UsedTypes<	Use<MyVertex>   ::AsVertexType,
    Use<MyEdge>     ::AsEdgeType,
    Use<MyFace>     ::AsFaceType> {};

class MyVertex : public Vertex<MyUsedTypes,
    vertex::Coord3f,
    vertex::Normal3f,
    vertex::VFAdj,
    vertex::Qualityf,
    vertex::BitFlags,
    vertex::Mark>{
public:
    vcg::math::Quadric<double>& Qd() { return q; }
private:
    math::Quadric<double> q;
};


class MyFace : public Face< MyUsedTypes,
    face::Mark,
    face::VertexRef,
    face::FFAdj,
    face::VFAdj,
    face::FEAdj,
    face::EFAdj,
    face::Normal3f,
    face::BitFlags > {};

class MyEdge : public Edge<MyUsedTypes,
    edge::EVAdj,
    edge::BitFlags> {};

class MyMesh : public tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace>, std::vector<MyEdge>  > {};
