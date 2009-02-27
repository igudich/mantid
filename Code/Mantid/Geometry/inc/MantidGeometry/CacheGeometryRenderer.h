#ifndef CACHE_GEOMETRYRENDERER_H
#define CACHE_GEOMETRYRENDERER_H

#include "MantidKernel/System.h"
#include "MantidKernel/Logger.h"
namespace Mantid
{

  namespace Geometry
  {
    /*!
    \class CacheGeometryRenderer
    \brief rendering geometry using opengl from the geometry cache.
    \author Srikanth Nagella
    \date Jan 2009
    \version 1.0

    This is an concrete class for rendering cached geometry using opengl.

    Copyright &copy; 2008 STFC Rutherford Appleton Laboratories

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
    */
	class IObjComponent;
	class V3D;
	class DLLExport CacheGeometryRenderer
    {
    private:

      static Kernel::Logger& PLog;           ///< The official logger
	  unsigned int           iDisplaylistId; ///< OpenGL display list id
	  bool                   boolDisplaylistCreated; ///< flag to store whether display list is created or not

    public:
		CacheGeometryRenderer();       ///< Constructor
		~CacheGeometryRenderer();      ///< Destructor
		void Render(IObjComponent* ObjComp) const;
		void Render(int noPts,int noFaces,double* points,int* faces) const;
		void Initialize(int noPts,int noFaces,double* points,int* faces);
		void Initialize(IObjComponent* ObjComp);
    };

  }   // NAMESPACE Geometry

}  // NAMESPACE Mantid

#endif
