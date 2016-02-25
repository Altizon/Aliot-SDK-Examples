require 'aliot/thing'
require 'aliot/aliot_configuration'
require 'aliot/aliot_gateway_http'

#Aliot::AliotConfiguration.new(access_key, secrete_key, protocol = "http", ssl = false, url=nil)
conf = Aliot::AliotConfiguration.new("72f2a8322fca8dc4ttt321t49f83at58f8t78t54", "cf5a1eba58756db5ta741t7a25ca686eba3acdaf")
#initialize the gateway
gateway = Aliot::AliotGatewayHttp.new(conf)
#Aliot::Thing.new(key, name, description)
thing = Aliot::Thing.new("3e2b1f8t81", "LivingRoom", "The living room temperature and humidity device.")
#register the thing
gateway.register(thing)

counter = 0
while true
  if counter == 0
    # Send heart beat in between (optional)
    gateway.send_heart_beat(thing)
  end
  data = {:temperature => , :humidity => }
  #waypoint format : [latitude, longitude]
  waypoint = [(18.52 + rand/10), (73.85 + rand/10)];
  timestamp = Time.now.to_i * 1000;
  #we can send the data in three ways
  gateway.send_data(thing, data, nil, timestamp) #1. send only data
  #g.send_data(t, nil, waypoint, timestamp) #2. send only waypoint
  #g.send_data(t, data, waypoint, timestamp) #3. send both data and waypoint
  
  counter = (counter + 1) % 5
  sleep(5)
end