//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidKernel/Exception.h"
#include "MantidAPI/CompositeFunction.h"
#include "MantidAPI/ParameterTie.h"
#include "MantidAPI/IConstraint.h"

#include <boost/lexical_cast.hpp>
#include <boost/shared_array.hpp>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace Mantid
{
namespace API
{

using std::size_t;

//DECLARE_FUNCTION(CompositeFunction)

/// Copy contructor
CompositeFunction::CompositeFunction(const CompositeFunction& f)
:m_nActive(f.m_nParams),m_nParams(f.m_nParams),m_iConstraintFunction(0)
{
  m_functions.assign(f.m_functions.begin(),f.m_functions.end());
  m_activeOffsets.assign(f.m_activeOffsets.begin(),f.m_activeOffsets.end());
  m_paramOffsets.assign(f.m_paramOffsets.begin(),f.m_paramOffsets.end());
}

///Assignment operator
CompositeFunction& CompositeFunction::operator=(const CompositeFunction& f)
{
  m_nActive = f.m_nActive;
  m_nParams = f.m_nParams;
  m_functions.assign(f.m_functions.begin(),f.m_functions.end());
  m_activeOffsets.assign(f.m_activeOffsets.begin(),f.m_activeOffsets.end());
  m_paramOffsets.assign(f.m_paramOffsets.begin(),f.m_paramOffsets.end());
  m_iConstraintFunction = f.m_iConstraintFunction;
  return *this;
}

///Destructor
CompositeFunction::~CompositeFunction()
{
  for(size_t i=0;i<nFunctions();i++)
    if (m_functions[i]) delete m_functions[i];
}


/// Function initialization. Declare function parameters in this method.
void CompositeFunction::init()
{
}

/** 
 * Writes itself into a string. Functions derived from CompositeFunction must
 * override this method with something like this:
 *   std::string NewFunction::asString()const
 *   {
 *      ostr << "composite=" << this->name() << ';';
 *      // write NewFunction's own attributes and parameters
 *      ostr << CompositeFunction::asString();
 *      // write NewFunction's own ties and constraints
 *      // ostr << ";constraints=(" << ... <<")";
 *   }
 * @return the string representation of the composite function
 */
std::string CompositeFunction::asString()const
{
  std::ostringstream ostr;
  if (name() != "CompositeFunctionMW")
  {
    ostr << "composite=" <<name() << ";";
  }
  for(size_t i=0;i<nFunctions();i++)
  {
    IFitFunction* fun = getFunction(i);
    bool isComp = dynamic_cast<CompositeFunction*>(fun) != 0;
    if (isComp) ostr << '(';
    ostr << fun->asString();
    if (isComp) ostr << ')';
    if (i < nFunctions() - 1)
    {
      ostr << ';';
    }
  }
  std::string ties;
  for(size_t i=0;i<nParams();i++)
  {
    const ParameterTie* tie = getTie(i);
    if (tie)
    {
      IFitFunction* fun = getFunction(functionIndex(i));
      std::string tmp = tie->asString(fun);
      if (tmp.empty())
      {
        tmp = tie->asString(this);
        if (!tmp.empty())
        {
          if (!ties.empty())
          {
            ties += ",";
          }
          ties += tmp;
        }
      }
    }
  }
  if (!ties.empty())
  {
    ostr << ";ties=(" << ties << ")";
  }
  return ostr.str();
}

/** Sets a new value to the i-th parameter.
 *  @param i :: The parameter index
 *  @param value :: The new value
 *  @param explicitlySet :: A boolean falgging the parameter as explicitly set (by user)
 */
void CompositeFunction::setParameter(size_t i, const double& value, bool explicitlySet)
{
  size_t iFun = functionIndex(i);
  m_functions[ iFun ]->setParameter(i - m_paramOffsets[iFun],value,explicitlySet);
}

/** Sets a new description to the i-th parameter.
 *  @param i :: The parameter index
 *  @param description :: The new description
 */
void CompositeFunction::setParameterDescription(size_t i, const std::string& description)
{
  size_t iFun = functionIndex(i);
  m_functions[ iFun ]->setParameterDescription(i - m_paramOffsets[iFun],description);
}

/** Get the i-th parameter.
 *  @param i :: The parameter index
 *  @return value of the requested parameter
 */
double CompositeFunction::getParameter(size_t i)const
{
  size_t iFun = functionIndex(i);
  return m_functions[ iFun ]->getParameter(i - m_paramOffsets[iFun]);
}

/**
 * Sets a new value to a parameter by name.
 * @param name :: The name of the parameter.
 * @param value :: The new value
 * @param explicitlySet :: A boolean falgging the parameter as explicitly set (by user)
 */
void CompositeFunction::setParameter(const std::string& name, const double& value, bool explicitlySet)
{
  std::string pname;
  size_t index;
  parseName(name,index,pname);
  getFunction(index)->setParameter(pname,value,explicitlySet);
}

/**
 * Sets a new description to a parameter by name.
 * @param name :: The name of the parameter.
 * @param value :: The new description
 */
void CompositeFunction::setParameterDescription(const std::string& name, const std::string& description)
{
  std::string pname;
  size_t index;
  parseName(name,index,pname);
  getFunction(index)->setParameterDescription(pname,description);
}

/**
 * Parameters by name.
 * @param name :: The name of the parameter.
 * @return value of the requested named parameter
 */
double CompositeFunction::getParameter(const std::string& name)const
{
  std::string pname;
  size_t index;
  parseName(name,index,pname);
  return getFunction(index)->getParameter(pname);
}

/// Total number of parameters
size_t CompositeFunction::nParams()const
{
  return m_nParams;
}

/**
 * 
 * @param name :: The name of a parameter
 * @return index of the requested named parameter
 */
size_t CompositeFunction::parameterIndex(const std::string& name)const
{
  std::string pname;
  size_t index;
  parseName(name,index,pname);
  return getFunction(index)->parameterIndex(pname) + m_paramOffsets[index];
}

/// Returns the name of parameter
/// @param i :: The index
/// @return The name of the parameter
std::string CompositeFunction::parameterName(size_t i)const
{
  size_t iFun = functionIndex(i);
  std::ostringstream ostr;
  ostr << 'f' << iFun << '.' << m_functions[ iFun ]->parameterName(i - m_paramOffsets[iFun]);
  return ostr.str();
}

/// Returns the description of parameter
/// @param i :: The index
/// @return The description of the parameter
std::string CompositeFunction::parameterDescription(size_t i)const
{
  size_t iFun = functionIndex(i);
  std::ostringstream ostr;
  ostr << m_functions[ iFun ]->parameterDescription(i - m_paramOffsets[iFun]);
  return ostr.str();
}

/// Number of active (in terms of fitting) parameters
size_t CompositeFunction::nActive()const
{
  return m_nActive;
}

/// Value of i-th active parameter. Override this method to make fitted parameters different from the declared
double CompositeFunction::activeParameter(size_t i)const
{
  size_t iFun = functionIndexActive(i);
  return m_functions[ iFun ]->activeParameter(i - m_activeOffsets[iFun]);
}

/// Set new value of i-th active parameter. Override this method to make fitted parameters different from the declared
void CompositeFunction::setActiveParameter(size_t i, double value)
{
  size_t iFun = functionIndexActive(i);
  m_functions[ iFun ]->setActiveParameter(i - m_activeOffsets[iFun],value);
}

/// Update parameters after a fitting iteration
void CompositeFunction::updateActive(const double* in)
{
  for(size_t iFun = 0; iFun < m_functions.size(); iFun++)
  {
    m_functions[ iFun ]->updateActive(in + m_activeOffsets[ iFun ]);
  }
  applyTies();
}

/// Returns "global" index of active parameter i
size_t CompositeFunction::indexOfActive(size_t i)const
{
  size_t iFun = functionIndexActive(i);
  return m_paramOffsets[ iFun ] + m_functions[ iFun ]->indexOfActive(i - m_activeOffsets[iFun]);
}

/// Returns the name of active parameter i
std::string CompositeFunction::nameOfActive(size_t i)const
{
  size_t iFun = functionIndexActive(i);
  std::ostringstream ostr;
  ostr << 'f' << iFun << '.' << m_functions[ iFun ]->nameOfActive(i - m_activeOffsets[iFun]);
  return ostr.str();
}

/// Returns the description of active parameter i
std::string CompositeFunction::descriptionOfActive(size_t i)const
{
  size_t iFun = functionIndexActive(i);
  std::ostringstream ostr;
  ostr << m_functions[ iFun ]->descriptionOfActive(i - m_activeOffsets[iFun]);
  return ostr.str();
}

/**
 * query to see in the function is active
 * @param i :: The index of a declared parameter
 * @return true if parameter i is active
 */
bool CompositeFunction::isActive(size_t i)const
{
  size_t iFun = functionIndex(i);
  return m_functions[ iFun ]->isActive(i - m_paramOffsets[iFun]);
}

/**
 * @param i :: A declared parameter index to be removed from active
 */
void CompositeFunction::removeActive(size_t i)
{
  if (!isActive(i)) return;
  size_t iFun = functionIndex(i);
  size_t ia = m_activeOffsets[iFun] + m_functions[iFun]->activeIndex(i - m_paramOffsets[iFun]);
  m_IFitFunctionActive.erase(m_IFitFunctionActive.begin()+ia);
  m_functions[ iFun ]->removeActive(i - m_paramOffsets[iFun]);

  m_nActive--;
  for(size_t j=iFun+1;j<nFunctions();j++)
    m_activeOffsets[j] -= 1;
}

/** Makes a parameter active again. It doesn't change the parameter's tie.
 * @param i :: A declared parameter index to be restored to active
 */
void CompositeFunction::restoreActive(size_t i)
{
  size_t iFun = functionIndex(i);
  size_t ia = m_activeOffsets[iFun] + m_functions[iFun]->activeIndex(i - m_paramOffsets[iFun]);

  std::vector<size_t>::iterator itFun = 
    std::find_if(m_IFitFunctionActive.begin(),m_IFitFunctionActive.end(),std::bind2nd(std::greater<size_t>(),i));

  m_IFitFunctionActive.insert(itFun,1,ia);
  m_functions[ iFun ]->restoreActive(i - m_paramOffsets[iFun]);

  m_nActive++;
  for(size_t j=iFun+1;j<nFunctions();j++)
    m_activeOffsets[j] += 1;
}

/**
 * @param i :: The index of a declared parameter
 * @return The index of declared parameter i in the list of active parameters
 *         if the parameter is tied.
 */
size_t CompositeFunction::activeIndex(size_t i)const
{
  size_t iFun = functionIndex(i);
  size_t j = m_functions[iFun]->activeIndex(i - m_paramOffsets[iFun]);
  return m_activeOffsets[iFun] + j;
}

/** Makes sure that the function is consistent. 
 */
void CompositeFunction::checkFunction()
{
  m_nParams = 0;
  m_nActive = 0;
  m_paramOffsets.clear();
  m_activeOffsets.clear();
  m_IFitFunction.clear();
  m_IFitFunctionActive.clear();

  std::vector<IFitFunction*> functions(m_functions.begin(),m_functions.end());
  m_functions.clear();

  for(std::vector<IFitFunction*>::size_type i=0;i<functions.size();i++)
  {
    IFitFunction* f = functions[i];
    CompositeFunction* cf = dynamic_cast<CompositeFunction*>(f);
    if (cf) cf->checkFunction();
    addFunction(f);
  }
}

/** Add a function
 * @param f :: A pointer to the added function
 * @return The function index
 */
size_t CompositeFunction::addFunction(IFitFunction* f)
{
  m_IFitFunction.insert(m_IFitFunction.end(),f->nParams(), m_functions.size());
  m_IFitFunctionActive.insert(m_IFitFunctionActive.end(),f->nActive(),m_functions.size());
  m_functions.push_back(f);
  //?f->init();
  if (m_paramOffsets.size() == 0)
  {
    m_paramOffsets.push_back(0);
    m_activeOffsets.push_back(0);
    m_nParams = f->nParams();
    m_nActive = f->nActive();
  }
  else
  {
    m_paramOffsets.push_back(m_nParams);
    m_activeOffsets.push_back(m_nActive);
    m_nParams += f->nParams();
    m_nActive += f->nActive();
  }
  return m_functions.size() - 1;
}

/** Remove a function
 * @param i :: The index of the function to remove
 * @param del :: The deletion flag. If true the function will be deleted otherwise - simply detached
 */
void CompositeFunction::removeFunction(size_t i, bool del)
{
  if ( i >= nFunctions() )
    throw std::out_of_range("Function index out of range.");

  IFitFunction* fun = getFunction(i);

  size_t dna = fun->nActive();
  size_t dnp = fun->nParams();

  for(size_t j=0;j<nParams();)
  {
    ParameterTie* tie = getTie(j);
    if (tie && tie->findParametersOf(fun))
    {
      removeTie(j);
    }
    else
    {
      j++;
    }
  }

  // Shift down the function indeces for parameters
  for(std::vector<size_t>::iterator it=m_IFitFunction.begin();it!=m_IFitFunction.end();)
  {

    if (*it == i)
    {
      it = m_IFitFunction.erase(it);
    }
    else
    {
      if (*it > i)
      {
        *it -= 1;
      }
      ++it;
    }
  }

  // Shift down the function indeces for active parameters
  for(std::vector<size_t>::iterator it=m_IFitFunctionActive.begin();it!=m_IFitFunctionActive.end();)
  {
    if (*it == i)
    {
      it = m_IFitFunctionActive.erase(it);
    }
    else
    {
      if (*it > i)
      {
        *it -= 1;
      }
      ++it;
    }
  }

  m_nActive -= dna;
  // Shift the active offsets down by the number of i-th function's active params
  for(size_t j=i+1;j<nFunctions();j++)
  {
    m_activeOffsets[j] -= dna;
  }
  m_activeOffsets.erase(m_activeOffsets.begin()+i);

  m_nParams -= dnp;
  // Shift the parameter offsets down by the total number of i-th function's params
  for(size_t j=i+1;j<nFunctions();j++)
  {
    m_paramOffsets[j] -= dnp;
  }
  m_paramOffsets.erase(m_paramOffsets.begin()+i);

  m_functions.erase(m_functions.begin()+i);
  if (del)
  {
    delete fun;
  }
}

/** Replace a function with a new one. The old function is deleted.
 *  The new function must have already its workspace set.
 * @param f_old :: The pointer to the function to replace. If it's not
 *  a member of this composite function nothing happens
 * @param f_new :: A pointer to the new function
 */
void CompositeFunction::replaceFunctionPtr(const IFitFunction* f_old,IFitFunction* f_new)
{
  std::vector<IFitFunction*>::const_iterator it = 
    std::find(m_functions.begin(),m_functions.end(),f_old);
  if (it == m_functions.end()) return;
  std::vector<IFitFunction*>::difference_type iFun = it - m_functions.begin();
  replaceFunction(iFun,f_new);
}

/** Replace a function with a new one. The old function is deleted.
 * @param i :: The index of the function to replace
 * @param f :: A pointer to the new function
 */
void CompositeFunction::replaceFunction(size_t i,IFitFunction* f)
{
  if ( i >= nFunctions() )
    throw std::out_of_range("Function index out of range.");

  IFitFunction* fun = getFunction(i);
  size_t na_old = fun->nActive();
  size_t np_old = fun->nParams();

  size_t na_new = f->nActive();
  size_t np_new = f->nParams();

  // Modify function indeces: The new function may have different number of parameters
  {
    std::vector<size_t>::iterator itFun = std::find(m_IFitFunction.begin(),m_IFitFunction.end(),i);
    if(itFun != m_IFitFunction.end()) // functions must have at least 1 parameter
    {
      if (np_old > np_new)
      {
        m_IFitFunction.erase(itFun,itFun + np_old - np_new);
      }
      else if (np_old < np_new) 
      {
        m_IFitFunction.insert(itFun,np_new - np_old,i);
      }
    }
    else if (np_new > 0) // it could happen if the old function is an empty CompositeFunction
    {
      itFun = std::find_if(m_IFitFunction.begin(),m_IFitFunction.end(),std::bind2nd(std::greater<size_t>(),i));
      m_IFitFunction.insert(itFun,np_new,i);
    }
  }

  // Modify function indeces: The new function may have different number of active parameters
  {
    std::vector<size_t>::iterator itFun = std::find(m_IFitFunctionActive.begin(),m_IFitFunctionActive.end(),i);
    if (itFun != m_IFitFunctionActive.end())
    {
      if (na_old > na_new)
      {
        m_IFitFunctionActive.erase(itFun,itFun + na_old - na_new);
      }
      else if (na_old < na_new) 
      {
        m_IFitFunctionActive.insert(itFun,na_new - na_old,i);
      }
    }
    else if (na_new > 0)
    {
      itFun = std::find_if(m_IFitFunctionActive.begin(),m_IFitFunctionActive.end(),std::bind2nd(std::greater<size_t>(),i));
      m_IFitFunctionActive.insert(itFun,na_new,i);
    }
  }

  size_t dna = na_new - na_old;
  m_nActive += dna;
  // Recalc the active offsets 
  for(size_t j=i+1;j<nFunctions();j++)
  {
    m_activeOffsets[j] += dna;
  }

  size_t dnp = np_new - np_old;
  m_nParams += dnp;
  // Shift the parameter offsets down by the total number of i-th function's params
  for(size_t j=i+1;j<nFunctions();j++)
  {
    m_paramOffsets[j] += dnp;
  }

  m_functions[i] = f;
  delete fun;
}

/**
 * @param i :: The index of the function
 * @return function at the requested index
 */
IFitFunction* CompositeFunction::getFunction(size_t i)const
{
  if ( i >= nFunctions() )
  {
    throw std::out_of_range("Function index out of range.");
  }
  return m_functions[i];
}

/**
 * Get the index of the function to which parameter i belongs
 * @param i :: The parameter index
 * @return function index of the requested parameter
 */
size_t CompositeFunction::functionIndex(size_t i)const
{
  if( i >= nParams() )
  {
    throw std::out_of_range("Function parameter index out of range.");
  }
  return m_IFitFunction[i];
}

/**
 * Get the index of the function to which parameter i belongs
 * @param i :: The active parameter index
 * @return active function index of the requested parameter
 */
size_t CompositeFunction::functionIndexActive(size_t i)const
{
  if( i >= nParams() )
    throw std::out_of_range("Function parameter index out of range.");
  return m_IFitFunctionActive[i];
}

/**
* @param varName :: The variable name which may contain function index ( [f<index.>]name )
* @param index :: Receives function index or throws std::invalid_argument
* @param name :: Receives the parameter name
*/
void CompositeFunction::parseName(const std::string& varName,size_t& index, std::string& name)
{
  size_t i = varName.find('.');
  if (i == std::string::npos)
  {
    throw std::invalid_argument("Parameter " + varName + " not found.");
  }
  else
  {
    if (varName[0] != 'f')
      throw std::invalid_argument("External function parameter name must start with 'f'");

    std::string sindex = varName.substr(1,i - 1);
    index = boost::lexical_cast<int>(sindex);

    if (i == varName.size() - 1)
      throw std::invalid_argument("Name cannot be empty");

    name = varName.substr(i+1);
  }
}

/** Returns the index of parameter i as it declared in its function
 * @param i :: The parameter index
 * @return The local index of the parameter
 */
size_t CompositeFunction::parameterLocalIndex(size_t i)const
{
  size_t iFun = functionIndex(i);
  return i - m_paramOffsets[iFun];
}

/** Returns the name of parameter i as it declared in its function
 * @param i :: The parameter index
 * @return The pure parameter name (without the function identifier f#.)
 */
std::string CompositeFunction::parameterLocalName(size_t i)const
{
  size_t iFun = functionIndex(i);
  return m_functions[ iFun ]->parameterName(i - m_paramOffsets[iFun]);
}

/**
 * Apply the ties.
 */
void CompositeFunction::applyTies()
{
  for(size_t i=0;i<nFunctions();i++)
  {
    getFunction(i)->applyTies();
  }
}

/**
 * Clear the ties. 
 */
void CompositeFunction::clearTies()
{
  for(size_t i=0;i<nFunctions();i++)
  {
    getFunction(i)->clearTies();
  }
}

/** Removes i-th parameter's tie if it is tied or does nothing.
 * @param i :: The index of the tied parameter.
 * @return True if successfull
 */
bool CompositeFunction::removeTie(size_t i)
{
  size_t iFun = functionIndex(i);
  bool res = m_functions[ iFun ]->removeTie(i - m_paramOffsets[iFun]);
  if (res)
  {
    m_nActive++;
  }
  return res;
}

/** Get the tie of i-th parameter
 * @param i :: The parameter index
 * @return A pointer to the tie.
 */
ParameterTie* CompositeFunction::getTie(size_t i)const
{
  size_t iFun = functionIndex(i);
  return m_functions[ iFun ]->getTie(i - m_paramOffsets[iFun]);
}

/**
 * Attaches a tie to this function. The attached tie is owned by the function.
 * @param tie :: A pointer to a new tie
 */
void CompositeFunction::addTie(ParameterTie* tie)
{
  size_t i = getParameterIndex(*tie);
  size_t iFun = functionIndex(i);
  m_functions[iFun]->addTie(tie);
}

/**
 * Declare a new parameter. To used in the implementation'c constructor.
 * @param name :: The parameter name.
 * @param initValue :: The initial value for the parameter
 */
void CompositeFunction::declareParameter(const std::string& name, double initValue, const std::string& description)
{
  (void) name; //Avoid compiler warning
  (void) initValue; //Avoid compiler warning
  (void) description; //Avoid compiler warning
  throw Kernel::Exception::NotImplementedError("CompositeFunction cannot not have its own parameters.");
}

/** Add a constraint
 *  @param ic :: Pointer to a constraint.
 */
void CompositeFunction::addConstraint(IConstraint* ic)
{
  size_t i = getParameterIndex(*ic);
  size_t iFun = functionIndex(i);
  getFunction(iFun)->addConstraint(ic);
}

void CompositeFunction::setParametersToSatisfyConstraints()
{
  for(size_t i=0;i<nFunctions();i++)
  {
    getFunction(i)->setParametersToSatisfyConstraints();
  }
}

/// Get constraint
/// @param i :: the index
/// @return A pointer to the constraint
IConstraint* CompositeFunction::getConstraint(size_t i)const
{
  size_t iFun = functionIndex(i);
  return m_functions[ iFun ]->getConstraint(i - m_paramOffsets[iFun]);
}

/** Remove a constraint
 * @param parName :: The name of a parameter which constarint to remove.
 */
void CompositeFunction::removeConstraint(const std::string& parName)
{
  size_t iPar = parameterIndex(parName);
  size_t iFun = functionIndex(iPar);
  getFunction(iFun)->removeConstraint(parameterLocalName(iPar));
}

/** Checks if a constraint has been explicitly set
 *  @param i :: The parameter index
 *  @return true if the function is explicitly set
 */
bool CompositeFunction::isExplicitlySet(size_t i)const
{
  size_t iFun = functionIndex(i);
  return m_functions[ iFun ]->isExplicitlySet(i - m_paramOffsets[iFun]);
}

/**
 * Returns the index of parameter if the ref points to one of the member function
 * @param ref :: A reference to a parameter
 * @return Parameter index or number of nParams() if parameter not found
 */
size_t CompositeFunction::getParameterIndex(const ParameterReference& ref)const
{
  if (ref.getFunction() == this && ref.getIndex() < nParams())
  {
    return ref.getIndex();
  }
  for(size_t iFun=0;iFun<nFunctions();iFun++)
  {
    IFitFunction* fun = getFunction(iFun);
    size_t iLocalIndex = fun->getParameterIndex(ref);
    if (iLocalIndex < fun->nParams())
    {
      return m_paramOffsets[iFun] + iLocalIndex;
    }
  }
  return nParams();
}

/**
 * @param ref :: The reference
 * @return A function containing parameter pointed to by ref
 */
IFitFunction* CompositeFunction::getContainingFunction(const ParameterReference& ref)const
{
  if (ref.getFunction() == this && ref.getIndex() < nParams())
  {
    return ref.getFunction();
  }
  for(size_t iFun=0;iFun<nFunctions();iFun++)
  {
    IFitFunction* fun = getFunction(iFun)->getContainingFunction(ref);
    if (fun)
    {
      return getFunction(iFun);
    }
  }
  return NULL;
}

/**
 * @param fun :: The searched function
 * @return A function containing the argument function fun
 */
IFitFunction* CompositeFunction::getContainingFunction(const IFitFunction* fun)
{
  if (fun == this)
  {
    return this;
  }
  for(size_t iFun=0;iFun<nFunctions();iFun++)
  {
    IFitFunction* f = getFunction(iFun)->getContainingFunction(fun);
    if (f)
    {
      return getFunction(iFun);
    }
  }
  return NULL;
}

} // namespace API
} // namespace Mantid
