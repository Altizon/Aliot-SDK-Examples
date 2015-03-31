package com.datonis.aliot.sample;

import org.json.simple.JSONObject;

import java.lang.management.ManagementFactory;
import java.util.concurrent.LinkedBlockingQueue;
import com.sun.management.OperatingSystemMXBean;

import com.altizon.aliot.gateway.AliotCommunicator;
import com.altizon.aliot.gateway.AliotConfiguration;
import com.altizon.aliot.gateway.AliotGateway;
import com.altizon.aliot.gateway.AliotMessageListener;
import com.altizon.aliot.gateway.AliotUtil;
import com.altizon.aliot.gateway.BulkData;
import com.altizon.aliot.gateway.entity.Sensor;
import com.altizon.aliot.gateway.exception.AliotException;
import com.altizon.aliot.gateway.exception.IllegalSensorException;
import com.altizon.aliot.gateway.exception.InvalidRequestException;
import com.altizon.aliot.gateway.message.AlertType;
import com.altizon.aliot.gateway.message.AliotInstruction;

/**
 * This is a sample agent that demonstrates how to push Data to Datonis. In this example we will push data as it arrives
 * to the Datonis platform.
 * 
 * For demonstration purpose, we will push CPU and Memory utilization. Please note that the example uses java's
 * OperatingSystemMXBean class which is known to be unreliable across platforms.
 * 
 * @author Ranjit Nair
 * 
 */
public class SampleAgentRealTime implements AliotMessageListener
{
    private static Sensor sensor;
    private static AliotGateway gateway;
    private static LinkedBlockingQueue<AliotInstruction> instructionQueue = new LinkedBlockingQueue<AliotInstruction>();
    private static int NUM_EVENTS = 50;

    /**
     * @param args
     * @throws InvalidRequestException
     * @throws IllegalSensorException
     * @throws AliotException 
     */
    public static void main(String[] args) throws IllegalSensorException, InvalidRequestException, AliotException
    {
        // First construct an AliotConfiguration object using your downloaded access and secret keys.
        // The keys are available in the platform portal under your license plan.
        AliotConfiguration config = new AliotConfiguration("your_access_key", "your_secret_key");

        //If you wish to use bidirectional data transfer, please use MQTT. That can be configured like this
        config = new AliotConfiguration("your_access_key", "your_secret_key", AliotConfiguration.MQTTS);
        
        // Construct the Gateway that helps us send various types of events to Datonis
        gateway = new AliotGateway(config);

        SampleAgentRealTime realTimeAgent = new SampleAgentRealTime();
        realTimeAgent.register();

        if ((config.getProtocol() == AliotConfiguration.MQTT) || config.getProtocol() == AliotConfiguration.MQTTS) {
            // Bi-directional communication is only possible with MQTT/Secure MQTT
            gateway.addMessageListener(realTimeAgent);
            realTimeAgent.startInstructionHandler();
        }

        // Enable this condition if you want to send alerts
        if (false) {
            realTimeAgent.transmitDemoAlerts();
        }

        realTimeAgent.transmitData();
        System.out.println("Exiting");
        System.exit(0);
    }

    private boolean register()
    {

        try
        {
            // Decide what the metadata format of your sensor should be. This will show up as the sensor parameters that
            // you are pushing
            // on Datonis. In this example we will be pushing cpu and memory utilization. We therefore declare them in a
            // JSON schema
            // format
            /*
             * {"cpu": { "type": "number" }, "mem": { "type": "number" } }
             */

            // Obviously escaped so that java does not complain
            String metadata = "{\"cpu\": {\"type\":\"number\"}, \"mem\": {\"type\":\"number\"}}";

            // Create a sensor. It is extremely important to have unique sensor keys or collisions may occur and data
            // could get
            // overwritten. Please create a sensor on the Datonis portal and use the key here. Sensor names are
            // non-unique, however
            // uniqueness is recommended. Use a logical 'type' to describe the sensor. For instance, System Monitor in
            // this case.
            // Multiple sensors can exist for a type.
            // This constructor will throw an illegal sensor execption if conditions are not met.
            sensor = new Sensor("sensor_key", "SysMon", "System Monitor", "A monitor for CPU and Memory", metadata);

            // You can register multiple sensors and send data for them. First add the sensors and then call register.
            // In this case, there is only a single sensor object.
            gateway.addSensor(sensor);
            int registered = gateway.register();
            if (registered != AliotCommunicator.OK)
                return false;

            System.out.println("Registered a sensor");
            // Heartbeats are used to indicate to Datonis that your agent is alive even though it
            // might not be sending data. It is a good practice to send heartbeats every 5 minutes.
            gateway.transmitHeartbeat();
            System.out.println("Transmitted a heartbeat");
        }
        catch (IllegalSensorException e)
        {
            System.out.println(e.getMessage());
            return false;
        }
        catch (InvalidRequestException e)
        {
            System.out.println(e.getMessage());
            return false;
        }
        return true;
    }
    
    private void startInstructionHandler() {
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                
                while (true) {
                    AliotInstruction instruction = instructionQueue.poll();
                    if (instruction != null) {
                        System.out.println("Received instruction for sensor: " + instruction.getSensorKey() + " from Datonis: " + instruction.getInstruction().toJSONString());
                        JSONObject data = new JSONObject();
                        data.put("demoKey", "demoValue");
                        int ret = gateway.transmitAlert(instruction.getAlertKey(), instruction.getSensorKey(), AlertType.WARNING, "Demo warning, instruction received and logged!", data);
                        if (ret != AliotCommunicator.OK) {
                            System.err.println("Could not send Acknowlegement for instruction back to datonis. Error: " + AliotUtil.getMappedErrorMessage(ret));
                        } else {
                            System.out.println("Sent an instruction acknowlegement back to Datonis!");
                        }
                    }
                }
            }
        });
        t.start();
    }

    private JSONObject getSystemInfo()
    {

        OperatingSystemMXBean os = (OperatingSystemMXBean) ManagementFactory.getOperatingSystemMXBean();
        long mem = (os.getFreePhysicalMemorySize() / (1024 * 1024));
        // Not accurate but sufficient for demonstration
        double cpu = ((os.getSystemLoadAverage() / os.getAvailableProcessors()) * 10);

        JSONObject obj = new JSONObject();
        obj.put("cpu", cpu);
        obj.put("mem", mem);
        return obj;
    }
    
    private void transmitAlert(AlertType alertType) {
        JSONObject data = new JSONObject();
        data.put("demoKey", "demoValue");
        
        int ret = gateway.transmitAlert(sensor.getKey(), alertType, "This is an example " + alertType.toString() + " alert!", data);
        if (ret == AliotCommunicator.OK) {
            System.out.println("Sent example " + alertType.toString() + " alert!");
        } else {
            System.err.println("Could not send example " + alertType.toString() + " alert: " + AliotUtil.getMappedErrorMessage(ret));
        }
    }
    
    public void transmitDemoAlerts() {
        System.out.println("Transmitting demo alerts!");
        
        transmitAlert(AlertType.INFO);
        transmitAlert(AlertType.WARNING);
        transmitAlert(AlertType.ERROR);
        transmitAlert(AlertType.CRITICAL);
    }

    public void transmitData() throws IllegalSensorException, InvalidRequestException
    {
        System.out.println("Starting to transmit data");

        int heartbeat = 0;
        for (int count = 1; count <= NUM_EVENTS; count++)
        {
            // Construct the JSON packet to be sent. This has to match the
            // metadata structure.
            JSONObject data = getSystemInfo();
            // Transmit the data. There is also a method to
            // specify the timestamp of the data packet if the tranmission is
            // delayed. The syntax is
            // gateway.transmitData(sensor, data, timestamp)
            long startTime = System.currentTimeMillis();
            int retval = gateway.transmitData(sensor, data);
            System.out.println("Transmitted data in: " + (1.0 * (System.currentTimeMillis() - startTime) / 1000) + " seconds");

            // The communicator returns an OK if the tranmission is successful
            if (retval == AliotCommunicator.OK)
            {
                System.out.println("Transmitted packet : " + count + " value " + data.toJSONString() + " response: " + retval);
            }

            else
            {
                System.out.println("Unable to transmit packet : " + count + " value " + data.toJSONString() + " response: " + retval);
                System.out.println("Reason is : "+getTransmissionError(retval));
            }

            heartbeat++;
            if (heartbeat == 300)
            {
                gateway.transmitHeartbeat();
                heartbeat = 0;
            }
            try
            {
                Thread.currentThread().sleep(5000);
            }
            catch (InterruptedException e)
            {
                e.printStackTrace();
            }

        }

        System.out.println("Transmitted data");

    }

    private String getTransmissionError(int code)
    {
        //There could be several reasons for tranmission failure.
        switch (code)
        {
        case AliotCommunicator.UNAUTHORIZED:
            return "Unauthorized access. Please check your access and secret key";

        //Your license allows you to transmit data at a defined rate. This is thrown when the rate is breached.
        //Please throttle down    
        case AliotCommunicator.EXCESSIVE_RATE:
            return "You are pushing data at a rate that is greater than what your license allows";

        //Happens when the packet sent does not match the JSON metadata.    
        case AliotCommunicator.INVALID_REQUEST:
            return "Request is invalid";

        case AliotCommunicator.FAILED:
            return "Failed to send data. Reasons are unknown";

        case AliotCommunicator.NOT_ACCEPTABLE:
            return "Failed to send data. Request is unacceptable";

        case AliotCommunicator.NO_CONNECTION:
            return "Not connected";

        default:
            return "Failed to send data. The response code is: " + code;

        }

    }

    public void transmitDataInBulk() throws IllegalSensorException, InvalidRequestException
    {
        System.out.println("Starting to transmit data");

        BulkData bulkData = new BulkData();
        int count = 2;
        int eventCount = 0;
        for (; count <= 27; count++)
        {
            long start = System.currentTimeMillis();
            JSONObject value = getSystemInfo();
            bulkData.addData(start, value);
            eventCount++;
            try
            {
                Thread.currentThread().sleep(1000);
            }
            catch (InterruptedException e)
            {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        long start = System.currentTimeMillis();
        int retval = gateway.transmitData(sensor, bulkData);
        System.out.println("Transmitted data: " + (eventCount) + " return code is: " + retval + " transmitted in: " + (System.currentTimeMillis() - start));

    }

    @Override
    public void messageReceived(AliotInstruction instruction) {
        instructionQueue.add(instruction);
    }
}
