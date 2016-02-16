### selector module (AnalysisBase.py, name has to match as per in main.py)
import sys
from multiprocessing import Process
import Queue
import os.path

class AnalysisBase:
    def __init__(self,nworkers):
        self.nproc=nworkers
        self.q=Queue.Queue(self.nproc)
        self.arguments=[]
        return 
    

    def Init(self):
        pass
    def Begin(self,filename):
        pass
    def Terminate(self,filename):
        pass
    def Process(self,filename):
        pass
    
    def FileLoop(self,filename):
        self.Begin(filename)
        res=self.Process(filename)
        self.Terminate(filename)
        return res
    
    def Worker(self,input):
        for arg in input:
            print "running", arg                                                                                              
            FileLoop(str(arg))
            print "finished", arg
        return
    
    def __call__(self):
        print("running "+str(self.nproc)+" workers")
        for arg in self.arguments:
            self.q.put(arg)
        for i in range(self.nproc):
            Process(target=Worker, args=task_queue).start()
        self.q.join()

class hit:
    def __init__(self,hit):
        #self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event
        self.trackId=hit.trackId
        self.x=hit.x
        self.y=hit.y
        self.z=hit.z
        self.time=hit.time
        self.etot=hit.etot
        self.particleId=hit.particleId
def unpack(tree):
    res=[]
    for evt in tree:
        res.append(hit(evt))
    res.sort(key=lambda evt:evt.event)
    return res

def getOneEvent(EventIndex,EventList):
    result=[]
    while True:
        if len(EventList)==0:
            break
        if EventList[-1].event==EventIndex:
           result.append(EventList.pop())
        else:
            break
    return result
