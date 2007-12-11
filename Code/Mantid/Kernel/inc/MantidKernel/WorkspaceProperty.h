#ifndef MANTID_KERNEL_WORKSPACEPROPERTY_H_
#define MANTID_KERNEL_WORKSPACEPROPERTY_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidKernel/PropertyWithValue.h"
#include "MantidKernel/IStorable.h"
#include "MantidAPI/AnalysisDataService.h"

namespace Mantid
{
namespace Kernel
{
/** @class WorkspaceProperty WorkspaceProperty.h Kernel/WorkspaceProperty.h

    A property class for workspaces. Inherits from PropertyWithValue, with the value being
    a pointer to the workspace type given to the WorkspaceProperty constructor. This kind
    of property also holds the name of the workspace (as used by the AnalysisDataService)
    and an indication of whether it is an input or output to an algorithm (or both).

    @author Russell Taylor, Tessella Support Services plc
    @date 10/12/2007
    
    Copyright &copy; 2007 STFC Rutherford Appleton Laboratories

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
    
    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
template <typename TYPE>
class WorkspaceProperty : public PropertyWithValue<TYPE*>, public IStorable
{
public:  
  /** Constructor.
   *  Sets the property and workspace names but initialises the workspace pointer to null.
   *  @param name The name to assign to the property
   *  @param wsName The name of the workspace
   *  @param direction Whether this is a Direction::Input, Direction::Output or Direction::InOut (Input & Output) workspace
   *  @throw std::out_of_range if the direction argument is not a member of the Direction enum (i.e. 0-2)
   */
  WorkspaceProperty( const std::string &name, const std::string &wsName, const unsigned int direction ) :
    PropertyWithValue<TYPE*>( name, NULL ),
    m_workspaceName( wsName ),
    m_direction( direction )
  {
    // Make sure a random int hasn't been passed in for the direction
    // Property & PropertyWithValue destructors will be called in this case
    if (m_direction > 2) throw std::out_of_range("direction should be a member of the Direction enum");
  }

  /// Copy constructor
  WorkspaceProperty( const WorkspaceProperty& right ) :
    PropertyWithValue<TYPE*>( right ),
    m_workspaceName( right.m_workspaceName ),
    m_direction( right.m_direction )
  {
  }
    
  /// Copy assignment operator. Only copies the value (i.e. the pointer to the workspace)
  WorkspaceProperty& operator=( const WorkspaceProperty& right )
  {
    if ( &right == this ) return *this;
    PropertyWithValue<TYPE*>::operator=( right );
    return *this;
  }
  
  /// Virtual destructor
  virtual ~WorkspaceProperty()
  {
  }

  /** Get the name of the workspace
   *  @return The workspace's name
   */
  virtual std::string value() const
  {
    return m_workspaceName;
  }

  /// Overrides PropertyWithValue method. Not to be used.
  virtual bool setValue( const std::string& value )
  {
    // Not allowed!
    return false;
  }
  
  /** Checks whether the property is valid
   *  @returns True if the property is valid, otherwise false.
   */
  virtual const bool isValid()
  {
    // Assume that any declared WorkspaceProperty must have a name set (i.e. is not an optional property)
    if ( m_workspaceName.empty() ) return false;
    
    // If an input workspace, check that it exists in the AnalysisDataService & can be cast to correct type
    if ( ( m_direction==0 ) || ( m_direction==2 ) )
    {
      try {
        API::Workspace *ws = API::AnalysisDataService::Instance()->retrieve(m_workspaceName);
        // Check retrieved workspace is the type that it should be and assign to the property value
        TYPE* workspace = PropertyWithValue<TYPE*>::operator=( dynamic_cast<TYPE*>(ws) );
        if ( !workspace ) return false;
      } catch (Exception::NotFoundError e) {
        return false;
      }
    }
    
    return true;
  }

  /// If this is an output workspace, store it into the AnalysisDataService
  void store()
  {
    if ( m_direction )
    {
      API::AnalysisDataService::Instance()->addOrReplace(m_workspaceName, this->operator()() );
    }
  }
  
  /// Reset the pointer to the workspace
  void clear()
  {
    PropertyWithValue<TYPE*>::operator=( NULL );
  }
  
private:
  /// The name of the workspace (as used by the AnalysisDataService)
  const std::string m_workspaceName;
  /// Whether the workspace is used as input, output or both to an algorithm
  const unsigned int m_direction;
};

/// Describes the direction (within an algorithm) of a WorkspaceProperty
struct Direction
{
  /// Enum giving the possible directions
  enum
  { 
    Input,    ///< An input workspace
    Output,   ///< An output workspace
    InOut     ///< Both an input & output workspace
  };
};

} // namespace Kernel
} // namespace Mantid

#endif /*MANTID_KERNEL_WORKSPACEPROPERTY_H_*/
