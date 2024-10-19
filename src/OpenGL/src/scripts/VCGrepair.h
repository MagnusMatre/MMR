#pragma once

#include "VCGclasses.h" 

//class MyEdge;
//class MyFace;
//class MyVertex;
//struct MyUsedTypes : public vcg::UsedTypes<	vcg::Use<MyVertex>   ::AsVertexType,
//	vcg::Use<MyEdge>     ::AsEdgeType,
//	vcg::Use<MyFace>     ::AsFaceType> {};
//
//class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::VFAdj, vcg::vertex::Qualityf, vcg::vertex::BitFlags, vcg::vertex::Mark> {};
//class MyFace : public vcg::Face< MyUsedTypes, vcg::face::Mark, vcg::face::VertexRef, vcg::face::VFAdj, vcg::face::FFAdj, vcg::face::Normal3f, vcg::face::BitFlags > {};
//class MyEdge : public vcg::Edge<MyUsedTypes> {};
//class MyMesh : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace>, std::vector<MyEdge>  > {};

class VCGrepair {

public:
	VCGrepair(std::string& input_dir, std::string& output_dir);
	~VCGrepair();

	void Execute();


private:
	std::string m_input_dir;
	std::string m_output_dir;

	void repairMesh(MyMesh& mesh);
	void repair_directory();

	void reorientFaces(MyMesh& mesh);
};