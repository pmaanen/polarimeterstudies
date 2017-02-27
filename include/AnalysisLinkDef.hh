/*
 * AnalysisLinkDef.hh
 *
 *  Created on: Jul 25, 2013
 *      Author: maanen
 */


#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class calorhit_t+;
#pragma link C++ class trackerhit_t+;
//#pragma link C++ class DeuteronCarbonElasticScatteringModel;
//#pragma link C++ class DeuteronCarbonBreakupModel;
#pragma link C++ class std::vector <trackerhit_t>+;
#pragma link C++ class std::vector <calorhit_t>+;
#pragma link C++ class std::map<std::string,std::vector<calorhit_t> > +;
#pragma link C++ class std::map<std::string, std::vector<trackerhit_t> > +;
#pragma link C++ class simevent_t+;
#pragma link C++ class std::vector<simevent_t> +;
#pragma link C++ class particle_t +;
#pragma link C++ class std::vector<particle_t> +;
#pragma link C++ class genevent_t +;
#pragma link C++ class std::vector<genevent_t> +;
#pragma link C++ class genvertex_t +;
#pragma link C++ class std::vector<genvertex_t> +;
#endif
/*
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ struct trackerhit_t+;
#pragma link C++ struct calorhit_t+;
#pragma link C++ struct vertex_t+;
#pragma link C++ class DetId+;
#pragma link C++ struct gen_t+;
#pragma link C++ class std::vector <trackerhit_t>+;
#pragma link C++ class std::vector <calorhit_t>+;
#pragma link C++ class std::vector <Double_t>+;
#endif
*/
