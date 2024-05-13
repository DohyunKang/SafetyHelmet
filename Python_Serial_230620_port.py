import serial
import signal
import sys
from PyQt5.QtWidgets import *
from PyQt5 import uic
from PyQt5.QtCore import QTimer, pyqtSignal, QObject, QThread
from PyQt5.QtGui import QColor, QTextCursor, QTextCharFormat

exit = False

####################################################################################
from fileinput import filename
import os
def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    base_path = getattr(sys, '_MEIPASS', os.path.dirname(os.path.abspath(__file__)))
    return os.path.join(base_path, relative_path)

form = resource_path("Device_UI1.ui")
form_class = uic.loadUiType(form)[0]
####################################################################################


def handler(signum, frame):
    global exit
    exit = True
    print("프로그램을 종료합니다.")
    ser.close()


def Decode(data):
    data = data.decode().strip()
    data_split = data.split('_')
    if len(data_split) == 10:
        try:
            device_number = int(data_split[0])
            status_number = int(data_split[1])
            co2_concentration = float(data_split[2])
            tvoc_concentration = float(data_split[3])
            temperature = float(data_split[4])
            pressure = float(data_split[5])
            altitude = float(data_split[6])
            humidity = float(data_split[7])
            rssi = int(data_split[8])
            elapsed_time = int(data_split[9])

            return [device_number, status_number, co2_concentration, tvoc_concentration, temperature, pressure, altitude, humidity, rssi, elapsed_time]
        except ValueError:
            return
    else:
        return


class SerialReader(QObject):
    dataReceived = pyqtSignal(list)

    def __init__(self, port, baud):
        super().__init__()
        self.port = port
        self.baud = baud

    def read_serial(self):
        ser = serial.Serial(self.port, self.baud, timeout=0)
        while not exit:
            line = ser.readline()
            data_line = Decode(line)
            if data_line:
                self.dataReceived.emit(data_line)


class WindowClass(QMainWindow, form_class):
    def __init__(self):
        super().__init__()
        self.setupUi(self)

        self.timer = QTimer(self)
        self.timer.start(1000)  # 1초 타이머 설정
        self.timer.timeout.connect(self.QTextBrowserUpdate)  # 타이머 시간 다 되면 프로그램 실행하는 코드

        # textBrowsers 속성 초기화
        self.textBrowsers = [
            self.textBrowser_1,
            self.textBrowser_2,
            self.textBrowser_3,
            self.textBrowser_4,
            self.textBrowser_5,
            self.textBrowser_6
        ]

        self.serial_reader = None

    def QTextBrowserUpdate(self):
        pass

    def handle_serial_data(self, data):
        # 데이터 유효성 검사
        if len(data) == 10:
            # 데이터를 문자열로 변환
            data_str = '_'.join(str(item) for item in data)

            # 데이터 분리
            data_split = data_str.split('_')

            device_number = int(data_split[0])
            status_number = int(data_split[1])
            co2_concentration = float(data_split[2])
            tvoc_concentration = float(data_split[3])
            temperature = float(data_split[4])
            pressure = float(data_split[5])
            altitude = float(data_split[6])
            humidity = float(data_split[7])
            rssi = int(data_split[8])
            elapsed_time = int(data_split[9])

            # 센서 데이터 출력
            if device_number >= 1 and device_number <= len(self.textBrowsers):
                textBrowser = self.textBrowsers[device_number - 1]
                textBrowser.clear()  # 기존 내용 지우기

                # Status에 따른 텍스트와 배경색 설정
                if status_number == 0 and elapsed_time < 4000:
                    status_text = "Idle"
                    textBrowser.setStyleSheet("background-color: white;")
                elif status_number == 1 and elapsed_time < 4000:
                    status_text = "Normal"
                    textBrowser.setStyleSheet("background-color: green;")
                elif status_number == 2 and elapsed_time < 4000:
                    status_text = "Accident"
                    textBrowser.setStyleSheet("background-color: red;")
                elif elapsed_time > 4000:
                    status_text = "Disconnected!"
                    textBrowser.setStyleSheet("background-color: gray;")
                else:
                    status_text = "Unknown"
                    textBrowser.setStyleSheet("background-color: gray;")

                co2_text = "CO2 Concentration: {:.2f}".format(co2_concentration)
                if co2_concentration > 1000:
                    co2_text = "<span style='color:red;display:inline-block;width:200px'>{}</span>".format(co2_text)
                else:
                    co2_text = "<span style='display:inline-block;width:200px'>{}</span>".format(co2_text)

                tvoc_text = "TVOC Concentration: {:.2f}".format(tvoc_concentration)
                if tvoc_concentration > 500:
                    tvoc_text = "<span style='color:red;display:inline-block;width:200px'>{}</span>".format(tvoc_text)
                else:
                    tvoc_text = "<span style='display:inline-block;width:200px'>{}</span>".format(tvoc_text)

                temperature_text = "Temperature: {:.2f}".format(temperature)
                if temperature > 40:
                    temperature_text = "<span style='color:red;display:inline-block;width:200px'>{}</span>".format(
                        temperature_text)
                else:
                    temperature_text = "<span style='display:inline-block;width:200px'>{}</span>".format(
                        temperature_text)

                pressure_text = "Pressure: {:.2f}".format(pressure)
                pressure_text = "<span style='display:inline-block;width:200px'>{}</span>".format(pressure_text)

                altitude_text = "Altitude: {:.2f}".format(altitude)
                altitude_text = "<span style='display:inline-block;width:200px'>{}</span>".format(altitude_text)

                humidity_text = "Humidity: {:.2f}".format(humidity)
                if humidity > 80:
                    humidity_text = "<span style='color:blue;display:inline-block;width:200px'>{}</span>".format(
                        humidity_text)
                else:
                    humidity_text = "<span style='display:inline-block;width:200px'>{}</span>".format(humidity_text)

                rssi_text = "RSSI: {}".format(rssi)
                if rssi < -110:
                    rssi_text = "<span style='color:yellow;display:inline-block;width:200px'>{}</span>".format(
                        rssi_text)
                else:
                    rssi_text = "<span style='display:inline-block;width:200px'>{}</span>".format(rssi_text)

                elapsed_time_text = "Elapsed Time: {}".format(elapsed_time)
                if elapsed_time > 4000:
                    elapsed_time_text = "<span style='color:red;display:inline-block;width:200px'>{}</span>".format(
                        elapsed_time_text)
                else:
                    elapsed_time_text = "<span style='display:inline-block;width:200px'>{}</span>".format(
                        elapsed_time_text)

                text = "<span style='font-size:12pt'>{}<br>{}<br>{}<br>{}<br>{}<br>{}<br>{}<br>{}<br>{}</span>".format(
                    co2_text, tvoc_text, temperature_text, pressure_text, altitude_text, humidity_text,
                    rssi_text, elapsed_time_text, status_text)
                textBrowser.insertHtml(text)
                textBrowser.moveCursor(QTextCursor.End)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    myWindow = WindowClass()
    myWindow.show()

    port, ok = QInputDialog.getText(myWindow, "Port Number", "Enter the port number (e.g., 12):")
    if ok:
        port = port.strip()
        port = 'COM' + port
        baud = 9600
        myWindow.serial_reader = SerialReader(port, baud)
        myWindow.serial_reader.dataReceived.connect(myWindow.handle_serial_data)
        thread = QThread()
        myWindow.serial_reader.moveToThread(thread)
        thread.started.connect(myWindow.serial_reader.read_serial)
        thread.start()

    signal.signal(signal.SIGINT, handler)
    sys.exit(app.exec_())
