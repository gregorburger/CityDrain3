#include "reactor.h"
#include <boost/foreach.hpp>
#include <muParser.h>
#include <QString>
#include <QStringList>
#include <logger.h>

Reactor::Reactor(Flow &volume, bool use_outer_volume)
    : volume(volume), use_outer_volume(use_outer_volume) {
   size_t nc = Flow::countUnits(Flow::concentration);   

   reactor_volume = 1.0;     // default Volume of reactor stretch
   nstep = 10;               // default internal timesteps for calculation
   init_v.resize(nc);
   parsers.resize(nc);
   
   for (size_t c = 0; c < nc; ++c) {
      parsers[c] = new mu::Parser();
      parsers[c]->EnableOptimizer();
      parsers[c]->EnableByteCode();
      for (size_t cvars = 0; cvars < nc; ++cvars) {
          std::string ccname = Flow::getUnitNames(Flow::concentration)[cvars];
          parsers[c]->DefineVar(ccname, &volume[cvars+1]);
          Logger(Debug) << "defining var:" << ccname;
      }
   }
}

Reactor::~Reactor() {
    for (size_t c = 0; c < Flow::countUnits(Flow::concentration); ++c) {
       delete parsers[c];
    }
}

typedef std::pair<std::string, double> kvpair;

bool Reactor::init(int dt, 
                 std::string constants, 
                 std::vector<std::string> conc_formula) {
   QString consts = QString::fromStdString(constants);

   // Actuelle Werte volume mit Startwerten init_v belegen
   volume.clear();
   for (size_t i = 0; i < init_v.size(); ++i) {
       volume[i+1] = init_v[i];
   }

   BOOST_FOREACH(QString cvar, consts.split(",", QString::SkipEmptyParts)) {
       QStringList vardev = cvar.split("=");
       if (vardev.size() != 2) {
           Logger(Error) << "could not parse const definition";
           return false;
       }
       QString cname = vardev[0];
       QString cvalue = vardev[1];
       bool ok;
       double dvalue = cvalue.toDouble(&ok);
       for (size_t i = 0; i < parsers.size(); ++i) {
           parsers[i]->DefineConst(cname.toStdString(), dvalue);
       }

   }

   for (size_t i = 0; i < parsers.size(); ++i) {
       try {
          parsers[i]->SetExpr(conc_formula[i]);
          parsers[i]->Eval();
      } catch (mu::Parser::exception_type exc) {
           Logger(Error) << exc.GetMsg();
           return false;
      }
   }

   acc = 0.000001;

   return true;
   
}

void Reactor::react(Flow &in, int dt) {
    int nc = Flow::countUnits(Flow::concentration);
    Flow oldvol, dvol1, dvol2, errvol, scalvol;
    
    if (use_outer_volume)
        reactor_volume = volume[0];

    double thyd = in[0]/reactor_volume/dt;          // Hydraulische Aufenthaltszeit
    
    if (nstep!=0){
        // *** Primitiv Euler Vorwaertsintegration - nstep= interne Anzahl von Zeitschritten
        
        double idt = dt/static_cast<double>(nstep);     // Interner Zeitschritt
        
        for (int ntstep = 0; ntstep < nstep; ++ntstep) {
            
            // Funktionsaufruf - ableitung d/dt
            for (int i = 0; i < nc; ++i) {                
                dvol1[i+1] = (thyd * (in[i+1] - volume[i+1]) + parsers[i]->Eval());
            }
            
            //Effektive Lg Euler - volume wird upgedatet
            for (int i = 0; i < nc; ++i) {
                volume[i+1] += dvol1[i+1] * idt;
                volume[i+1] = std::max(volume[i+1], acc);
            }
        }
    }
    else
    {
        // *** nstep=0 >> Adaptive timestep - Heun method
        double errmax;
        double hh=dt/2.0;           // Startschrittweite >> minimale Anzahl Steps
        double hhmin=dt/1000.0;     // Minimum Schrittweite = Abbruchkriterium >> maximale Anzahl Steps
        double hhnext;
        double hsum=0.0;
        
        for (;;) {
            // Skalierungsfaktoren fuer maximalen Fehler errmax
            for (int i = 0; i < nc; ++i) {
                dvol1[i+1] = (thyd * (in[i+1] - volume[i+1]) + parsers[i]->Eval());
                scalvol[i+1] = fabs(volume[i+1]) + fabs(hh*dvol1[i+1]) +acc;
            }
            
            for (int i = 0; i < nc; ++i) {oldvol[i+1] = volume[i+1];}
            
            // Adaptiver Zeitschritt
            for (;;) {
                
                for (int i = 0; i < nc; ++i) {volume[i+1] = oldvol[i+1];}
                
                // make step : Heun's method + error O(h3)
                // Erstens: Euler Step
                for (int i = 0; i < nc; ++i) {
                    dvol1[i+1] = (thyd * (in[i+1] - volume[i+1]) + parsers[i]->Eval());
                }
                for (int i = 0; i < nc; ++i) {
                    volume[i+1] += dvol1[i+1] * hh;
                    volume[i+1] = std::max(volume[i+1], acc);
                }
                
                // Zeitens: Ableitung am Ende des Zeitschritts
                for (int i = 0; i < nc; ++i) {
                    dvol2[i+1] = (thyd * (in[i+1] - volume[i+1]) + parsers[i]->Eval());
                }
                
                for (int i = 0; i < nc; ++i) {
                    volume[i+1] = oldvol[i+1] + hh/2.0*(dvol1[i+1]+dvol2[i+1]);
                    volume[i+1] = std::max(volume[i+1], acc);
                }
                
                // Error
                errmax= 0.0;
                for (int i = 0; i < nc; ++i) {
                    errvol[i+1] = hh/2.0*(dvol2[i+1]-dvol1[i+1]);
                    errmax = std::max(fabs(errvol[i+1]/scalvol[i+1]), errmax);
                }
                if (errmax <= acc*100.0) {
                    break;
                } // OK done - regular exit
                
                // else truncation error too large, reduce stepsize
                hh /= 2.0;
                // Emergency exit - caution
                if (hh < hhmin){
                    hh *=2.0;		// reset achieved timestep
                    break;
                }
            }
            
            // if possible increase stepsize (factor 2)
            if (pow(acc/errmax,0.5) > 2.0)
                hhnext = hh*2.0;
            else
                hhnext = hh;
            
            // Check if End of Timestep dt reached
            hsum += hh;
            hh = hhnext;
            if (hsum >= dt) {break;}
        }
        
    }
}
