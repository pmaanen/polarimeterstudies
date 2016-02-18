### selector module (AnalysisBase.py, name has to match as per in main.py)
import sys
from multiprocessing import Process
from multiprocessing import JoinableQueue as Queue
import os.path
from time import sleep

class AnalysisBase:
    def __init__(self,nworkers):
        self.nproc=nworkers
        self.task_queue=Queue()
        self.done_queue=Queue()
        self.arguments=[]
        return 
    
    def AddFiles(self,files):
        if str(files)==files:
            self.arguments.append(files)
        else:
            for file in files:
                self.arguments.append(file)
                  
    def Init(self): pass
    def Begin(self): pass
    def BeginWorker(self,filename): pass
    def Terminate(self): pass
    def TerminateWorker(self,filename): pass
    def Process(self,filename): pass
    
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
        for arg in self.arguments:
            self.task_queue.put(arg)
        nfiles=len(self.arguments)
        for i in range(self.nproc):
            Process(target=self.Worker).start()
        
        try:    
            while not nfiles==self.done_queue.qsize():
                done=self.done_queue.qsize()
                self.statusBar(done,nfiles)
            self.task_queue.join()
        except NotImplementedError:
            self.task_queue.join()
        self.Terminate()
       
    def statusBar(self,done,total):
        sys.stdout.write('\r')
        sys.stdout.write("[%-20s] %d%%" % ('='*i, 5*i))
        sys.stdout.flush()
        sleep(0.25)
        
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
