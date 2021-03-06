#include <chrono>

#include "meanCurvature.h"

#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"

namespace curvscope{

float cotan(double theta){
	return (float)std::cos(theta)/std::sin(theta);
}

void meanCurvature(std::string meshName, std::vector<glm::vec3>& curvVecs, std::vector<double>& meanCurv, std::vector<float>& vertexAreasVoronoi, bool areaVoronoi){


	auto sMesh=polyscope::getSurfaceMesh(meshName);

	//check that geometry data is computed in the mesh
	if (sMesh->vertexAreas.size()!=sMesh->nVertices()){
		sMesh->computeGeometryData();
	}

	//resize input containers
	curvVecs.resize(sMesh->nVertices());
	meanCurv.resize(sMesh->nVertices());

	//get surface data
	std::vector<std::vector<size_t>> faces=sMesh->faces;
	std::vector<glm::vec3> vertices=sMesh->vertices;
	std::vector<double> faceAreas=sMesh->faceAreas;
	std::vector<double> vertexAreasBarycentric=sMesh->vertexAreas;

	if (areaVoronoi){
		//will make use of it only if voronoi areas are chosen
		vertexAreasVoronoi.resize(sMesh->nVertices());
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for (size_t iF=0; iF<sMesh->nFaces();iF++){

		//circulate around vertices of each face
		for (size_t j=0;j<3;j++){
			glm::vec3 pA=vertices[faces[iF][j]];
			glm::vec3 pB=vertices[faces[iF][(j+1)%3]];
			glm::vec3 pC=vertices[faces[iF][(j+2)%3]];

			double cos_theta_pA=glm::dot(glm::normalize(pB-pA), glm::normalize(pC-pA));
			double cos_theta_pB=glm::dot(glm::normalize(pC-pB), glm::normalize(pA-pB));
			double cos_theta_pC=glm::dot(glm::normalize(pA-pC), glm::normalize(pB-pC));
			float theta_pA=std::acos(glm::clamp(-1., 1., cos_theta_pA));
			float theta_pB=std::acos(glm::clamp(-1., 1., cos_theta_pB));
			float theta_pC=std::acos(glm::clamp(-1., 1., cos_theta_pC));

			//for the mean curvature normal
//			curvVecs[faces[iF][j]]+=cotan(theta_pB)*(pA-pC)+cotan(theta_pC)*(pA-pB);
			curvVecs[faces[iF][j]]+=cotan(theta_pB)*(pC-pA)+cotan(theta_pC)*(pB-pA);


			if (areaVoronoi){
				//use A_mixed area formula from Meyer et al. for normalization
				if (theta_pA>90.0 || theta_pB>90.0 || theta_pC>90.0){
					//Obtuse, Voronoi inappropriate
					if (theta_pA>90.0){
						//angle at vertex of interest is obtuse
						vertexAreasVoronoi[faces[iF][j]]+=faceAreas[iF]/2;
					}else{
						vertexAreasVoronoi[faces[iF][j]]+=faceAreas[iF]/4;
					}
				}else{
					//Non-obtuse, Voronoi appropriate
					vertexAreasVoronoi[faces[iF][j]]+=glm::max(0.0,(1.0/8.0)*glm::length2(pA-pB)*cotan(theta_pC));
					vertexAreasVoronoi[faces[iF][j]]+=glm::max(0.0,(1.0/8.0)*glm::length2(pA-pC)*cotan(theta_pB));

				}
			}
		}
	}


	for (size_t iV=0; iV<sMesh->nVertices();iV++){

		if (areaVoronoi){
			//normalization
			curvVecs[iV]=(1/(2*vertexAreasVoronoi[iV]))*curvVecs[iV];
		}else{
			float normalizer=1/(2*vertexAreasBarycentric[iV]);
			curvVecs[iV]=normalizer*curvVecs[iV];
		}

		//compute curvature from mean curvature normal
		meanCurv[iV]=glm::length(curvVecs[iV])/2.0;
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	if (areaVoronoi){
		std::cout << "Mean Curvature (Voronoi areas) computation time: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms" << std::endl;
	}else{
		std::cout << "Mean Curvature (Barycentric areas) computation time: " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms" << std::endl;

	}

}

void meanCurvatureFlow(std::string meshName, std::vector<glm::vec3>& coords, float factor, bool voronoiAreas){

	auto sMesh=polyscope::getSurfaceMesh(meshName);

	std::vector<glm::vec3> meanCurvVecs(sMesh->nVertices());
	std::vector<double> meanCurv(sMesh->nVertices());
	std::vector<float> vertexAreasVoronoi;

	meanCurvature(meshName, meanCurvVecs, meanCurv,vertexAreasVoronoi, voronoiAreas);

	for (size_t iV=0;iV<sMesh->nVertices();iV++){
			coords[iV]+=meanCurvVecs[iV]*static_cast<float>(sMesh->vertexAreas[iV])*factor;
	}
}


}
