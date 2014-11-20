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

#pragma link C++ struct trackerhit_t+;
#pragma link C++ struct calorhit_t+;
#pragma link C++ struct vertex_t+;
#pragma link C++ class DetId+;
#pragma link C++ class TrackerId+;
#pragma link C++ struct gen_t+;
#pragma link C++ class std::vector <trackerhit_t>+;
#pragma link C++ class std::vector <calorhit_t>+;
#endif
