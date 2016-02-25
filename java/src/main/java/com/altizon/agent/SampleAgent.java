
package com.altizon.agent;

import io.datonis.sdk.AliotGateway;
import io.datonis.sdk.InstructionHandler;
import io.datonis.sdk.Thing;
import io.datonis.sdk.exception.IllegalThingException;
import io.datonis.sdk.message.AlertType;
import io.datonis.sdk.message.AliotInstruction;

import java.util.Random;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
/**
 * A sample program that sends events to Datonis
 * 
 * @author Ranjit Nair (ranjit@altizon.com)
 */
public class SampleAgent {
    private static final Logger logger = LoggerFactory.getLogger(SampleAgent.class);
    private static int NUM_EVENTS = 50;

    private Thing thing;
    private AliotGateway gateway;

    private SampleAgent() {
        // Construct the Gateway that helps us send various types of events to
        // Datonis
        this.gateway = new AliotGateway();
    }

    /**
     * Main Entry Point in the program
     * 
     * @param args
     * @throws IllegalThingException
     */
    public static void main(String[] args) throws IllegalThingException {
        // First make sure you create an aliot.properties file using your downloaded access and secret keys.
        // A sample file should be available with this package
        // The keys are available in the Datonis platform portal (Please contact your Account Administrator).

        logger.info("Starting the Sample Aliot Agent");
        // Create and Initialize the agent
        SampleAgent agent = new SampleAgent();

        // Start the Gateway
        if (agent.startGateway()) {
            logger.info("Agent started Successfully, setting up bidirectional communication");

            // Setup bidirectional communication - Needs to use MQTT mode for communication
            // Also, it needs your thing to be configured accordingly in the start gateway
            agent.setupBiDirectionalCommunication();

            logger.info("Bidirectional communication is setup with Datonis");

            // Enable this condition if you want to send a few example alerts
            if (false) {
                logger.info("Transmitting Demo Alerts");
                agent.transmitDemoAlerts();
            }

            // Send a few sample simulated data events
            logger.info("Transmitting Demo Data");
            agent.transmitData();
            logger.info("Transmitted Demo data");

            logger.info("Exiting");
            agent.stopGateway();
        } else {
            logger.error("Could not start Sample Aliot Agent. Please check aliot.log for more details");
        }
    }

	private boolean startGateway() {
        try {
            // Decide what the metadata format of your thing should be.
            // This will show up as the thing parameters that you are pushing on Datonis.
            // In this example we will be pushing cpu and memory utilization.
            // We therefore declare them in a JSON schema format.
            // Obviously escaped so that java does not complain
            

            // Create a Thing Object.
            // It is extremely important to have unique Thing keys or collisions may occur and data could get overwritten.
            // Please create a Thing on the Datonis portal and use the key here.
            // Thing names are non-unique, however uniqueness is recommended.
            // Use a logical 'type' to describe the Thing. For instance, System Monitor in this case.
            // Multiple things can exist for a type.
            // This constructor will throw an illegal thing exception if conditions are not met.
            thing = new Thing("t75fd554e6", "LivingRoom", "The living room temperature and humidity device.");

            // Comment the line earlier and un-comment this line if you want this thing to be bi-directional i.e. supports receiving instructions (Note: Only works with MQTT/MQTTs)
            // thing = new Thing("Your Thing's key goes here", "SysMon", "A monitor for CPU and Memory", true);

            // You can register multiple things and send data for them.
            // First add the things and then call register.
            // In this case, there is only a single thing object.
            gateway.addThing(thing);

            gateway.start();
        } catch (IllegalThingException e) {
            logger.error("Could not start the aliot gateway: ", e.getMessage(), e);
            return false;
        }
        return true;
    }

    private void stopGateway() {
        gateway.stop();
    }

    private void setupBiDirectionalCommunication() {
        gateway.setInstructionHandler(new InstructionHandler() {

            @Override
            public void handleInstruction(AliotGateway gateway, AliotInstruction instruction) {
                logger.info("Received instruction for thing: " + instruction.getThingKey()
                        + " from Datonis: " + instruction.getInstruction().toJSONString());
                JSONObject data = new JSONObject();
                data.put("execution_status", "success");
                if (!gateway.transmitAlert(instruction.getAlertKey(), instruction.getThingKey(), AlertType.WARNING, "Demo warning, instruction received and logged!", data)) {
                    logger.error("Could not send Acknowlegement for instruction back to datonis.");
                } else {
                    logger.info("Sent an instruction acknowlegement back to Datonis!");
                }
            }
        });
    }

    private JSONObject getMetric() {
        JSONObject obj = new JSONObject();
        obj.put("temperature", new Random().nextLong());
        obj.put("humidity", new Random().nextDouble());
        return obj;
    }
    
    private JSONArray createWaypoint(double latitude, double longitude) {
        JSONArray waypoint = new JSONArray();
        waypoint.add(latitude);
        waypoint.add(longitude);
        return waypoint;
    }

    private void transmitAlert(AlertType alertType) {
        JSONObject data = new JSONObject();
        data.put("demoKey", "demoValue");

        if (gateway.transmitAlert(thing.getKey(), alertType, "This is an example " + alertType.toString() + " alert!", data)) {
            logger.info("Sent example " + alertType.toString() + " alert!");
        } else {
            logger.error("Could not send example " + alertType.toString() + " alert");
        }
    }

    public void transmitDemoAlerts() {
        transmitAlert(AlertType.INFO);
        transmitAlert(AlertType.WARNING);
        transmitAlert(AlertType.ERROR);
        transmitAlert(AlertType.CRITICAL);
    }

    public void transmitData() throws IllegalThingException {
        int heartbeat = 0;
        for (int count = 1; count <= NUM_EVENTS; count++) {
            // Construct the JSON packet to be sent. This has to match the
            // metadata structure.
            JSONObject data = getMetric();
            //un-comment below line to create waypoints.
            //JSONArray waypoint = createWaypoint(18.52 + Math.random(), 73.85 + Math.random());
            
            // Transmit the data and waypoint. There is also a method to
            // specify the timestamp of the data packet if the transmission is delayed.
            // The syntax is gateway.transmitData(thing, data, waypoint, timestamp)
            // You can transmit, either data or waypoint, or both. Atleast one should be present
            // from data and waypoint. Pass data or waypoint as null, if you don't want to send it.
            // Following are three ways to transmit data.
            // 1st way
            if (!gateway.transmitCompressedData(thing, data, null)) {
                logger.warn("Could not transmit packet : " + count + " value " + data.toJSONString());
            }
            // 2nd way. Note un-comment the commented code for creating the waypoint
            //if (!gateway.transmitData(thing, null, waypoint)) {
            //    logger.warn("Could not transmit packet : " + count + " waypoint " + waypoint.toJSONString());
            //}
            
            // 3rd way. Note un-comment the commented code for creating the waypoint
            //if (!gateway.transmitData(thing, data, waypoint)) {
            //    logger.warn("Could not transmit packet : " + count + " data: " + data.toJSONString()  + ", waypoint: " + waypoint.toJSONString());
            //}
      
    
            heartbeat++;
            if (heartbeat == 300) {
                gateway.transmitHeartbeat();
                heartbeat = 0;
            }
            try {
                Thread.currentThread().sleep(30000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

        }
    }
}

