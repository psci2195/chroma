// $Id: unprec_dwf_linop_array_w.cc,v 1.9 2005-01-04 06:52:03 edwards Exp $
/*! \file
 *  \brief Unpreconditioned domain-wall linear operator
 */

#include "chromabase.h"
#include "actions/ferm/linop/unprec_dwf_linop_array_w.h"

namespace Chroma 
{ 
  //! Creation routine
  /*! \ingroup fermact
   *
   * \param u_            gauge field   (Read)
   * \param WilsonMass_   DWF height    (Read)
   * \param m_q_          quark mass    (Read)
   */
  void UnprecDWLinOpArray::create(const multi1d<LatticeColorMatrix>& u_, 
				  const Real& WilsonMass_, const Real& m_q_, int N5_)
  {
    WilsonMass = WilsonMass_;
    m_q = m_q_;
    a5  = 1.0;
    N5  = N5_;

    D.create(u_);
//    CoeffWilsr_s = (AnisoP) ? Wilsr_s / xiF_0 : 1;
  }



  //! Apply unpreconditioned domain-wall fermion linear operator
  /*!
   * \ingroup linop
   *
   * The operator acts on the entire lattice
   *
   * \param psi 	  Pseudofermion field     	       (Read)
   * \param isign   Flag ( PLUS | MINUS )   	       (Read)
   */
  void UnprecDWLinOpArray::operator() (multi1d<LatticeFermion>& chi, 
				       const multi1d<LatticeFermion>& psi, 
				       enum PlusMinus isign) const
  {
    START_CODE();

    chi.resize(N5);

    //
    //  Chi   =  D' Psi
    //
    LatticeFermion  tmp;
    Real fact1 = a5*(Nd - WilsonMass) + 1;
    Real fact2 = -0.5*a5;

    switch (isign)
    {
    case PLUS:
      for(int n=0; n < N5; ++n)
      {
	D(tmp, psi[n], isign);

	if (n == 0)
	  chi[n] = fact2*tmp + fact1*psi[n] 
	    + m_q*chiralProjectPlus(psi[N5-1]) - chiralProjectMinus(psi[1]);
	else if (n == N5-1)
	  chi[n] = fact2*tmp + fact1*psi[n] 
	    - chiralProjectPlus(psi[N5-2]) + m_q*chiralProjectMinus(psi[0]);
	else
	  chi[n] = fact2*tmp + fact1*psi[n] 
	    - chiralProjectPlus(psi[n-1]) - chiralProjectMinus(psi[n+1]);
      }          
      break;

    case MINUS:
      for(int n=0; n < N5; ++n)
      {
	D(tmp, psi[n], isign);

	if (n == 0)
	  chi[n] = fact2*tmp + fact1*psi[n] 
	    + m_q*chiralProjectMinus(psi[N5-1]) - chiralProjectPlus(psi[1]);
	else if (n == N5-1)
	  chi[n] = fact2*tmp + fact1*psi[n] 
	    - chiralProjectMinus(psi[N5-2]) + m_q*chiralProjectPlus(psi[0]);
	else
	  chi[n] = fact2*tmp + fact1*psi[n] 
	    - chiralProjectMinus(psi[n-1]) - chiralProjectPlus(psi[n+1]);
      }          
      break;
    }

    END_CODE();
  }

  //! Derivative
  void 
  UnprecDWLinOpArray::deriv(multi1d<LatticeColorMatrix>& ds_u, 
			    const multi1d<LatticeFermion>& chi, const multi1d<LatticeFermion>& psi, 
			    enum PlusMinus isign) const
  {
    ds_u.resize(Nd);
    ds_u = zero;

    multi1d<LatticeColorMatrix> ds_tmp(Nd);
    for(int s(0);s<N5;s++)
    {
      D.deriv(ds_tmp,chi[s],psi[s],isign);
      for(int mu(0);mu<Nd;mu++)
	ds_u[mu] += Real(-0.5)*ds_tmp[mu];
    }
  }

}; // End Namespace Chroma

