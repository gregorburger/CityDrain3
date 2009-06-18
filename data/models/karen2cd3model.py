#!/usr/bin/env python
# -*- coding: latin-1 -*-
import cd3modelgen as cd3
import sys

class Catchment:
    def __init__(self, name):
        self.name = name
        self.eff1 = -1
        self.eff2 = -1
        self.upstreams = []
        self.nextCon = 1
    
    def makeNodes(self, s):
        
        if self.type == 3:
            self.mixer = cd3.Mixer(len(self.upstreams))
            self.fileout = cd3.FileOut("tmp/WWTP-out.txt")
            s.nodes += [self.mixer, self.fileout]
            s.cons += [cd3.Connection(self.mixer, self.fileout)]
            self.nextCon = 0
            return
        self.mixer = cd3.Mixer(len(self.upstreams)+1)
#        self.rain = cd3.RainRead(self.rain_file)
#        self.catchment = cd3.Catchment()
        self.const = cd3.ConstSource()
        self.cso = cd3.CSO()
        self.sewer = cd3.Sewer()
        self.fileout = cd3.FileOut("tmp/river-out-%s.txt" % self.name)
#        s.nodes+=[self.rain, self.catchment, self.cso, self.mixer, self.sewer, self.fileout]
        s.nodes+=[self.const, self.cso, self.mixer, self.sewer, self.fileout]
        #rain_catch_con = cd3.Connection(self.rain, self.catchment, "out", "rain_in")
        cso_fileout_con = cd3.Connection(self.cso, self.fileout, "overflow", "in")
#        s.cons+=[rain_catch_con, cso_fileout_con]
        s.cons+=[cso_fileout_con]
        if (self.type == 1):
            self.connectType1(s)
        else:
            self.connectType2(s)
        
    def connectType1(self, s):
        catch_mixer = cd3.Connection(self.const, self.mixer, "out", "inputs[0]")
        mixer_cso = cd3.Connection(self.mixer, self.cso)
        cso_sewer = cd3.Connection(self.cso, self.sewer)
        s.cons += [catch_mixer, mixer_cso, cso_sewer]
        pass
        
    def connectType2(self, s):
        catch_cso = cd3.Connection(self.const, self.cso)
        cso_mixer = cd3.Connection(self.cso, self.mixer, "out", "inputs[0]")
        mixer_sewer = cd3.Connection(self.mixer, self.sewer)
        s.cons += [catch_cso, cso_mixer, mixer_sewer]
        pass

def render_catchments(cs, s):
    for c in cs:
        if c.eff1 >= 0:
            cs[c.eff1].upstreams.append(c)
    
    for c in cs:
        c.makeNodes(s)
    
    for i in xrange(1, len(cs)):
        c = cs[i]
        if c.eff1 >= 0:
            down = cs[c.eff1]
            s.cons+=[cd3.Connection(c.sewer, down.mixer, "out", "inputs[%d]" % down.nextCon)]
            down.nextCon += 1
    s.render()


def main():
    if (len(sys.argv) < 2):
        print "usage: karen2cd3model karen_model [sim_class=DefaultSimulation]"
        return
    s = cd3.Simulation("DefaultSimulation", 0, 72000, 300)
    if (len(sys.argv) > 2):
        s.klass = sys.argv[2]
    lines = []
    f = open(sys.argv[1])

    for l in f:
        lines.append(l.strip())
    
    #print len(lines)
    if lines[0] != "Karen":
        print "not a karen file"
        return

    model_begin = 0
    while lines[model_begin] != "":
        model_begin += 1
    model_begin += 1
    rain = lines[model_begin-3]
    numc = int(lines[model_begin-2])
    
    catchments = []
    current_line = model_begin
    
    for i in xrange(numc):
#        print "catchment nummer %d" % i
        ccatch = Catchment(lines[current_line])
        ccatch.type = int(lines[current_line+1])
        ccatch.rain_file = rain
        
        if (ccatch.type != 3):
            ccatch.eff1 = int(lines[current_line+4])
            ccatch.eff2 = int(lines[current_line+5])
            ccatch.inlinecso = float(lines[current_line+5])
            ccatch.areaa = float(lines[current_line+6])     #loadData19new.setArea(kar.readLine().toDouble());
            ccatch.flowtime = float(lines[current_line+7])     #loadData19new.setFlowTime(kar.readLine().toDouble());
            ccatch.initloss= float(lines[current_line+8])      #loadData19new.setInitLoss(kar.readLine().toDouble());
            ccatch.permloss = float(lines[current_line+9])      #loadData19new.setPermLoss(kar.readLine().toDouble());
            ccatch.dryflow = float(lines[current_line+10])      #loadData19new.setDryFlow(kar.readLine().toDouble());
            ccatch.constorm = float(lines[current_line+11])      #loadData19new.setConStorm(kar.readLine().toDouble());
            ccatch.condry = float(lines[current_line+12])      #loadData19new.setConDry(kar.readLine().toDouble());
            ccatch.volume = float(lines[current_line+13])      #loadData19new.setVolume(kar.readLine().toDouble());
            ccatch.reseff = float(lines[current_line+14])      #loadData19new.setResEff(kar.readLine().toDouble());
            ccatch.setcoeff = float(lines[current_line+15])     #loadData19new.setSedCoeff(kar.readLine().toDouble());
            ccatch.flowtimefull = float(lines[current_line+16])      #loadData19new.setFlowTimeFull(kar.readLine().toDouble());
        
        catchments.append(ccatch)
        while lines[current_line] != "":
            current_line += 1
        current_line += 1
#    print current_line
    render_catchments(catchments, s)
        
if __name__ == "__main__":
    main()
