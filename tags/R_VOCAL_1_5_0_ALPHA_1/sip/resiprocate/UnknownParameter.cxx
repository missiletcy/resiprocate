#include <sipstack/UnknownParameter.hxx>
#include <util/ParseBuffer.hxx>

using namespace Vocal2;
using namespace std;

UnknownParameter::UnknownParameter(const char* startName, unsigned int nameSize,
                                   ParseBuffer& pb)
   : DataParameter(ParameterTypes::UNKNOWN, pb),
     mName(startName, nameSize)
{}

UnknownParameter::UnknownParameter(const Data& name)
   : DataParameter(ParameterTypes::UNKNOWN),
     mName(name)
{
}

const Data& 
UnknownParameter::getName()
{
   return mName;
}

Parameter* 
UnknownParameter::clone() const
{
   return new UnknownParameter(*this);
}

ostream& operator<<(ostream& stream, UnknownParameter& comp)
{
   return stream << comp.getName() << "=" << comp.value();
}