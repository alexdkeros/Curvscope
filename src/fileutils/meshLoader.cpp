#include <string>
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
	std::cout<<"INSIDE off loader"<<std::endl;
}

}
