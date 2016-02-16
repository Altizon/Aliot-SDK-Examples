Ruby - Aliot-Appliance-SDK
==========================

Pull the repository to your workspace. The ruby folder contains a aliot-3.0.0.gem that allows to communicate and send endpoint data to the Datonis Platform

Installing gem in your system:

gem install aliot-3.0.0.gem 

Usage:

Modify the sample.rb file as follows:

1. Add appropriate access_key and secret_key from the downloded key_pair in AliotConfiguration.
2. Add Thing id, Thing name, Thing Description of the thing whose data you want to send to Datonis.
3. Finally add the metrics name and it value in the data field. You can also set waypoints and send it to Datonis.

You can then run example as follows:

ruby sample.rb