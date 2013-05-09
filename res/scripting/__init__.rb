require 'SBE.so'

puts "Hello SBE!"

#SBE::Module.Get().QueueEvent( "TOGGLE_SIM_PAUSE" )
puts SBE::Module.Get()

class Test < SBE::EventUser
	def initialize()
		super
		RegisterForEvent( "SCRIPT_HELLO" )
	end

	def HandleEvent( e )
		print "FUCKINGHELLYEAH!"
	end
end

puts Test.instance_methods
blah = Test.new
puts SBE.constants
#puts blah.RegisterForEventS("TOGGLE_SIM_PAUSE")
