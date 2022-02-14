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
		if (s.size()>0){
			std::string meshName=curvscope::meshLoader(s);
			if (!meshName.empty()){
				availableMeshes.push_back(meshName);
			}
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
			std::vector<float> vertexAreasVoronoi;


			curvscope::meanCurvature(s, meanCurvVecs, meanCurv, vertexAreasVoronoi, voronoiAreas);
			sMesh->addVertexScalarQuantity("Mean curvature (Voronoi areas)", meanCurv);
			sMesh->addVertexVectorQuantity("Mean curvature vectors (Voronoi areas)", meanCurvVecs);

			if (voronoiAreas){
				//additional registrations to debug
				sMesh->addVertexScalarQuantity("Voronoi area", vertexAreasVoronoi);
			}
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
					std::vector<float> vertexAreasVoronoi;


					curvscope::meanCurvature(s, meanCurvVecs, meanCurv,vertexAreasVoronoi, voronoiAreas);
					sMesh->addVertexScalarQuantity("Mean curvature (Barycentric areas)", meanCurv);
					sMesh->addVertexVectorQuantity("Mean curvature vectors (Barycentric areas)", meanCurvVecs);
					sMesh->addVertexScalarQuantity("Barycentric areas", sMesh->vertexAreas);
			    	sMesh->addFaceScalarQuantity("Face areas", sMesh->faceAreas);
				}
	}

}


void meanCurvatureFlowCallback(){

	static float factor=0.1;

	ImGui::Text("Mean curvature flow:");
	ImGui::InputFloat("factor", &factor);
	ImGui::SameLine();
	if (ImGui::Button("Flow")){
		for (std::string s: availableMeshes){

			bool voronoiAreas=true;


			auto sMesh=polyscope::getSurfaceMesh(s);
			int nVertices=sMesh->nVertices();

			std::vector<glm::vec3> coords(sMesh->vertices);
			curvscope::meanCurvatureFlow(s, coords, factor, voronoiAreas);

			sMesh->updateVertexPositions(coords);
			sMesh->computeGeometryData();

		}
	}

}



void callback(){

	meshCallback();

	meanCurvatureVoronoiCallback();

	meanCurvatureBarycentricCallback();

	meanCurvatureFlowCallback();

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
