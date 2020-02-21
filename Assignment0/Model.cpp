#include "Model.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

void Model::loadObj(std::string pathName) {
	minX = 10000;
	maxX = -10000;
	minY = 10000;
	maxY = -10000;
	minZ = 10000;
	maxZ = -10000;

	vertices.clear();
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coods;
	ifstream ifs;
	try {
		ifs.open(pathName);
		string one_line;
		while (getline(ifs, one_line)) {
			stringstream ss(one_line);
			string type;
			ss >> type;
			if (type == "v") {
				glm::vec3 vert_pos;
				ss >> vert_pos[0] >> vert_pos[1] >> vert_pos[2];
				positions.push_back(vert_pos);
			}
			else if (type == "vt") {
				glm::vec2 tex_coord;
				ss >> tex_coord[0] >> tex_coord[1];
				tex_coods.push_back(tex_coord);
			}
			else if (type == "vn") {
				glm::vec3 vert_norm;
				ss >> vert_norm[0] >> vert_norm[1] >> vert_norm[2];
				normals.push_back(vert_norm);
			}
			else if (type == "f") {
				string s_vertex_0, s_vertex_1, s_vertex_2;
				ss >> s_vertex_0 >> s_vertex_1 >> s_vertex_2;
				int pos_idx, tex_idx, norm_idx;
				sscanf(s_vertex_0.c_str(), R"(%d/%d/%d)", &pos_idx, &tex_idx, &norm_idx);
				// We have to use index -1 because the obj index starts at 1
				int offset = 1;
				Vertex vertex_0;
				vertex_0.Position = positions[pos_idx - offset];
				vertex_0.TexCoords = tex_coods[tex_idx - offset];
				vertex_0.Normal = normals[norm_idx - offset];
				sscanf(s_vertex_1.c_str(), R"(%d/%d/%d)", &pos_idx, &tex_idx, &norm_idx);
				Vertex vertex_1;
				vertex_1.Position = positions[pos_idx - offset];
				vertex_1.TexCoords = tex_coods[tex_idx - offset];
				vertex_1.Normal = normals[norm_idx - offset];
				sscanf(s_vertex_2.c_str(), R"(%d/%d/%d)", &pos_idx, &tex_idx, &norm_idx);
				Vertex vertex_2;
				vertex_2.Position = positions[pos_idx - offset];
				vertex_2.TexCoords = tex_coods[tex_idx - offset];
				vertex_2.Normal = normals[norm_idx - offset];

				Vertex vertexList[3] = { vertex_0, vertex_1, vertex_2 };
				// Calculates min and max coordinates of the model
				for (int i = 0; i < 3; i++) {
					if (vertexList[i].Position.x < minX) {
						minX = vertexList[i].Position.x;
					}
					if (vertexList[i].Position.x > maxX) {
						maxX = vertexList[i].Position.x;
					}
					if (vertexList[i].Position.y < minY) {
						minY = vertexList[i].Position.y;
					}
					if (vertexList[i].Position.y > maxY) {
						maxY = vertexList[i].Position.y;
					}
					if (vertexList[i].Position.z < minZ) {
						minZ = vertexList[i].Position.z;
					}
					if (vertexList[i].Position.z > maxZ) {
						maxZ = vertexList[i].Position.z;
					}
				}
				vertices.push_back(vertex_0);
				vertices.push_back(vertex_1);
				vertices.push_back(vertex_2);
			}
		}
	}
	catch (const std::exception&) {
		cout << "Error: Obj file cannot be read\n";
	}
}
