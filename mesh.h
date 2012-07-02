/*  
 * This file is part of assimp2ctm.
 *
 * assimp2ctm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * assimp2ctm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with assimp2ctm.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MESH_H
#define MESH_H

#include "common.h"

class LogStream : public Assimp::LogStream
{
public:
	 LogStream (void);
	 ~LogStream (void);
	 void write (const char *msg);
	 const std::string &get (void);
	 void clear (void);
private:
	 std::string str;
};

class Mesh
{
public:
	 Mesh (void);
	 ~Mesh (void);
	 bool Load (aiMesh *mesh);
	 void Sanitize (void);
	 std::vector<CTMuint> indices;
	 std::vector<CTMfloat> vertices;
	 std::vector<CTMfloat> normals;
	 std::vector<CTMfloat> texcoords;
	 std::vector<CTMfloat> tangents;
	 std::vector<CTMfloat> bitangents;
	 glm::vec3 min;
	 glm::vec3 max;
};

bool LoadMeshes (const char *filename);

#endif /* !defined MESH_H */
