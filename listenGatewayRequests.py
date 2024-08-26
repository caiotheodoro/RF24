import serial
from flask import Flask, render_template_string
import threading

app = Flask(__name__)

# Serial port configuration (adjust as needed)
serial_port = '/dev/ttyUSB0'  # Replace with your serial port
baud_rate = 115200
ser = serial.Serial(serial_port, baud_rate)

# Data storage
received_data = []

def read_serial_data():
    while True:
        if ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').rstrip()  # Read bytes directly
            with app.app_context():
                received_data.append(data)

@app.route('/')
def index():
    template = '''
    <html>
    <body>
        <h1>Login/Register Log:</h1>
        <ul>
        {% for item in data %}
            <li>{{ item }}</li>
        {% endfor %}
        </ul>
    </body>
    </html>
    '''
    return render_template_string(template, data=received_data)

if __name__ == '__main__':
    serial_thread = threading.Thread(target=read_serial_data)
    serial_thread.start()
    app.run(debug=True)