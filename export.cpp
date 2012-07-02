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
#include "export.h"
#include "mesh.h"
#include <fstream>

unsigned int compression_level = 1;
enum COMPRESSION_METHOD compression_method = COMPRESSION_METHOD_MG1;

void export_set_compression_level (unsigned int lvl)
{
	compression_level = lvl;
}

void export_set_compression_method (enum COMPRESSION_METHOD m)
{
	compression_method = m;
}

bool export_mesh (const char *filename, Mesh *mesh)
{
	try {

		{
			std::ifstream file (filename, std::ios_base::in);
			if (file.is_open ())
			{
				fl_message_title ("File already exists");
				if (fl_choice ("The file \"%s\" does already exist.\nDo you really "
											 "want to override it?", "Override", "Cancel", NULL,
											 filename))
					 return false;
			}
		}

		CTMexporter exporter;

		exporter.CompressionLevel (compression_level);
		switch (compression_method)
		{
		case COMPRESSION_METHOD_RAW:
			exporter.CompressionMethod (CTM_METHOD_RAW);
			break;
		case COMPRESSION_METHOD_MG2:
			exporter.CompressionMethod (CTM_METHOD_MG2);
			break;
		default:
			exporter.CompressionMethod (CTM_METHOD_MG1);
			break;
		}

		mesh->Sanitize ();

		exporter.DefineMesh (&mesh->vertices[0], mesh->vertices.size () / 3,
												 &mesh->indices[0], mesh->indices.size () / 3,
												 &mesh->normals[0]);

		exporter.AddUVMap (&mesh->texcoords[0], "UV", NULL);
		exporter.AddAttribMap (&mesh->tangents[0], "TANGENTS");
		exporter.AddAttribMap (&mesh->bitangents[0], "BITANGENTS");

		exporter.Save (filename);

		return true;
	} catch (ctm_error &e) {
		fl_message_title ("Cannot export the mesh");
		fl_alert ("Cannot export the mesh: %s\n", e.what ());
		return false;
	}
}
