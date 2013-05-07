import sbeEvent
from sbeEvent import EventUser

print ( "Hello, sbeevent!" )
s = sbeEvent.Event("TOGGLE_SIM_PAUSE")
sbeEvent.Module.Get().QueueEvent( s, True )
print( sbeEvent.Module.Get() )

class myTest(EventUser):
	def __init__(self):
		print(dir(self))
		self.RegisterForEvent( "TEST" )
	
	def HandleEvent( self, e ):
		print( "EVENTFUCKYEAH!" )


x = myTest()
