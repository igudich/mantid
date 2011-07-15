#include "MantidDataObjects/WorkspaceSingleValue.h"
#include "MantidAPI/LocatedDataRef.h"
#include "MantidAPI/WorkspaceIterator.h"
#include "MantidAPI/WorkspaceIteratorCode.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/WorkspaceFactory.h"

namespace Mantid
{
  namespace DataObjects
  {

    using std::size_t;

    DECLARE_WORKSPACE(WorkspaceSingleValue)

    // Get a reference to the logger
    Kernel::Logger& WorkspaceSingleValue::g_log = Kernel::Logger::get("WorkspaceSingleValue");

    /// Constructor
    WorkspaceSingleValue::WorkspaceSingleValue(double value,double error) :
        API::MatrixWorkspace()
    {
      // Set the "histogram" to the single value
      data.dataX().resize(1, 0.0);
      data.dataY().resize(1, value);
      data.dataE().resize(1, error);
      data.dataDx().resize(1, 0.0);

      isDistribution(true);
    }

    /// Destructor
    WorkspaceSingleValue::~WorkspaceSingleValue()
    {}

    /** Does nothing in this case
    *  @param NVectors :: This value can only be equal to one, otherwise exception is thrown
    *  @param XLength :: The number of X data points/bin boundaries
    *  @param YLength :: The number of data/error points
    */
    void WorkspaceSingleValue::init(const size_t &NVectors, const size_t &XLength, const size_t &YLength)
    {
      (void) NVectors; (void) XLength; (void) YLength; //Avoid compiler warning
    }

    //--------------------------------------------------------------------------------------------
    /// Return the underlying ISpectrum ptr at the given workspace index.
    Mantid::API::ISpectrum * WorkspaceSingleValue::getSpectrum(const size_t /*index*/)
    {
      return &data;
    }

    /// Return the underlying ISpectrum ptr at the given workspace index.
    const Mantid::API::ISpectrum * WorkspaceSingleValue::getSpectrum(const size_t /*index*/) const
    {
      return &data;
    }
    

  } // namespace DataObjects
} // namespace Mantid

///\cond TEMPLATE
template DLLExport class Mantid::API::workspace_iterator<Mantid::API::LocatedDataRef, Mantid::DataObjects::WorkspaceSingleValue>;
template DLLExport class Mantid::API::workspace_iterator<const Mantid::API::LocatedDataRef, const Mantid::DataObjects::WorkspaceSingleValue>;

template DLLExport class Mantid::API::WorkspaceProperty<Mantid::DataObjects::WorkspaceSingleValue>;

namespace Mantid
{
namespace Kernel
{
    template<> DLLExport
      Mantid::DataObjects::WorkspaceSingleValue_sptr IPropertyManager::getValue<Mantid::DataObjects::WorkspaceSingleValue_sptr>(const std::string &name) const
    {
      PropertyWithValue<Mantid::DataObjects::WorkspaceSingleValue_sptr>* prop =
        dynamic_cast<PropertyWithValue<Mantid::DataObjects::WorkspaceSingleValue_sptr>*>(getPointerToProperty(name));
      if (prop)
      {
        return *prop;
      }
      else
      {
        throw std::runtime_error("Attempt to assign property of incorrect type. Expected WorkspaceSingleValue.");
      }
    }

} // namespace Kernel
} // namespace Mantid

///\endcond TEMPLATE
