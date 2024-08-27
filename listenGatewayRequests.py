import serial
from flask import Flask, render_template_string
import threading
from flask_socketio import SocketIO, emit
import time

app = Flask(__name__)
socketio = SocketIO(app)

# Serial port configuration (adjust as needed)
serial_port = '/dev/ttyUSB0'  # Replace with your serial port
baud_rate = 115200
ser = None

# Data storage
received_data = []

def initialize_serial():
    global ser
    while True:
        try:
            ser = serial.Serial(serial_port, baud_rate)
            print("Serial connection established.")
            break
        except serial.SerialException:
            print("Waiting for serial port to become available...")
            time.sleep(1)

def read_serial_data():
    global ser
    while True:
        try:
            if ser.in_waiting > 0:
                data = ser.readline().decode('utf-8').rstrip()
                with app.app_context():
                    received_data.append(data)
                    socketio.emit('update', {'data': data})
        except serial.SerialException:
            print("Serial connection lost. Attempting to reconnect...")
            initialize_serial()
        except Exception as e:
            print(f"An error occurred: {str(e)}. Attempting to reconnect...")
            initialize_serial()
        time.sleep(0.1)  # Small delay to prevent CPU overuse

def keep_alive():
    global ser
    while True:
        try:
            if ser and ser.is_open:
                ser.write(b'\x00')  # Send a null byte
            time.sleep(5)  # Send keep-alive signal every 5 seconds
        except serial.SerialException:
            print("Keep-alive failed. Attempting to reconnect...")
            initialize_serial()
        except Exception as e:
            print(f"An error occurred during keep-alive: {str(e)}. Attempting to reconnect...")
            initialize_serial()

@app.route('/')
def index():
    template = '''
    <html>
    <head>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/socket.io/4.0.1/socket.io.js"></script>
        <script type="text/javascript">
            var socket = io();
            socket.on('update', function(data) {
                var ul = document.getElementById('log');
                var li = document.createElement('li');
                li.appendChild(document.createTextNode(data.data));
                ul.appendChild(li);
            });
        </script>
    </head>
    <body>
        <h1>Login/Register Log:</h1>
        <ul id="log">
        {% for item in data %}
            <li>{{ item }}</li>
        {% endfor %}
        </ul>
    </body>
    </html>
    '''
    return render_template_string(template, data=received_data)

if __name__ == '__main__':
    initialize_serial()
    serial_thread = threading.Thread(target=read_serial_data)
    serial_thread.start()
    keep_alive_thread = threading.Thread(target=keep_alive)
    keep_alive_thread.start()
    socketio.run(app, debug=True)