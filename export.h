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
#ifndef EXPORT_H
#define EXPORT_H

#include "mesh.h"

enum COMPRESSION_METHOD
{
	COMPRESSION_METHOD_RAW,
	COMPRESSION_METHOD_MG1,
	COMPRESSION_METHOD_MG2
};

bool export_mesh (const char *filename, Mesh *mesh);
void export_set_compression_method (enum COMPRESSION_METHOD method);
void export_set_compression_level (unsigned int lvl);

#endif /* !defined EXPORT_H */
