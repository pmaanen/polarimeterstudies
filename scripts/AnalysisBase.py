### selector module (AnalysisBase.py, name has to match as per in main.py)
import sys
from multiprocessing import Process,JoinableQueue,Queue
import os.path
from time import sleep,time
import argparse

class AnalysisBase:
    def __init__(self):
        parser = argparse.ArgumentParser(description='Analysis')
        parser.add_argument('-n','--ncores', type=int, help='number of worker processes')
        parser.add_argument('-o','--output', type=str, help='output file')
        parser.add_argument("input", type=str, nargs='+',help="input files")
        self.args = parser.parse_args()
        self.nproc=self.args.ncores
        self.task_queue=JoinableQueue()
        self.done_queue=Queue()
        self.input=[]
        if self.args.output in self.args.input:
            self.args.input.remove(self.args.output)
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
        nfiles=len(self.input)
        while nfiles:
            item=self.done_queue.get()
            dir=self.outfile.mkdir(item[0])
            dir.cd()    
            for elm in item[1]:
                elm.Write()
            self.outfile.Write()
            self.outfile.Close()
    
    def FileLoop(self,filename):
        self.BeginWorker(filename)
        self.Process(filename)
        self.TerminateWorker(filename)
        return
    
    def Worker(self):
        nfiles=0
        while True:
            item=self.task_queue.get()
            if item == None:
                print "Worker exiting after",nfiles,"files"
                self.task_queue.task_done()
                break
            self.FileLoop(item)
            self.task_queue.task_done()
            nfiles+=1
        return
    
    def __call__(self):
        nfiles=len(self.input)
        for arg in self.input:
            self.task_queue.put(arg)
        procs=[Process(target=Worker) for _ in range(self.nproc)]
        for proc in procs:
            self.task_queue.put(None)
        print "Processing",nfiles,"files:",str(self.input)
        for p in procs:
            p.start()
        print "Waiting to join..."
        self.task_queue.join()
        print "Terminating"
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
