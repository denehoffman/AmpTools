#include <cassert>
#include <iostream>
#include <string>
#include <complex>
#include <cstdlib>
#include "CLHEP/Vector/LorentzVector.h"
#include "IUAmpTools/Kinematics.h"
#include "DalitzAmp/BreitWigner.h"

BreitWigner::BreitWigner( const vector< string >& args ) :
Amplitude(args)
{

  assert( args.size() == 4 );

  m_mass = AmpParameter(args[0]);
  m_width = AmpParameter(args[1]);
  m_daughter1 = atoi(args[2].c_str());
  m_daughter2 = atoi(args[3].c_str());
  
  // need to register any free parameters so the framework knows about them
  registerParameter( m_mass );
  registerParameter( m_width );

}


complex< GDouble >
BreitWigner::calcAmplitude( GDouble** pKin ) const {

  HepLorentzVector P1(pKin[m_daughter1-1][1], pKin[m_daughter1-1][2],
                      pKin[m_daughter1-1][3], pKin[m_daughter1-1][0]);

  HepLorentzVector P2(pKin[m_daughter2-1][1], pKin[m_daughter2-1][2],
                      pKin[m_daughter2-1][3], pKin[m_daughter2-1][0]);

  GDouble m = (P1+P2).m();

  complex<GDouble> bwdenominator(m*m - m_mass*m_mass, m_mass*m_width);

  return  complex<GDouble>(1.0,0.0) / bwdenominator;

}



#ifdef GPU_ACCELERATION
void
BreitWigner::launchGPUKernel( dim3 dimGrid, dim3 dimBlock, GPU_AMP_PROTO ) const {
  
  // use integers to endcode the string of daughters -- one index in each
  // decimal place
  
  GPUBreitWigner_exec( dimGrid,  dimBlock, GPU_AMP_ARGS, 
                       m_mass, m_width, m_daughter1, m_daughter2 );

}
#endif //GPU_ACCELERATION


BreitWigner*
BreitWigner::newAmplitude( const vector< string >& args ) const {
  return new BreitWigner( args );
}

BreitWigner*
BreitWigner::clone() const {
  return ( isDefault() ? new BreitWigner() : 
    new BreitWigner( arguments() ) );
}
