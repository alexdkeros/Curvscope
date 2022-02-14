#include <iostream>
#include <string>
#include <fstream>
#include "meshLoader.h"
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh_io.h"
#include "polyscope/surface_mesh.h"
#include "polyscope/messages.h"

namespace curvscope{


bool endsWith(const std::string& str, const std::string& suffix) {
  return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


std::string meshLoader(std::string filename){

	std::vector<std::array<double, 3>> vertexCoords;
	std::vector<std::vector<size_t>> faceIndices;

	if (endsWith(filename,".obj") || endsWith(filename,".ply")){
		//use Polyscope's convenient method to load OBJ and PLY files
		polyscope::loadPolygonSoup(filename, vertexCoords, faceIndices);
	}else if (endsWith(filename,".off")){
		loadPolygonSoup_OFF(filename, vertexCoords, faceIndices);
	}else{
		std::size_t pos=filename.find_last_of('.');
		std::cerr<<"Unknown file type:"<<filename.substr(pos)<<std::endl;
		polyscope::warning("Unknown file type '"+filename.substr(pos)+"' of input file '"+filename+"'");
		return "";
	}
	std::cout<<"nVertices:"<<vertexCoords.size()<<" nFaces:"<<faceIndices.size()<<std::endl;

	std::string meshName=polyscope::guessNiceNameFromPath(filename);

	//turn coordinates to glm vectors to aid linear algebraic operations later on
	std::vector<glm::vec3> vertexCoordsGLM;
	for (std::array<double,3> v: vertexCoords){
		vertexCoordsGLM.push_back(glm::vec3{v[0], v[1], v[2]});
	}
	auto sMesh=polyscope::registerSurfaceMesh(meshName, vertexCoordsGLM, faceIndices);

	//add the option to show vertex normals
	sMesh->computeGeometryData();
	auto vN=sMesh->vertexNormals;
	sMesh->addVertexVectorQuantity("Vertex Normals", vN);

	return meshName;

}




void loadPolygonSoup_OFF(std::string filename, std::vector<std::array<double,3>>& vertexCoords, std::vector<std::vector<size_t>>& faceIndices){

	vertexCoords.clear();
	faceIndices.clear();

	// Open the file
	std::ifstream in(filename);
	if (!in) throw std::invalid_argument("Could not open mesh file " + filename);

	std::string line;
	//get first line
	size_t linecount=0;

	size_t nVertices=0;
	size_t nFaces=0;

	bool hasHeader=false;

	while (std::getline(in, line)) {
		 std::stringstream ss(line);


		 if (line.at(0)!='#'){


			 if (linecount==0 && line.substr(0,3).compare("OFF")==0){
				 hasHeader=true; //good, it is an OFF file with a header
			 } else if ((hasHeader==true && linecount==1) || (hasHeader==false && linecount==0)){
				 ss>>nVertices>>nFaces;
				 //ignore number of edges
			 }else{

				 if (nVertices>0){
					 double v0,v1,v2;
					 ss>>v0>>v1>>v2;

					 vertexCoords.push_back({{v0,v1,v2}});
					 nVertices-=1;

				 }else	if (nVertices==0 && vertexCoords.size()>0){//all vertices parsed, proceed to faces
					 size_t faceSize;
					 std::vector<size_t> face;
					 ss>>faceSize;


					 for (size_t iF=0;iF<faceSize;iF++){
						 size_t iFace;
						 ss>>iFace;
						 face.push_back(iFace);
					 }


					 faceIndices.push_back(face);
				 }
			 }


			 linecount++;
		 }
	}

}

}
