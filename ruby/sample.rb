require 'aliot/thing'
require 'aliot/aliot_configuration'
require 'aliot/aliot_gateway_http'

#Aliot::AliotConfiguration.new(access_key, secrete_key, protocol = "http", ssl = false, url=nil)
conf = Aliot::AliotConfiguration.new("93a768t53fd3e21tf7cc357c9b3f2f446297d872", "cefdf5e449c93e32567155f6ddct9d47e38tfd57")
#initialize the gateway
gateway = Aliot::AliotGatewayHttp.new(conf)
#Aliot::Thing.new(key, name, description)
thing = Aliot::Thing.new("742923439d", "Compressor 1", "Example Compressor 1", true)
#register the thing
gateway.register(thing)

counter = 0
while true
  if counter == 0
    # Send heart beat in between (optional)
    gateway.send_heart_beat(thing)
  end
  data = {:temperature => rand * 100, :pressure => rand * 100}
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