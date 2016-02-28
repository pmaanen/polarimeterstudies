### selector module (AnalysisBase.py, name has to match as per in main.py)
import sys
from multiprocessing import Process
from multiprocessing import JoinableQueue as Queue
import os.path
from time import sleep
import argparse

class AnalysisBase:
    def __init__(self,nworkers=1):
        parser = argparse.ArgumentParser(description='Analysis')
        parser.add_argument('-n','--ncores', type=int, help='number of worker processes')
        parser.add_argument('-o','--output', type=str, help='output file')
        parser.add_argument("input", type=str, nargs='+',help="input files")
        self.args = parser.parse_args()
        self.nproc=self.args.ncores
        self.task_queue=Queue()
        self.done_queue=Queue()
        self.input=[]
        self.AddFiles(self.args.input)
        return 
    
    def AddFiles(self,files):
        if str(files)==files:
            self.input.append(files)
        else:
            for file in files:
                self.input.append(file)
                  
    def Init(self): pass
    def Begin(self): pass
    def BeginWorker(self,filename): pass
    def TerminateWorker(self,filename): pass
    def Process(self,filename): pass
    def Terminate(self):
        try:
            self.outfile.cd()
            while not self.done_queue.empty():
                item=self.done_queue.get()
                dir=self.outfile.mkdir(item[0])
                dir.cd()    
                for elm in item[1]:
                    elm.Write()
            self.outfile.Write()
            self.outfile.Close()
        except:
            pass
    
    def FileLoop(self,filename):
        self.BeginWorker(filename)
        res=self.Process(filename)
        self.TerminateWorker(filename)
        return res
    
    def Worker(self):
        while not self.task_queue.empty():
            item=self.task_queue.get()
            self.FileLoop(item)
            self.task_queue.task_done()
        return
    
    def __call__(self):
        self.Init()
        for arg in self.input:
            self.task_queue.put(arg)
        nfiles=len(self.input)
        for i in range(self.nproc):
            Process(target=self.Worker).start()
        self.task_queue.join()
        self.Terminate()

class TrackerHit:
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
        
        
class CaloHit:
    def __init__(self,hit):
        self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event
    
def unpack(tree,HitClass):
    res=[]
    for evt in tree:
        res.append(HitClass(evt))
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
