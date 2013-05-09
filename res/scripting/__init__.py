import SBE
from SBE import EventUser

print ( "Hello, sbeevent!" )
s = SBE.Event("TOGGLE_SIM_PAUSE")
SBE.Module.Get().QueueEvent( s, True )
print( SBE.Module.Get() )

class myTest(EventUser):
	def __init__(self):
		EventUser.__init__(self)
		self.RegisterForEvent( "SCRIPT_HELLO" )
	
	def HandleEvent( self, e ):
		print( "EVENTFUCKYEAH!" )


x = myTest()
