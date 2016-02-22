Java - Aliot-Appliance-SDK
==========================

Pull the repository to your workspace. The java folder contains a pom.xml.
First install the SDK jar to your local mvn repository:

The Java and Android SDK has been tested for Java 1.6+ and Android 2.2+. As a prerequisite you will need JDK 1.6+ and (optionally) the Android SDK if you are developing on Android.

The java code sample is available in the java subfolder. The sample code creates a thing that sends CPU and memory usage readings from your machine to Datonis.

Structure of the downloaded repository.
------------------------------------------

If you are familiar with Git, you can pull the contents of the repository down. If not you can download the archive as a repository zip and unzip the file. Once you have the repository downloaded, the java folder contains the java code samples and dependent library files. The following steps will help you get set up.

If you develop with Eclipse, the java folder contains a convenient .project and .classpath file that Eclipse understands. You can directly import the
folder as a new project. Otherwise you will need the following jar files in your java CLASSPATH
1. /java/lib/aliot-agent-sdk-2.0.jar
2. /java/lib/json_simple-1.1.jar
3. /java/lib/mqtt-client-0.4.0.jar

A sample agent is available in java/src/com/datonis/aliot/sample/SampleAgentRealTime.java

Implementing Aliot Agent
------------------------

mvn install:install-file -Dfile=lib/aliot-sdk-3.1.jar -DgroupId=io.datonis.sdk -DartifactId=aliot-sdk -Dversion=3.1 -Dpackaging=jar

mvn clean install

Then, run:

mvn eclipse:eclipse

This should generate .classpath and .project files
Simply import the directory as an Eclipse Java project and you will be good to go.

Non Eclipse users, please include the jar files in the java/lib directory in your classpath and run the example
in the com.altizon.agent package.

You can also generate a package from the command line and then run it as follows:

mvn package

You can then run example as follows:

java -Dlog4j.properties=src/main/resources/log4j.properties -Daliot.properties=src/main/resources/aliot.properties -jar target/Aliot-SDK-Example-1.0-jar-with-dependencies.jar

Configuring the Agent
---------------------

The file src/main/resources/aliot.properties contains the following configuration required for the Agent to work:

1) access and secret key: A key pair with the Agent role is automatically created in your Datonis account. You can use this or create a new one from the Datonis Web UI.

2) protocol: Valid options are 'http' for one way communication or 'mqtt' for bi-directional communication with Datonis. Choosing 'mqtt' will allow your agent to accept instructions from Datonis. Instructions can be fired from the Datonis Web UI.

3) bulk_transmit: Uses batching and compression to optimize data transfers between the agent and Datonis.