### selector module (AnalysisBase.py, name has to match as per in main.py)
from ROOT import TPySelector, gROOT, TH1F, TFile, TH1
from numpy.random import randint
class MyPySelector(TPySelector):
    def __init__(self):
        print "Ran __init__()"

    def Begin(self):
        print 'Ran Begin()'
        #self.xhist=TH1F("random","random",50,-0.5,49.5)
        #self.outlist=self.GetOutputList()
        #self.outlist.Add(self.xhist)
    def SlaveBegin(self, tree):
        print 'py: slave beginning'
        self.xhist=TH1F("random","random",100,50,50)
        self.outlist=self.GetOutputList()
        self.outlist.Add(self.xhist)
        try:
            print self.fChain.GetFile().GetName()
        except StandardError as error:
            print str(error)

    def Process(self, entry):
        self.fChain.GetEntry( entry )
        #print self.fChain.x
        try:
            self.xhist.Fill(self.fChain.x)
        except (StandardError) as error:
            print "Exception occured in event loop!"
            print str(error)
            raise
        return 1

    def SlaveTerminate(self):
        print 'py: slave terminating'

    def Terminate(self):
        print 'py: terminating'
        out=TFile("histos.root","RECREATE")
        for h in self.GetOutputList():
            if isinstance(h,TH1): h.Write()
        

    def Init(self, tree):
        print 'py: initializing'

