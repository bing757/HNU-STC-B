import serial
import serial.tools.list_ports
import threading
import tkinter as tk
from tkinter import ttk, messagebox


class SerialMonitorApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Serial Monitor")

        self.port_label = ttk.Label(master, text="Select Serial Port:")
        self.port_label.pack(pady=5)

        self.port_combobox = ttk.Combobox(master)
        self.port_combobox.pack(pady=5)

        self.connect_button = ttk.Button(master, text="Connect", command=self.connect_serial)
        self.connect_button.pack(pady=5)

        self.output_text = tk.Text(master, wrap=tk.WORD, height=15, width=50)
        self.output_text.pack(pady=5)

        self.key_mapping = {
            1: "up",
            2: "down",
            3: "left",
            4: "right",
            5: "center"
        }

        self.ser = None
        self.read_thread = None
        self.populate_serial_ports()

    def populate_serial_ports(self):
        """Populate the combobox with available serial ports."""
        ports = list(serial.tools.list_ports.comports())
        port_names = [port.device for port in ports]

        if port_names:
            self.port_combobox['values'] = port_names
        else:
            messagebox.showwarning("Warning", "No serial devices found.")

    def connect_serial(self):
        """Connect to the selected serial port."""
        port = self.port_combobox.get()
        if port:
            self.ser = self.connect_to_serial(port)
            if self.ser:
                self.output_text.insert(tk.END, f"Connected to {port}\n")
                self.start_reading()
            else:
                messagebox.showerror("Error", "Failed to connect to serial port.")
        else:
            messagebox.showwarning("Warning", "Please select a port.")

    def connect_to_serial(self, port, baudrate=9600):
        """Attempt to connect to the specified serial device."""
        try:
            ser = serial.Serial(port, baudrate, timeout=1)
            return ser
        except Exception as e:
            print(f"Error connecting to serial port {port}: {e}")
            return None

    def start_reading(self):
        """Start reading data from the serial port in a separate thread."""
        if self.ser and not self.read_thread:
            self.read_thread = threading.Thread(target=self.read_serial)
            self.read_thread.daemon = True
            self.read_thread.start()

    def read_serial(self):
        """Read data from the serial port and update the text widget."""
        while True:
            if self.ser.in_waiting > 0:  # Check if there's data to read
                data = self.ser.read()  # Read one byte
                key_value = ord(data)  # Get the integer value of the byte
                output_message = f"Received data: {data} (Value: {key_value})\n"
                self.master.after(0, self.update_output, output_message)  # Schedule update
                if key_value in self.key_mapping:
                    direction_message = f"Direction: {self.key_mapping[key_value]}\n"
                    self.master.after(0, self.update_output, direction_message)
                else:
                    unknown_message = "Unknown key pressed.\n"
                    self.master.after(0, self.update_output, unknown_message)

    def update_output(self, message):
        """Update the text widget with the received message."""
        self.output_text.insert(tk.END, message)
        self.output_text.see(tk.END)  # Scroll to the end


if __name__ == "__main__":
    root = tk.Tk()
    app = SerialMonitorApp(root)
    root.mainloop()
