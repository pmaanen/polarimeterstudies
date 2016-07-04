### selector module (AnalysisBase.py, name has to match as per in main.py)
import sys
from multiprocessing import Process,JoinableQueue,Queue
import os.path
from os import system
from time import sleep,time
import argparse
import subprocess
class ConsumerBase(Process):
    def __init__(self, task_queue, result_queue):
        Process.__init__(self)
        self.task_queue = task_queue
        self.result_queue = result_queue
    def run(self):
        proc_name = self.name
        while True:
            try:
                next_task = self.task_queue.get()
                if next_task=="STOP":
                    # Poison pill means shutdown
                    #print '%s: Exiting' % proc_name
                    self.task_queue.task_done()
                    return
                #print '%s: Running %s' % (proc_name,next_task.filename)
                answer = next_task(self)
                #print '%s: Finished %s' % (proc_name,next_task.filename)
                self.task_queue.task_done()
                self.result_queue.put(answer)
            except Exception as e:
                self.result_queue.put(e)
        return

class Task(object):
    def __init__(self,filename,function):
        self.filename=filename
        self.function=function
    def __call__(self,myWorker):
        if self.function:
            return self.function(self.filename,myWorker)
        else:
            return None
    def __str__(self):
        return self.filename

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
        self.Consumer=ConsumerBase
        if self.args.output in self.args.input:
            self.args.input.remove(self.args.output)
        self.AddFiles(self.args.input)
        self.filesToMerge=[]
        return 
    
    def AddFiles(self,files):
        if str(files)==files:
            self.input.append(files)
        else:
            for file in files:
                self.input.append(file)
                  
    def Init(self): pass
    def Terminate(self):
        nfiles=len(self.input)
        noprob=0
        while True:
            try:
                item=self.done_queue.get(timeout=1)
            except:
                break
            nfiles-=1
            if item is not None:
                if isinstance(item, Exception):
                    print str(item)
                    continue
                self.filesToMerge.append(item+"-histos.root")
                try:
                    self.input.remove(item+".root")
                except ValueError:
                    print "Problem",item
                    continue
        if len(self.input)==0:
            hadd="hadd "+str(self.args.output)
            for f in self.filesToMerge:
                hadd+=" "+f
            with open(os.devnull, 'wb') as devnull:
                system(hadd)
                #subprocess.call(["hadd",hadd])
            with open(os.devnull, 'wb') as devnull:
                for f in self.filesToMerge:
                    subprocess.call(['rm', f])
        return self.input
    
    def __call__(self,function,**kwargs):
        nfiles=len(self.input)
        procs=[self.Consumer(self.task_queue,self.done_queue) for i in range(self.nproc)]
        for arg in self.input:
            self.task_queue.put(Task(arg,function,**kwargs))
        for proc in procs:
            self.task_queue.put("STOP")
        print "Processing",nfiles,"files"
        print "Using",len(procs),"workers"
        for p in procs:
            p.start()
        while True:
            _break=True
            for p in procs:
                if p.is_alive():
                    _break=False
            if _break:
                break
        while True:
            try:
                print str(self.task_queue.get(timeout=1))
            except:
                break
        #self.task_queue.join()
        print "Terminating"
        return self.Terminate()

class TrackerHit:
    def __init__(self,hit):
        #self.detid=hit.detid
        self.edep=hit.edep
#        self.event=hit.event
        self.trackId=hit.trid
        self.x=hit.x
        self.y=hit.y
        self.z=hit.z
        self.time=hit.tof
        self.etot=hit.ekin
        self.particleId=hit.pid

    def __str__(self):
        return str([self.edep,self.trackId,self.x,self.y,self.z,self.time,self.etot,self.particleId])
        
        
class CaloHit:
    def __init__(self,hit):
        self.detid=hit.detid
        self.edep=hit.edep
    def __str__(self):
        return str([self.event,self.detid,self.edep])
def unpack(tree,HitClass):
    res=[]
    for evt in tree:
        res.append(HitClass(evt))
    res.sort(key=lambda evt:evt.event)
    return res

def getOneEvent(EventIndex,EventList):
    result=filter(lambda hit:hit.event==EventIndex,EventList)
    for hit in result:
        EventList.remove(hit)
    return result
