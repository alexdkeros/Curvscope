#include <iostream>

#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh_io.h"
#include "polyscope/surface_mesh.h"

#include "polyscope/file_helpers.h"

#include "args/args.hxx"

#include "fileutils/meshLoader.h"
#include "curvature/meanCurvature.h"

//=========================================================================
// Global mesh structure tracker (by name)
//=========================================================================
std::vector<std::string> availableMeshes;


//=========================================================================
// Callbacks
//=========================================================================
void meshCallback(){

	//mesh input callback
	static char textBuff[128] = "";
	ImGui::InputTextWithHint("Select mesh", "enter filename here", textBuff, IM_ARRAYSIZE(textBuff));
	ImGui::SameLine();

	if (ImGui::Button("Add")) {
		std::string s(textBuff);
		std::string meshName=curvscope::meshLoader(s);
		if (!meshName.empty()){
			availableMeshes.push_back(meshName);
		}
	}


}



void meanCurvatureVoronoiCallback(){
	if (ImGui::Button("Mean Curvature (Voronoi areas)")){
		for (std::string s: availableMeshes){
			auto sMesh=polyscope::getSurfaceMesh(s);
			int nVertices=sMesh->nVertices();


			bool voronoiAreas=true;
			std::vector<glm::vec3> meanCurvVecs(nVertices);
			std::vector<double> meanCurv(nVertices);

			curvscope::meanCurvature(s, meanCurvVecs, meanCurv, voronoiAreas);
			sMesh->addVertexScalarQuantity("Mean curvature (Voronoi areas)", meanCurv);
			sMesh->addVertexVectorQuantity("Mean curvature vectors (Voronoi areas)", meanCurvVecs);
		}
	}
}

void meanCurvatureBarycentricCallback(){
	if (ImGui::Button("Mean Curvature (Barycentric areas)")){
		for (std::string s: availableMeshes){
					auto sMesh=polyscope::getSurfaceMesh(s);
					int nVertices=sMesh->nVertices();


					bool voronoiAreas=false;
					std::vector<glm::vec3> meanCurvVecs(nVertices);
					std::vector<double> meanCurv(nVertices);

					curvscope::meanCurvature(s, meanCurvVecs, meanCurv, voronoiAreas);
					sMesh->addVertexScalarQuantity("Mean curvature (Barycentric areas)", meanCurv);
					sMesh->addVertexVectorQuantity("Mean curvature vectors (Barycentric areas)", meanCurvVecs);
					sMesh->addVertexScalarQuantity("Barycentric areas", sMesh->vertexAreas);
			    	sMesh->addFaceScalarQuantity("Face areas", sMesh->faceAreas);;
				}
	}

}


void callback(){

	meshCallback();

	meanCurvatureVoronoiCallback();

	meanCurvatureBarycentricCallback();

}


//=========================================================================
// main
//=========================================================================
int main(int argc, char** argv){

	// Parse commandline arguments
	args::ArgumentParser parser("Curvscope: vizualizing curvature for Adobe");
	args::PositionalList<std::string> meshFiles(parser, "meshFiles", "Input meshes to vizualize");

	try{
		parser.ParseCLI(argc, argv);
	} catch (const args::Help&){
		std::cout<<parser<<std::endl;
		return 0;
	} catch (const args::ParseError& e){
		std::cerr<<e.what()<<std::endl;
		std::cerr<<parser;
		return 1;
	}

	//Importing (possible) commandline input meshes
	for (std::string s : meshFiles){
		std::cout<<"Importing: "<<s<<std::endl;
		std::string meshName=curvscope::meshLoader(s);
		if (!meshName.empty()){
			availableMeshes.push_back(meshName);
		}
	}




    polyscope::init();

    polyscope::state::userCallback=callback;

    polyscope::show();

    return 0;
}
