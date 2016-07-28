require 'aliot/thing'
require 'aliot/aliot_configuration'
require 'aliot/aliot_gateway_http'
require 'aliot/aliot_util'

#Aliot::AliotConfiguration.new(access_key, secrete_key, protocol = "http", ssl = false, url=nil)
conf = Aliot::AliotConfiguration.new("93a768t53fd3e21tf7cc357c9b3f2f446297d872", "cefdf5e449c93e32567155f6ddct9d47e38tfd57")
#initialize the gateway
gateway = Aliot::AliotGatewayHttp.new(conf)
#Aliot::Thing.new(key, name, description)
thing = Aliot::Thing.new("742923439d", "Compressor 1", "Example Compressor 1")
#register the thing
gateway.register(thing)

counter = 0
while true
  if counter == 0
    # Send heart beat in between (optional)
    gateway.send_heart_beat(thing)
  end
  
  #waypoint format : [latitude, longitude]
  timestamp = Time.now.to_i * 1000;

  #send bulk events by creating an array of events
  #for example 2 events 1 second apart
  event1 = thing_data_hash(thing,{:temperature => rand(100), :pressure => rand(100)},[(18.52 + rand/10), (73.85 + rand/10)],timestamp)
  event2 = thing_data_hash(thing,{:temperature => rand(100), :pressure => rand(100)},[(18.52 + rand/10), (73.85 + rand/10)],timestamp + 1000)
  events = []
  events.push(event1,event2)

  gateway.send_bulk_data(thing,events) #send bulk data
  
  counter = (counter + 1) % 5
  sleep(10)
end