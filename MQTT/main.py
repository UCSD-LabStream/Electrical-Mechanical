import os
import json
from flask import Flask, send_from_directory
import paho.mqtt.client as mqtt
from dotenv import load_dotenv
import threading
import logging
import time

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

load_dotenv()

# Initialize Flask app
app = Flask(__name__)

# Configuration
BASE_TOPIC = os.getenv("BASE_TOPIC", "stepper/labstream/motors")
BROKER = "labstream.ucsd.edu"
CONTROL_TOPIC = f"{BASE_TOPIC}/control1"
WEB_PORT = 5000

# Initialize MQTT client

# WebSocket
# client = mqtt.Client(client_id=None, transport="websockets", protocol=mqtt.MQTTv311)
# client.ws_set_options(path="/mqtt")
# client.tls_set()
# client.tls_insecure_set(True)

# Raw TCP
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.tls_set()  # For MQTTS over TLS (port 8883)
client.tls_insecure_set(True) 

# Initialize client connection status
client_connected = False

@app.route('/')
def serve_control_interface():
    return send_from_directory('.', 'index.html')

def on_connect(client, userdata, flags, reason_code, properties=None):

    global client_connected
    if reason_code == 0:
        client_connected = True
        logger.info(f"Connected to MQTT broker, subscribed to {CONTROL_TOPIC}")
        client.subscribe(CONTROL_TOPIC)
    else:
        logger.error(f"Connection failed: {reason_code} - {mqtt.connack_string(reason_code)}")
        client_connected = False

def on_disconnect(client, userdata, disconnect_flags, reason_code, properties=None):

    global client_connected
    client_connected = False
    logger.warning(f"Disconnected from broker: {reason_code}")
    reconnect(client)

def reconnect(client):
    while not client_connected:
        try:
            logger.info("Attempting to reconnect to MQTT broker...")
            client.reconnect()
            time.sleep(5) 
        except Exception as e:
            logger.error(f"Reconnection failed: {str(e)}")
            time.sleep(5)

def publish_motor_command(command, motor=None):
    if client_connected:
        if motor:
            payload = json.dumps({"motor": motor, "command": command})
        else:
            payload = command
        result = client.publish(CONTROL_TOPIC, payload)
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            logger.info(f"Published command: {command}")
        else:
            logger.error(f"Failed to publish: {mqtt.error_string(result.rc)}")
    else:
        logger.error("Cannot publish - not connected to broker")

def run_flask_app():
    logger.info(f"Web interface available at http://localhost:{WEB_PORT}")
    from waitress import serve
    serve(app, host="0.0.0.0", port=WEB_PORT)

def main():
    # Configure MQTT client
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    
    try:
        # if raw TCP works
        client.connect(BROKER, 1883, 60)
        # If only WS
        #client.connect("labstream.ucsd.edu", 9001, keepalive=60)
        
        client.loop_start()

        web_thread = threading.Thread(target=run_flask_app, daemon=True)
        web_thread.start()

        # CLI interface
        while True:
            cmd = input("Enter command [cw/ccw/stop/exit]: ").strip().lower()
            if cmd in ("cw", "ccw", "stop"):
                motor_input = input("Which motor? (1/2) [blank for default 1]: ").strip()
                if motor_input.isdigit():
                    motor = int(motor_input)
                    publish_motor_command(cmd, motor)
                else:
                    publish_motor_command(cmd)
            elif cmd == "exit":
                break
            else:
                logger.warning("Invalid command")
                
    except KeyboardInterrupt:
        logger.info("\nShutting down...")
    except Exception as e:
        logger.error(f"Error: {str(e)}")
    finally:
        client.loop_stop()
        client.disconnect()

if __name__ == "__main__":
    main()