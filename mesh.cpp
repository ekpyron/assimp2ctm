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
#include "mesh.h"
#include <cmath>

LogStream::LogStream (void)
{
}

LogStream::~LogStream (void)
{
}

void LogStream::write (const char *msg)
{
	str.append (msg);
}

void LogStream::clear (void)
{
	str.clear ();
}

const std::string &LogStream::get (void)
{
	return str;
}


Mesh::Mesh (void)
{
}

Mesh::~Mesh (void)
{
}

bool Mesh::Load (aiMesh *mesh)
{
	if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
	{
		fl_message_title ("Cannot load a mesh.");
		fl_alert ("Only triangle meshes are supported.");
		return false;
	}
	if (!mesh->HasFaces ())
	{
		fl_message_title ("Cannot load a mesh.");
		fl_alert ("The mesh does not contain any faces.");
		return false;
	}
	if (!mesh->HasNormals ())
	{
		fl_message_title ("Cannot load a mesh.");
		fl_alert ("The mesh does not contain normals.");
		return false;
	}
	if (!mesh->HasTangentsAndBitangents ())
	{
		fl_message_title ("Cannot load a mesh.");
		fl_alert ("The mesh does not contain tangents and bitangents.");
		return false;
	}
	if (!mesh->HasPositions ())
	{
		fl_message_title ("Cannot load a mesh.");
		fl_alert ("The mesh does not contain vertices.");
		return false;
	}
	if (!mesh->HasTextureCoords (0))
	{
		fl_message_title ("Cannot load a mesh.");
		fl_alert ("The mesh does not contain texture coordinates.");
		return false;
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		if (mesh->mFaces[i].mNumIndices != 3)
		{
			fl_message_title ("Cannot load a mesh.");
			fl_alert ("The mesh contains a face with an invalid number of indices.");
			return false;
		}
		for (int j = 0; j < 3; j++)
		{
			if (mesh->mFaces[i].mIndices[j] >= mesh->mNumVertices)
			{
				fl_message_title ("Cannot load a mesh.");
				fl_alert ("The mesh contains an index that's out of bounds.");
				return false;
			}
			indices.push_back (mesh->mFaces[i].mIndices[j]);
		}
	}

	min.x = max.x = mesh->mVertices[0].x;
	min.y = max.y = mesh->mVertices[0].y;
	min.z = max.z = mesh->mVertices[0].z;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.push_back (mesh->mVertices[i].x);
		vertices.push_back (mesh->mVertices[i].y);
		vertices.push_back (mesh->mVertices[i].z);

		if (mesh->mVertices[i].x < min.x)
			 min.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].y < min.y)
			 min.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].z < min.z)
			 min.z = mesh->mVertices[i].z;

		if (mesh->mVertices[i].x > max.x)
			 max.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].y > max.y)
			 max.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].z > max.z)
			 max.z = mesh->mVertices[i].z;

		normals.push_back (mesh->mNormals[i].x);
		normals.push_back (mesh->mNormals[i].y);
		normals.push_back (mesh->mNormals[i].z);

		texcoords.push_back (mesh->mTextureCoords[0][i].x);
		texcoords.push_back (mesh->mTextureCoords[0][i].y);

		tangents.push_back (mesh->mTangents[i].x);
		tangents.push_back (mesh->mTangents[i].y);
		tangents.push_back (mesh->mTangents[i].z);
		tangents.push_back (1.0f);

		bitangents.push_back (mesh->mBitangents[i].x);
		bitangents.push_back (mesh->mBitangents[i].y);
		bitangents.push_back (mesh->mBitangents[i].z);
		bitangents.push_back (1.0f);
	}
	return true;
}

void Mesh::Sanitize (void)
{
	for (std::vector<CTMfloat>::iterator it = vertices.begin ();
			 it != vertices.end (); it++)
	{
		if (!std::isfinite (*it))
			 *it = 0.0f;
	}
	for (std::vector<CTMfloat>::iterator it = normals.begin ();
			 it != normals.end (); it++)
	{
		if (!std::isfinite (*it))
			 *it = 0.0f;
	}
	for (std::vector<CTMfloat>::iterator it = texcoords.begin ();
			 it != texcoords.end (); it++)
	{
		if (!std::isfinite (*it))
			 *it = 0.0f;
	}
	for (std::vector<CTMfloat>::iterator it = tangents.begin ();
			 it != tangents.end (); it++)
	{
		if (!std::isfinite (*it))
			 *it = 0.0f;
	}
	for (std::vector<CTMfloat>::iterator it = bitangents.begin ();
			 it != bitangents.end (); it++)
	{
		if (!std::isfinite (*it))
			 *it = 0.0f;
	}
}

std::vector<Mesh> meshes;
extern LogStream logstream;

bool LoadMeshes (const char *filename)
{
	logstream.clear ();
	Assimp::Importer importer;

	const aiScene *scene;
	scene = importer.ReadFile (filename, aiProcess_CalcTangentSpace
														 | aiProcess_Triangulate
														 | aiProcess_JoinIdenticalVertices
														 | aiProcess_SortByPType
														 | aiProcess_GenSmoothNormals
														 | aiProcess_GenUVCoords
														 | aiProcess_TransformUVCoords
														 | aiProcess_OptimizeMeshes
														 | aiProcess_OptimizeGraph);

	if (!scene)
	{
		fl_message_title ("Cannot load the model file.");
		fl_alert ("%s", logstream.get ().c_str ());
		return false;
	}

	meshes.clear ();
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		Mesh mesh;
		if (mesh.Load (scene->mMeshes[i]))
		{
			meshes.push_back (mesh);
		}
	}

	return true;
}
